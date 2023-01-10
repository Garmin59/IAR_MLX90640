/*------------------------------------------------------------------------------
* файл init.c
* инициализация блоков микроконтроллера
*
*-----------------------------------------------------------------------------*/

#include	"global.h"


/*******************************************************************************
* Инициализация выводов микроконтроллера
*
*
*******************************************************************************/
void GPIO_init (void)
{
// разрешение тактирования 
	RCC->AHBENR = RCC_AHBENR_DMA1EN * 1 /*!< DMA1 clock enable */
        | RCC_AHBENR_SRAMEN     * 0   /*!< SRAM interface clock enable */
        | RCC_AHBENR_FLITFEN    * 0   /*!< FLITF clock enable */
        | RCC_AHBENR_CRCEN      * 0   /*!< CRC clock enable */
        | RCC_AHBENR_DMA2EN     * 0   /*!< DMA2 clock enable */
        | RCC_AHBENR_FSMCEN     * 0   /*!< FSMC clock enable */
        | RCC_AHBENR_SDIOEN     * 0;  /*!< SDIO clock enable */

  RCC->APB1ENR = RCC_APB1ENR_TIM2EN * 0 /*!< Timer 2 clock enabled*/
        | RCC_APB1ENR_TIM3EN    * 0     /*!< Timer 3 clock enable */
        | RCC_APB1ENR_WWDGEN    * 0     /*!< Window Watchdog clock enable */
        | RCC_APB1ENR_USART2EN  * 0     /*!< USART 2 clock enable */
        | RCC_APB1ENR_I2C1EN    * 1     /*!< I2C 1 clock enable */
        | RCC_APB1ENR_CAN1EN    * 0     /*!< CAN1 clock enable */
        | RCC_APB1ENR_BKPEN     * 0     /*!< Backup interface clock enable */
        | RCC_APB1ENR_PWREN     * 1     /*!< Power interface clock enable */
        | RCC_APB1ENR_TIM4EN    * 0     /*!< Timer 4 clock enable */
        | RCC_APB1ENR_SPI2EN    * 0     /*!< SPI 2 clock enable */
        | RCC_APB1ENR_USART3EN  * 0     /*!< USART 3 clock enable */
        | RCC_APB1ENR_I2C2EN    * 0     /*!< I2C 2 clock enable */
        | RCC_APB1ENR_USBEN     * 0     /*!< USB Device clock enable */
        | RCC_APB1ENR_TIM5EN    * 0     /*!< Timer 5 clock enable */
        | RCC_APB1ENR_TIM6EN    * 0     /*!< Timer 6 clock enable */
        | RCC_APB1ENR_TIM7EN    * 0     /*!< Timer 7 clock enable */
        | RCC_APB1ENR_SPI3EN    * 0     /*!< SPI 3 clock enable */
        | RCC_APB1ENR_UART4EN   * 1     /*!< UART 4 clock enable */
        | RCC_APB1ENR_UART5EN   * 0     /*!< UART 5 clock enable */
        | RCC_APB1ENR_DACEN     * 0;    /*!< DAC interface clock enable */
    
  RCC->APB2ENR = RCC_APB2ENR_AFIOEN * 1 /*!< Alternate Function I/O clock enable */
        | RCC_APB2ENR_IOPAEN    * 1     /*!< I/O port A clock enable */
        | RCC_APB2ENR_IOPBEN    * 1     /*!< I/O port B clock enable */
        | RCC_APB2ENR_IOPCEN    * 1     /*!< I/O port C clock enable */
        | RCC_APB2ENR_IOPDEN    * 1     /*!< I/O port D clock enable */
        | RCC_APB2ENR_ADC1EN    * 1     /*!< ADC 1 interface clock enable */
        | RCC_APB2ENR_ADC2EN    * 0     /*!< ADC 2 interface clock enable */
        | RCC_APB2ENR_TIM1EN    * 0     /*!< TIM1 Timer clock enable */
        | RCC_APB2ENR_SPI1EN    * 1     /*!< SPI 1 clock enable */
        | RCC_APB2ENR_USART1EN  * 0     /*!< USART1 clock enable */
        | RCC_APB2ENR_IOPEEN    * 0     /*!< I/O port E clock enable */
        | RCC_APB2ENR_IOPFEN    * 0     /*!< I/O port F clock enable */
        | RCC_APB2ENR_IOPGEN    * 0     /*!< I/O port G clock enable */
        | RCC_APB2ENR_TIM8EN    * 0     /*!< TIM8 Timer clock enable */
        | RCC_APB2ENR_ADC3EN    * 0;    /*!< DMA1 clock enable */
	__DSB();
	
// ------------------------ Порт А
// PA0 - 
// PA1 - ON1 выход 2МГц
// PA2 - VBAT (AF) ADC1 IN2
// PA3 - /BTN вход, pull up
// PA4 - LCD RST выход 10МГц
// PA5 - LCD SCK (AF) SPI1 SCK
// PA6 - 
// PA7 - LCD MOSI (AF PP) SPI1 MOSI
// PA8 - /B вход, pull up, не разрешено прерывание
// PA9 - /A вход, pull up, разрешено прерывание
// PA10 - USB ON выход 10МГц
// PA11 - USB DM (AF PP)
// PA12 - USB DP (AF PP)
// PA13 - SWDIO 
// PA14 - SWCLK 
// PA15 - 
	GPIOA->CRL = GPIO_CRL_MODE0_INPUT     | GPIO_CRL_CNF0_IN_FL
            | GPIO_CRL_MODE1_OUTPUT2M   | GPIO_CRL_CNF1_OUT_GP_PP
            | GPIO_CRL_MODE2_INPUT      | GPIO_CRL_CNF2_IN_AN
            | GPIO_CRL_MODE3_INPUT      | GPIO_CRL_CNF3_IN_PUPD
            | GPIO_CRL_MODE4_OUTPUT10M  | GPIO_CRL_CNF4_OUT_GP_PP
            | GPIO_CRL_MODE5_OUTPUT50M  | GPIO_CRL_CNF5_OUT_AF_PP
            | GPIO_CRL_MODE6_INPUT      | GPIO_CRL_CNF6_IN_FL
            | GPIO_CRL_MODE7_OUTPUT50M  | GPIO_CRL_CNF7_OUT_AF_PP;
  GPIOA->CRH = GPIO_CRH_MODE8_INPUT     | GPIO_CRH_CNF8_IN_PUPD
            | GPIO_CRH_MODE9_INPUT      | GPIO_CRH_CNF9_IN_PUPD
            | GPIO_CRH_MODE10_INPUT     | GPIO_CRH_CNF10_IN_FL
            | GPIO_CRH_MODE11_INPUT     | GPIO_CRH_CNF11_IN_FL
            | GPIO_CRH_MODE12_INPUT     | GPIO_CRH_CNF12_IN_FL
            | GPIO_CRH_MODE13_INPUT     | GPIO_CRH_CNF13_IN_FL
            | GPIO_CRH_MODE14_INPUT     | GPIO_CRH_CNF14_IN_FL
            | GPIO_CRH_MODE15_INPUT     | GPIO_CRH_CNF15_IN_FL;
  GPIOA->ODR = 0x031A;  // 0b0000 0011 0001 1010;  // Pull up - pull down settings
  
//------------------------- Порт B
// PB0 - 
// PB1 - 
// PB2 - 
// PB3 - 
// PB4 - 	
// PB5 - 
// PB6 - SCL (AF OD) I2C1 SCL
// PB7 - SDA (AF OD) I2C1 SDA
// PB8 - 
// PB9 - CHARGE In PU
// PB10 - LCD CS Out pp
// PB11 - LCD DC Out pp
// PB12 - /LIGHT Out pp
// PB13 - /LIGHT Out pp
// PB14 - /LIGHT Out pp
// PB15 - /LIGHT Out pp
  GPIOB->CRL = GPIO_CRL_MODE0_INPUT     | GPIO_CRL_CNF0_IN_FL
            | GPIO_CRL_MODE1_INPUT      | GPIO_CRL_CNF1_IN_FL
            | GPIO_CRL_MODE2_INPUT      | GPIO_CRL_CNF2_IN_FL
            | GPIO_CRL_MODE3_INPUT      | GPIO_CRL_CNF3_IN_FL
            | GPIO_CRL_MODE4_INPUT      | GPIO_CRL_CNF4_IN_FL
            | GPIO_CRL_MODE5_INPUT      | GPIO_CRL_CNF5_IN_FL
            | GPIO_CRL_MODE6_OUTPUT10M  | GPIO_CRL_CNF6_OUT_AF_OD
            | GPIO_CRL_MODE7_OUTPUT10M  | GPIO_CRL_CNF7_OUT_AF_OD;
  GPIOB->CRH = GPIO_CRH_MODE8_INPUT     | GPIO_CRH_CNF8_IN_FL
            | GPIO_CRH_MODE9_INPUT      | GPIO_CRH_CNF9_IN_PUPD
            | GPIO_CRH_MODE10_OUTPUT10M | GPIO_CRH_CNF10_OUT_GP_PP
            | GPIO_CRH_MODE11_OUTPUT10M | GPIO_CRH_CNF11_OUT_GP_PP
            | GPIO_CRH_MODE12_OUTPUT2M  | GPIO_CRH_CNF12_OUT_GP_PP
            | GPIO_CRH_MODE13_OUTPUT2M  | GPIO_CRH_CNF13_OUT_GP_PP
            | GPIO_CRH_MODE14_OUTPUT2M  | GPIO_CRH_CNF14_OUT_GP_PP
            | GPIO_CRH_MODE15_OUTPUT2M  | GPIO_CRH_CNF15_OUT_GP_PP;
  GPIOB->ODR = 0xF2C0;  // 0b1111 0010 1100 0000;  // Pull up - pull down settings

  
// ----------------------- Порт C
// PC0 - 
// PC1 - 
// PC2 - 
// PC3 - 
// PC4 - 
// PC5 - 
// PC6 - 
// PC7 - 
// PC8 - 
// PC9 - 
// PC10 - RF RXD (AF PP) UART4 TX
// PC11 - RF TXD (AF PP) UART4 RX
// PC12 - RF SET Out pp
// PC13 - 
// PC14 - 
// PC15 - 
	GPIOC->CRL = GPIO_CRL_MODE0_INPUT     | GPIO_CRL_CNF0_IN_FL
            | GPIO_CRL_MODE1_INPUT      | GPIO_CRL_CNF1_IN_FL
            | GPIO_CRL_MODE2_INPUT      | GPIO_CRL_CNF2_IN_FL
            | GPIO_CRL_MODE3_INPUT      | GPIO_CRL_CNF3_IN_FL
            | GPIO_CRL_MODE4_INPUT      | GPIO_CRL_CNF4_IN_FL
            | GPIO_CRL_MODE5_INPUT      | GPIO_CRL_CNF5_IN_FL
            | GPIO_CRL_MODE6_INPUT      | GPIO_CRL_CNF6_IN_FL
            | GPIO_CRL_MODE7_INPUT      | GPIO_CRL_CNF7_IN_FL;
  GPIOC->CRH = GPIO_CRH_MODE8_INPUT     | GPIO_CRH_CNF8_IN_FL
            | GPIO_CRH_MODE9_INPUT      | GPIO_CRH_CNF9_IN_FL
            | GPIO_CRH_MODE10_OUTPUT2M  | GPIO_CRH_CNF10_OUT_AF_PP
            | GPIO_CRH_MODE11_OUTPUT2M  | GPIO_CRH_CNF11_OUT_AF_PP
            | GPIO_CRH_MODE12_INPUT     | GPIO_CRH_CNF12_IN_FL
            | GPIO_CRH_MODE13_INPUT     | GPIO_CRH_CNF13_IN_FL
            | GPIO_CRH_MODE14_INPUT     | GPIO_CRH_CNF14_IN_FL
            | GPIO_CRH_MODE15_INPUT     | GPIO_CRH_CNF15_IN_FL;
  GPIOC->ODR = 0x0000;  // 0b0000 0000 0000 0000;  // Pull up - pull down settings
  
// ----------------------- Порт D
// PD2 - RF CS Out pp
	
	GPIOD->CRL = GPIO_CRL_MODE0_INPUT     | GPIO_CRL_CNF0_IN_FL
            | GPIO_CRL_MODE1_INPUT      | GPIO_CRL_CNF1_IN_FL
            | GPIO_CRL_MODE2_OUTPUT10M  | GPIO_CRL_CNF2_OUT_GP_PP
            | GPIO_CRL_MODE3_INPUT      | GPIO_CRL_CNF3_IN_FL
            | GPIO_CRL_MODE4_INPUT      | GPIO_CRL_CNF4_IN_FL
            | GPIO_CRL_MODE5_INPUT      | GPIO_CRL_CNF5_IN_FL
            | GPIO_CRL_MODE6_INPUT      | GPIO_CRL_CNF6_IN_FL
            | GPIO_CRL_MODE7_INPUT      | GPIO_CRL_CNF7_IN_FL;
  GPIOD->CRH = GPIO_CRH_MODE8_INPUT     | GPIO_CRH_CNF8_IN_FL
            | GPIO_CRH_MODE9_INPUT      | GPIO_CRH_CNF9_IN_FL
            | GPIO_CRH_MODE10_INPUT     | GPIO_CRH_CNF10_IN_FL
            | GPIO_CRH_MODE11_INPUT     | GPIO_CRH_CNF11_IN_FL
            | GPIO_CRH_MODE12_INPUT     | GPIO_CRH_CNF12_IN_FL
            | GPIO_CRH_MODE13_INPUT     | GPIO_CRH_CNF13_IN_FL
            | GPIO_CRH_MODE14_INPUT     | GPIO_CRH_CNF14_IN_FL
            | GPIO_CRH_MODE15_INPUT     | GPIO_CRH_CNF15_IN_FL;
  GPIOD->ODR = 0x0002;  // 0b0000 0000 0000 0010;  // Pull up - pull down settings
}



