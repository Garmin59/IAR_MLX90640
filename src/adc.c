//----------------------------------------------------------------------------
// Файл adc.c
// контроль за напряжением питания и зарядкой аккумулятора
//----------------------------------------------------------------------------

#include  "global.h"

void	adc_init (void);

#define	ADC_START_PERIOD		(1000)	  // период запуска АЦП
#define	V_REF					      (1200)	  // опорное напряжение 1,200В
#define	N_MAX					      (0x0FFF)	// максимальное число от АЦП
#define V_MAX               (4200)    // макс. напряжение батареи
#define V_MIN               (3000)    // мин. напряжение батареи
// temperature sensor
#define T_V25               (1400)    // 1.430V at 25C
#define T_SLOPE             (43)      // 4.3mv/C

static u16 ADC1_result[3];


/*******************************************************************************
* Инициализация АЦП
* Подключение:
* PA2 - VBAT (AF) ADC1 IN2, temp sensor - ADC1 IN16, Vrefint - ADC1_IN17
* PB9 - CHARGE In PU (digital) - inverse
* Режим: 3 инжекторных канала
*******************************************************************************/
void adc_init (void)
{
	ADC1->CR1 = ADC_CR2_JEXTSEL_ADC12_TIM1_TRGO	// Analog watchdog channel select bits 
				| ADC_CR1_EOCIE		  * 0	  // Interrupt enable for EOC                              
				| ADC_CR1_AWDIE		  * 0	  // Analog Watchdog interrupt enable                     
				| ADC_CR1_JEOCIE	  * 0	  // Interrupt enable for injected channels                
				| ADC_CR1_SCAN		  * 1	  // Scan mode on                                            
				| ADC_CR1_AWDSGL	  * 0	  // Enable the watchdog on a single channel in scan mode  
				| ADC_CR1_JAUTO		  * 0	  // Automatic injected group conversion                   
				| ADC_CR1_DISCEN	  * 0	  // Discontinuous mode on regular channels                
				| ADC_CR1_JDISCEN	  * 0	  // Discontinuous mode on injected channels               
				| ADC_CR1_DISCNUM_0_CH	  // Discontinuous mode no channels
        | ADC_CR1_DUALMOD_INDEPENDENT   // Independent mode
				| ADC_CR1_JAWDEN	  * 0	  // Analog watchdog enable on injected channels           
				| ADC_CR1_AWDEN		  * 0;  // Analog watchdog enable on regular channels            
	
	ADC1->CR2 = ADC_CR2_ADON	* 1   // A/D Converter ON / OFF             
				| ADC_CR2_CONT		  * 0   // Continuous Conversion  
        | ADC_CR2_CAL       * 0   // A/D Calibration
        | ADC_CR2_RSTCAL    * 0   // Reset Calibration
				| ADC_CR2_DMA		    * 1   // Direct Memory access mode          
				| ADC_CR2_ALIGN_RIGHT     // Data Alignment 0: Right alignment  
				| ADC_CR2_JEXTSEL_ADC12_TIM1_TRGO // not sel = 0 
				| ADC_CR2_JEXTTRIG	* 0   // External Trigger Conversion mode for injected channels
				| ADC_CR2_EXTSEL_ADC12_SWSTART   // Event Select for regular group = sw start  
				| ADC_CR2_EXTTRIG	  * 0   // External Trigger Conversion mode for regular channels
				| ADC_CR2_JSWSTART	* 0   // Start Conversion of injected channels */
				| ADC_CR2_SWSTART	  * 1   // Start Conversion of regular channels by software trigger
        | ADC_CR2_TSVREFE   * 1;  //  Temperature Sensor and VREFINT Enable */
  
	ADC1->SMPR1 = ADC_SMPR2_SMP16_CKL_2395  // 111: 239.5 cycles
              | ADC_SMPR2_SMP17_CKL_2395; // 111: 239.5 cycles
	ADC1->SMPR2 = ADC_SMPR2_SMP2_CKL_2395;  // 111: 239.5 cycles

  ADC1->SQR3 = ADC_SQR3_SQ1_CH2           // 1-е измерение канал номер 2
              | ADC_SQR3_SQ2_CH16         // 2-е измерение канал номер 16
              | ADC_SQR3_SQ3_CH17;        // 3-е измерение канал номер 17
  ADC1->SQR1 = ADC_SQR1_L_L3;             // 3 измерения 
		
// разрешение прерываний в контроллере прерываний
  
  // настройка DMA1 CH1 для пересылки данных от ADC1
  DMA1_Channel1->CCR = DMA_CCR1_TCIE               * 1 /*!< Transfer complete interrupt enable */
                     | DMA_CCR1_HTIE               * 0 /*!< Half Transfer interrupt enable */
                     | DMA_CCR1_TEIE               * 0 /*!< Transfer error interrupt enable */
                     | DMA_CCR1_DIR_FROM_PERIFERAL     /*!< Data Read from peripheral */
                     | DMA_CCR1_CIRC               * 1 /*!< Circular mode */
                     | DMA_CCR1_PINC               * 0 /*!< Peripheral increment mode */
                     | DMA_CCR1_MINC               * 1 /*!< Memory increment mode */
                     | DMA_CCR1_PSIZE_16
                     | DMA_CCR1_MSIZE_16
                     | DMA_CCR1_PL_MEDIUM
                     | DMA_CCR1_MEM2MEM            * 0;
  DMA1_Channel1->CNDTR = 3;
  DMA1_Channel1->CMAR = (u32)&ADC1_result[0];
  DMA1_Channel1->CPAR = (u32)&ADC1->DR;
  
  DMA1_Channel1->CCR |= DMA_CCR1_EN;  // enable DMA1 ch1

	
}


