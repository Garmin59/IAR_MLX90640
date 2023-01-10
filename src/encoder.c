/******************************************************************************
* Файл encoder.c
* функции работы с энкодером
* функция int_8t encoder() возвращает изменение положения
* энкодера после последнего чтения, если энкодер не двигался, то 0
* Работает с прерываниями по порту А и системным таймером 1 мс
******************************************************************************/

#include  "global.h"


#define	BTN_DEBOUNCE 	        (60)	  // время подтверждения состояния кнопки для антидребезга
#define	BUTTON_SHORT_TIME		  (1000)  // граничное время нажатия кнопки


typedef enum _btn_stat
{
	UNPRESS = 0,
	PRESS,
	LONG_PRESS
} btn_stat;


// массив правильных значений переходов 
// между предыдущим и настоящим значением энкодера

// для 2-х переключений за период
static const s8 enc_array [16] =
{
	 0,  0,  0,  0,
	-1,  0,  0,  1,
	 1,  0,  0, -1,
	 0,  0,  0,  0
};

static u16	state		= 0;
static u32	btn_timer	= 0;


/*****************************************************************************
* Настройка энкодера. 
* Ножки на вход с подтяжкой к питанию
* Прерывания на этот порт с активацией по фронту и спаду
* Приоритет высокий
*****************************************************************************/
void encoder_init (void)
{
  // Конфигурация внешних прерываний
  AFIO->EXTICR[3] &= ~AFIO_EXTICR3_EXTI8;
  AFIO->EXTICR[3] |= AFIO_EXTICR3_EXTI8_PA;
  AFIO->EXTICR[3] &= ~AFIO_EXTICR3_EXTI9;
  AFIO->EXTICR[3] |= AFIO_EXTICR3_EXTI9_PA;
	// настройка контроллера прерываний
  EXTI->IMR = EXTI_IMR_MR8  // PA8 - B вход, pull up, разрешено прерывание
            | EXTI_IMR_MR9; // PA9 - A вход, pull up, разрешено прерывание
  EXTI->RTSR = EXTI_RTSR_TR8 // Прерывания А8 по фронту и спаду
            | EXTI_RTSR_TR9; // Прерывания А9 по фронту и спаду
  EXTI->FTSR = EXTI_FTSR_TR8
            | EXTI_FTSR_TR9; // 
	// настройка ножек в файле init.c

	if (ENC_A_PIN)
	{
		state |= 0x01;
	}
	if (ENC_B_PIN)
	{
		state |= 0x02;
	}
	state |= state << 2;
	
	g.enc = 0;
}

/*****************************************************************************
* Прерывание по порту A PA9 - /A вход
* здесь опрашивается состояние энкодера
* результат прерывания сохраняется в переменной
* дополнительно при быстром вращении энкодера изменняются не единицы, а десятки
*****************************************************************************/
#pragma call_graph_root="interrupt"         // interrupt category
void EXTI9_5_IRQisr (void)  // ISR for 5...9 lines
{
  if (EXTI->PR & EXTI_PR_PR8)
  {
    EXTI->PR = EXTI_PR_PR8; // сброс запроса на прерывание
  }
  if (EXTI->PR & EXTI_PR_PR9)
  {
    EXTI->PR = EXTI_PR_PR9; // сброс запроса на прерывание
  }
  
  state <<= 2;
	if (ENC_A_PIN)
	{
		state |= 0x01;
	}
	if (ENC_B_PIN)
	{
		state |= 0x02;										          // считали данные энкодера
	}
	if ((state & 0x03) != ((state >> 2) & 0x03))  // если разные значения с предыдущим
	{
		s8	stt = enc_array [state & 0x0F];	// получили условие из таблицы
		if (stt)											      // при изменении энкодера
		{
			if ((state & 0x03) != ((state >> 4) & 0x03))	// если не вернулся обратно
			{
				g.enc += stt;				// изменим счётчик энкодера на 1
			}
		}
	}
	else	// если одинаковые значения - вернуть state
	{
		state >>= 2;
	}
}

/*****************************************************************************
* Опрос кнопки и анидребезг
* вход - состояние пина кнопки
* выход - переменная btn
*****************************************************************************/
inline void btn_procedure (void)
{
	// опросить состояние кнопки, записать и перевести вывод в состояние ОС
	if (g.btn)	// если состояние - кнопка нажата
	{
		if (BTN_PIN)			// при отпускании кнопки
		{
			if (timer_end (&btn_timer, BTN_DEBOUNCE))  // считаем время, которое отжата для антидребезга
			{
        timer_reset (&btn_timer);	// как досчитали - сбросили таймер 
				g.btn = false;	          // и переключились
			}
		}
		else					// если не досчитали и нажали кнопку
		{
			timer_reset (&btn_timer);	// тогда сбрасываем таймер и считаем заново
		}
	}
	else		// если состояние - кнопка не нажата
	{
		if (BTN_PIN)			// если кнопка отпущена
		{
			timer_reset (&btn_timer);		// тогда сбрасываем таймер и считаем заново
		}
		else					// когда нажали кнопку
		{
			if (timer_end (&btn_timer, BTN_DEBOUNCE))// считаем время, которое нажата для антидребезга
			{
				timer_reset (&btn_timer);	// как досчитали - сбросили таймер 
				g.btn = true;		          // и переключились
			}
		}	
	}
}



