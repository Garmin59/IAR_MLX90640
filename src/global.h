
#ifndef _GLOBAL_H
#define _GLOBAL_H


/* Standard includes. */
#include  <stdio.h>
#include  <string.h> 
#include  <intrinsics.h>
#include  <math.h>

/* Scheduler includes. */
#include  "FreeRTOS.h"
#include  "event_groups.h"
#include  "task.h"
#include  "queue.h"
#include  "semphr.h"
#include  "stream_buffer.h"

/* Library includes. */
#include  "stm32f10x.h"
#include  "system_stm32f10x.h"
#include  "stm32f103xB_it.h"
#include  "stm32f10x_flash.h"
#include  "stm32_assert.h"
#include  "misc.h"
#include  "timer.h"

/* Task includes. */
#include  "display.h"
#include  "i2c.h"
#include	"encoder.h"
#include  "menu.h"
#include	"power_control.h"
#include  "adc.h"

/* user includes. */
#include  "init.h"
#include	"palettes.h"
#include	"lcd.h"
#include	"lcd_types.h"
#include	"lcd_fonts.h"
#include  "MLX90640_API.h"
#include  "MLX90640_I2C_Driver.h"


//#include  "tests.h"

//#define   SIMULATION    // use define to use simulation data

// PA1 - ON1 выход 2МГц
#define   POWER_ON      (GPIOA->BSRR = GPIO_BSRR_BS1)
#define   POWER_OFF     (GPIOA->BRR = GPIO_BRR_BR1)

// PA3 - /BTN вход, pull up
#define   BTN_PIN       (GPIOA->IDR_BITS.bit3)
// PA9 - /A вход, pull up, разрешено прерывание
#define   ENC_A_PIN     (GPIOA->IDR_BITS.bit9)
// PA8 - /B вход, pull up, разрешено прерывание
#define   ENC_B_PIN     (GPIOA->IDR_BITS.bit8)

// PA4 - LCD RST выход 10МГц
#define   LCD_RST_0     (GPIOA->BSRR = GPIO_BSRR_BR4)
#define   LCD_RST_1     (GPIOA->BSRR = GPIO_BSRR_BS4)
#define		LCD_RST(a)		(a ?  LCD_RST_1 : LCD_RST_0)


// PB11 - LCD DC Out pp
#define   LCD_DC_0      (GPIOB->BSRR = GPIO_BSRR_BR11)
#define   LCD_DC_1      (GPIOB->BSRR = GPIO_BSRR_BS11)
#define		LCD_DC(a)		  (a ?  LCD_DC_1 : LCD_DC_0)
#define   LCD_DATA      LCD_DC_1
#define   LCD_COMMAND   LCD_DC_0


// определения портов контроллера
// PB10 - LCD CS Out pp
#define   LCD_CS_0      (GPIOB->BSRR = GPIO_BSRR_BR10)
#define   LCD_CS_1      (GPIOB->BSRR = GPIO_BSRR_BS10)
#define		LCD_CS(a)		  (a ?  LCD_CS_1 : LCD_CS_0)

#define		LCD_SCK_0		  (GPIOA->BSRR = GPIO_BSRR_BR5)
#define		LCD_SCK_1		  (GPIOA->BSRR = GPIO_BSRR_BS5)
#define		LCD_SCK(a)		(a ?  LCD_SCK_1 : LCD_SCK_0)

#define		LCD_MOSI_0		(GPIOA->BSRR = GPIO_BSRR_BR7)
#define		LCD_MOSI_1		(GPIOA->BSRR = GPIO_BSRR_BS7)
#define		LCD_MOSI(a)	  (a ?  LCD_MOSI_1 : LCD_MOSI_0)
#define		LCD_MOSI_PIN  (GPIOA->IDR_BITS.bit7)

// PB9 - CHARGE In PU
#define   IS_CHARGE     (!(GPIOB->IDR_BITS.bit9))

// PB12 - /LIGHT Out pp
// PB13 - /LIGHT Out pp
// PB14 - /LIGHT Out pp
// PB15 - /LIGHT Out pp
#define LCD_LIGHT_ON  (GPIOB->BRR = GPIO_BRR_BR12 | GPIO_BRR_BR13 | GPIO_BRR_BR14 | GPIO_BRR_BR15)
#define LCD_LIGHT_OFF (GPIOB->BSRR = GPIO_BSRR_BS12 | GPIO_BSRR_BS13 | GPIO_BSRR_BS14 | GPIO_BSRR_BS15)