/*******************************************************************************
* прерывание DMA1 CH1 от АЦП
*
*******************************************************************************/
#pragma call_graph_root = "interrupt"         // interrupt category
void DMA1_Channel1_IRQisr (void)
{
  BaseType_t woken, res;
  
	if (DMA1->ISR & DMA_ISR_TCIF1)
	{
    DMA1->IFCR = DMA_IFCR_CTCIF1;			// сброс флага
    
		g.adc_battery = ADC1_result[0];		// запись значений АЦП
		g.adc_temperature = ADC1_result[1]; 
		g.adc_ref = ADC1_result[2];
    
    woken = pdFALSE;				
    // обработать данные
    res = xEventGroupSetBitsFromISR (g.events, M_ADC_READY, &woken);
    if (res == pdPASS)
    {
      portYIELD_FROM_ISR (woken);
    }
	}
}

#pragma call_graph_root = "interrupt"         // interrupt category
void ADC1_2_IRQisr (void)
{
  if (ADC1->SR & ADC_SR_EOC)
  {
    ADC1->SR &= ~ADC_SR_EOC;
    g.adc_battery = ADC1->DR;
  }
}



void ads_task (void *param)
{
  (void) param;
  
  while (1)
  {
    adc_func ();
    vTaskDelay (100);
  }
  
}


/*******************************************************************************
* функция работы АЦП
* Период запуска - 1000 мс
* После окончания измерения запускается функция обработки 
* Выход - значения напряжения питания, уровень зарядки и статус зарядки
*******************************************************************************/
void adc_func (void)
{
	typedef enum _adc_func_states
	{
		ADC_FUNC_INIT = 0,
    ADC_FUNC_CALIB_START,
    ADC_FUNC_CALIB,
		ADC_FUNC_WORK
	} adc_func_states;
	
	static adc_func_states	adc_func_state = ADC_FUNC_INIT;
	static u32	adc_timer = 0;
	
	switch (adc_func_state)
	{
	case ADC_FUNC_INIT:
    g.charging = false;
    ADC1->CR1 = 0;
    ADC1->CR2 = 0;
    timer_reset (&adc_timer);
		adc_func_state = ADC_FUNC_CALIB_START;
		break;
    
  case ADC_FUNC_CALIB_START:
    if (timer_end (&adc_timer, 2))
    {
      timer_reset (&adc_timer);
      ADC1->CR2 = ADC_CR2_CAL | ADC_CR2_ADON;
      adc_func_state = ADC_FUNC_CALIB;
    }
    break;
    
  case ADC_FUNC_CALIB:
    if (!(ADC1->CR2 & ADC_CR2_CAL) 
        || timer_end (&adc_timer, 2000))
    {
      timer_reset (&adc_timer);
      adc_init ();
      adc_func_state = ADC_FUNC_WORK;
    }
    break;
		
	case ADC_FUNC_WORK:
		if (timer_end (&adc_timer, ADC_START_PERIOD))
		{
			timer_reset (&adc_timer);
			ADC1->CR2 |= ADC_CR2_ADON;	// раз в 1с запустить АЦП
		}

		if (xEventGroupClearBits (g.events, M_ADC_READY))
		{
			g.v_battery = ((u32)g.adc_battery * V_REF * 2) / g.adc_ref;	// вычислить напряжение батареи
			g.battery = ((u32)g.v_battery - V_MIN) * 100 / (V_MAX - V_MIN);  // пока просто линейная ф-я
			// battery_calculate ();				// раз в секунду оценить напряжение батареи   
			g.v_temperature = ((u32)g.adc_temperature * V_REF) / g.adc_ref;
			// Temperature (in °C) = {(V25 - VSENSE) / Avg_Slope} + 25.
			g.temperature = 25 + ((((s32)T_V25 - g.v_temperature) * 10) / (s32)T_SLOPE);
		}
		break;
		
	default:
		adc_func_state = ADC_FUNC_INIT;
		break;
	}
}


						
						