/******************************************************************************
* Программа работы с кнопкой
* работает по отпусканию кнопки.
* при нажатии меньше 1 сек M_SHORT_PRESS
* при нажатии больше 1 сек M_LONG_PRESS
* вход - состояние btn, g.enc
* выход - сообщения M_ENC, M_SHORT_PRESS, M_LONG_PRESS, M_PUSH_LEFT, M_PUSH_RIGHT
******************************************************************************/
void button_func (void)
{
	static btn_stat				btn_switch = UNPRESS;
	static __no_init u32	btn_press_time;
  
  btn_procedure ();
  
	switch (btn_switch)
	{
	case UNPRESS:
		if (g.btn)
		{
			timer_reset (&btn_press_time);
			btn_switch = PRESS;
		}
		else if (g.enc)
		{
			xEventGroupSetBits (g.events, M_ENC);
		}
		break;
		
	case PRESS:
		if (!g.btn)
		{
			if (timer_end (&btn_press_time, BUTTON_SHORT_TIME))
			{
				if (g.enc < 0)
				{
					g.enc = 0;
					xEventGroupSetBits (g.events, M_PUSH_LEFT);
				}
				else if (g.enc > 0)
				{
					g.enc = 0;
					xEventGroupSetBits (g.events, M_PUSH_RIGHT);
				}
				else
				{
					xEventGroupSetBits (g.events, M_LONG_PRESS);
				}
			}
			else
			{
				if (g.enc < 0)
				{
					g.enc = 0;
					xEventGroupSetBits (g.events, M_PUSH_LEFT);
				}
				else if (g.enc > 0)
				{
					g.enc = 0;
					xEventGroupSetBits (g.events, M_PUSH_RIGHT);
				}
				else
				{
					xEventGroupSetBits (g.events, M_SHORT_PRESS);
				}
			}
			btn_switch = UNPRESS;
		}
		else
		{
			if (timer_end (&btn_press_time, BUTTON_SHORT_TIME))
			{
				xEventGroupSetBits (g.events, M_LONG_PRESS);
				btn_switch = LONG_PRESS;
			}
		}
		break;
		
	case LONG_PRESS:
		if (!g.btn)
		{
			btn_switch = UNPRESS;
		}
		break;
		
	default:
		btn_switch = UNPRESS;
	}	
}

/******************************************************************************
* Программа работы с кнопкой
* работает по отпусканию кнопки.
* при нажатии меньше 1 сек M_SHORT_PRESS
* при нажатии больше 1 сек M_LONG_PRESS
* вход - состояние btn, g.enc
* выход - сообщения M_ENC, M_SHORT_PRESS, M_LONG_PRESS, M_PUSH_LEFT, M_PUSH_RIGHT
******************************************************************************/
void button_task (void *param)
{
	static btn_stat				btn_switch = UNPRESS;
	static __no_init u32	btn_press_time;
  
  (void) param;
  
  encoder_init ();
  
  while (1)
  {
    btn_procedure ();
    
    switch (btn_switch)
    {
    case UNPRESS:
      if (g.btn)
      {
        timer_reset (&btn_press_time);
        btn_switch = PRESS;
      }
      else if (g.enc)
      {
        xEventGroupSetBits (g.events, M_ENC);
      }
      break;
      
    case PRESS:
      if (!g.btn)
      {
        if (timer_end (&btn_press_time, BUTTON_SHORT_TIME))
        {
          if (g.enc < 0)
          {
            g.enc = 0;
            xEventGroupSetBits (g.events, M_PUSH_LEFT);
          }
          else if (g.enc > 0)
          {
            g.enc = 0;
            xEventGroupSetBits (g.events, M_PUSH_RIGHT);
          }
          else
          {
            xEventGroupSetBits (g.events, M_LONG_PRESS);
          }
        }
        else
        {
          if (g.enc < 0)
          {
            g.enc = 0;
            xEventGroupSetBits (g.events, M_PUSH_LEFT);
          }
          else if (g.enc > 0)
          {
            g.enc = 0;
            xEventGroupSetBits (g.events, M_PUSH_RIGHT);
          }
          else
          {
            xEventGroupSetBits (g.events, M_SHORT_PRESS);
          }
        }
        btn_switch = UNPRESS;
      }
      else
      {
        if (timer_end (&btn_press_time, BUTTON_SHORT_TIME))
        {
          xEventGroupSetBits (g.events, M_LONG_PRESS);
          btn_switch = LONG_PRESS;
        }
      }
      break;
      
    case LONG_PRESS:
      if (!g.btn)
      {
        btn_switch = UNPRESS;
      }
      break;
      
    default:
      btn_switch = UNPRESS;
    }	
    
    vTaskDelay (5);
  }
}