/*******************************************************************************
* Разрешение прерываний и их приоритеты
*
*******************************************************************************/
void init_irq (void)
{
  NVIC_PriorityGroupConfig (NVIC_PriorityGroup_4);
	/* Enable and configure RCC global IRQ channel, will be used to manage HSE ready 
	and PLL ready interrupts. 
	These interrupts are enabled in stm32f10x_it.c file **********************/
  NVIC_SetPriority (RCC_IRQn, configMAX_PRIORITIES + 1);     // 
  NVIC_SetPriority (SPI1_IRQn, configMAX_PRIORITIES + 1);    // SPI1 (display)
  NVIC_SetPriority (UART4_IRQn, configMAX_PRIORITIES + 2);	  // UART4_IRQn (RF module)
  NVIC_SetPriority (I2C1_EV_IRQn, configMAX_PRIORITIES + 3); // I2C event
  NVIC_SetPriority (ADC1_2_IRQn, configMAX_PRIORITIES + 2);  // ADC1 power batt
  NVIC_SetPriority (DMA1_Channel1_IRQn, configMAX_PRIORITIES + 2);  // ADC1 power batt
  NVIC_SetPriority (EXTI9_5_IRQn, configMAX_PRIORITIES + 4);  // EXT encoder interrupt
  
  SysTick->LOAD  = (uint32_t)((36000000 / 1000) - 1UL);	// конфигурация системного таймера 1000 Гц (1 мс)
  SysTick->VAL   = 0UL;                                  /* Load the SysTick Counter Value */
  NVIC_SetPriority (SysTick_IRQn, configMAX_PRIORITIES); //
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE   * 1   // AHB clock
                   | SysTick_CTRL_TICKINT   * 1   // enable SysTick IRQ
                   | SysTick_CTRL_ENABLE    * 1;  // Enable SysTick IRQ and SysTick Timer */

  NVIC_EnableIRQ (SysTick_IRQn);  // Разрешить прерывания systick
  
  NVIC_EnableIRQ (EXTI9_5_IRQn);  // Разрешить прерывания энкодера
  NVIC_EnableIRQ (DMA1_Channel1_IRQn);   // Разрешить прерывания DMA1 CH1 ADC1
  NVIC_EnableIRQ (ADC1_2_IRQn);   // Разрешить прерывания ADC1

}