// PC12 - RF SET Out pp
#define RF_SET_ON     (GPIOC->BSRR = GPIO_BSRR_BS12)
#define RF_SET_OFF    (GPIOC->BRR = GPIO_BRR_BR12)

// Global events define
// сообщения
#define	M_ENC			      (0x00000001)	// поворот энкодера. Приращение в d.inc.
#define	M_SHORT_PRESS	  (0x00000002)	// короткое нажатие кнопки
#define	M_LONG_PRESS	  (0x00000004)	// длинное нажатие кнопки
#define	M_PUSH_RIGHT	  (0x00000008)	// нажатие кнопки с поворотом вправо 
#define	M_PUSH_LEFT		  (0x00000010)	// нажатие кнопки с поворотом влево
                                      
#define	M_LCD_READY	    (0x00000020)	// менеджер дисплея свободен
#define	M_LCD_RESET     (0x00000040)  // 
                                      
#define	M_POWER_ACTIVE  (0x00000080)  // Переход пульта в активный режим
#define	M_POWER_SLEEP   (0x00000100)  // Засыпание
#define	M_POWER_OFF     (0x00000200)  // Полное выключенние

#define	M_ADC_READY     (0x00000400)  // данные АЦП обновлены

#define	M_DISP_MESSAGE  (0x00000800)  // есть данные для отображения на дисплее
#define	M_IR_READY      (0x00001000)  // данные ИК сенсора обновлены
#define	M_DISP_ON       (0x00002000)  // отображать ИК данные на дисплее
#define	M_DISP_OFF      (0x00004000)  // данные ИК не отображать - работает меню

#define M_WARNING       (0x00008000)




typedef enum _interpolations
{
  INTR_OFF = 0,
  INTR_1,
  INTR_2
} interpolations;

typedef struct _disp_params
{
  s16             min_t;          // мин. температура
  s16             max_t;          // макс. температура
  s16             range;          // диапазон температур
  s16             center_t;       // температура в центре экрана
  u8              min_t_x;        // координата мин. т-ры по X
  u8              min_t_y;        // по Y
  u8              max_t_x;        // координата макс. т-ры по X
  u8              max_t_y;        // по Y
  s16             min_t_setup;    // ручной предел мин. температуры
  s16             max_t_setup;    // ручной предел макс. температуры
  bool            auto_off;       // автоматическое отключение
  bool            abs_t;          // отображение абсолютной температуры
  bool            min_t_auto;     // автоматический выбор предела мин
  bool            min_t_pointer;  // отображать пиксель с мин. температурой
  bool            max_t_auto;     // автоматический выбор предела макс
  bool            max_t_pointer;  // отображать пиксель с макс. температурой 
  u8              chess;          // режим дисплея
  u8              refresh_rate;   // скорость обновления
  interpolations  interpolation;  // режим интерплояции
  palettes        palette;        // палитра цветов
} disp_params;


typedef struct _global
{
  volatile s16		enc;            // переменная энкодера
  volatile bool   btn;		        // состояние кнопки
  bool            charging;       // статус зарядки батареи
  u16             adc_battery;    // АЦП напряжения батареи
  u16             adc_temperature;  // АЦП датчика температуры
  u16             adc_ref;        // АЦП образцового напряжения
  u16			        v_battery;	    // напряжение батареи в тысячных 0-5000 (0-5,000В)
  u16             v_temperature;  // напряжение на датчике температуры
  s8              temperature;    // температура процессора
	u8				      battery;			  // уровень заряда батареи 0-100%
  u32             auto_off_time;  // время автоматического отключения
  ram_data        frame;          // прочитанный фрейм
  paramsMLX90640  par;            // структура из параметров MLX90640
  s16             to[PIXEL_COUNT];      // вычисленные значения температуры 32*24, до 0,1С (273 = 27,3С)
  u16             image[PIXEL_COUNT];   // Цвета пикселей для вывода на экран
  float           temp[PIXEL_COUNT];
  EventGroupHandle_t  events;     // Заголовок системных событий    
  disp_params     disp;           // параметры отображения на дисплее
  /* The queue used to send messages to the LCD task. */
  char            txt[128];
  StreamBufferHandle_t   lcd_stream; // поток сообщений на экран
} global;

extern global g;


extern void assert_failed (unsigned char* pcFile, unsigned long ulLine );




#endif  // _GLOBAL_H
