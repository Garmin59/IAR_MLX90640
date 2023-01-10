/******************** (C) COPYRIGHT 2012 STMicroelectronics ********************
* File Name          : stm32f103xB_it.с
* Author             : Ilya Petrukhin
*******************************************************************************/
#include	<stdint.h>
#include	"stm32f103xB_it.h"

#ifdef BAREMETAL_TIMERS
volatile u32 system_timer = 0;	// системный счётчик времени в мс.
#endif

/* Define to prevent recursive inclusion -------------------------------------*/
// Enable the IAR extensions for this source file.
#pragma language=extended
#pragma segment="CSTACK"

// Forward declaration of the default fault isrs.

__weak void reset_isr              (void); // Reset isr
__weak void nm_isr                 (void); //  NMI isr
__weak void HardFault_isr          (void); //  Hard Fault isr
__weak void MemManage_isr          (void); //  MPU Fault isr
__weak void BusFault_isr           (void); //  Bus Fault isr
__weak void UsageFault_isr         (void); //  Usage Fault isr
__weak void vPortSVCHandler        (void); //  SVCall isr
__weak void DebugMon_isr           (void); //  Debug Monitor isr
__weak void xPortPendSVHandler     (void); //  PendSV isr
__weak void systick_isr            (void); //  SysTick isr
__weak void int_default_isr        (void); //  default isr

// External Interrupts

__weak void WWDG_IRQisr            (void); //  Window Watchdog
__weak void PVD_IRQisr             (void); //  PVD through EXTI Line detect
__weak void TAMPER_IRQisr          (void); //  Tamper
__weak void RTC_IRQisr             (void); //  RTC
__weak void FLASH_IRQisr           (void); //  Flash
__weak void RCC_IRQisr             (void); //  RCC
__weak void EXTI0_IRQisr           (void); //  EXTI Line 0
__weak void EXTI1_IRQisr           (void); //  EXTI Line 1
__weak void EXTI2_IRQisr           (void); //  EXTI Line 2
__weak void EXTI3_IRQisr           (void); //  EXTI Line 3
__weak void EXTI4_IRQisr           (void); //  EXTI Line 4
__weak void DMA1_Channel1_IRQisr   (void); //  DMA1 Channel 1
__weak void DMA1_Channel2_IRQisr   (void); //  DMA1 Channel 2
__weak void DMA1_Channel3_IRQisr   (void); //  DMA1 Channel 3
__weak void DMA1_Channel4_IRQisr   (void); //  DMA1 Channel 4
__weak void DMA1_Channel5_IRQisr   (void); //  DMA1 Channel 5
__weak void DMA1_Channel6_IRQisr   (void); //  DMA1 Channel 6
__weak void DMA1_Channel7_IRQisr   (void); //  DMA1 Channel 7
__weak void ADC1_2_IRQisr          (void); //  ADC1 & ADC2
__weak void USB_HP_CAN1_TX_IRQisr  (void); //  USB High Priority or CAN1 TX
__weak void USB_LP_CAN1_RX0_IRQisr (void); //  USB Low  Priority or CAN1 RX0
__weak void CAN1_RX1_IRQisr        (void); //  CAN1 RX1
__weak void CAN1_SCE_IRQisr        (void); //  CAN1 SCE
__weak void EXTI9_5_IRQisr         (void); //  EXTI Line 9..5
__weak void TIM1_BRK_IRQisr        (void); //  TIM1 Break
__weak void TIM1_UP_IRQisr         (void); //  TIM1 Update
__weak void TIM1_TRG_COM_IRQisr    (void); //  TIM1 Trigger and Commutation
__weak void TIM1_CC_IRQisr         (void); //  TIM1 Capture Compare
__weak void TIM2_IRQisr            (void); //  TIM2
__weak void TIM3_IRQisr            (void); //  TIM3
__weak void TIM4_IRQisr            (void); //  TIM4
__weak void I2C1_EV_IRQisr         (void); //  I2C1 Event
__weak void I2C1_ER_IRQisr         (void); //  I2C1 Error
__weak void I2C2_EV_IRQisr         (void); //  I2C2 Event
__weak void I2C2_ER_IRQisr         (void); //  I2C2 Error
__weak void SPI1_IRQisr            (void); //  SPI1
__weak void SPI2_IRQisr            (void); //  SPI2
__weak void USART1_IRQisr          (void); //  USART1
__weak void USART2_IRQisr          (void); //  USART2
__weak void USART3_IRQisr          (void); //  USART3
__weak void EXTI15_10_IRQisr       (void); //  EXTI Line 15..10
__weak void RTC_Alarm_IRQisr       (void); //  RTC Alarm through EXTI Line
__weak void USBWakeUp_IRQisr       (void); //  USB Wakeup from suspend


// The entry point for the application startup code.
extern void __iar_program_start (void); 
//extern void EXTI_Line0_Intisr(void);
//extern void EXTI_Line6_Intisr(void);
// A union that describes the entries of the vector table.  The union is needed
// since the first entry is the stack pointer and the remainder are function
// pointers.
typedef union
{
    void (*pfnisr)(void);
    void * ulPtr;
}
uVectorEntry;

// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
__root const uVectorEntry __vector_table[] @ ".intvec" =
{
    { .ulPtr = __sfe( "CSTACK" ) }, // The initial stack pointer    
    // Core isr
    reset_isr,        // Reset isr
    nm_isr,           //  NMI isr
    HardFault_isr,    //  Hard Fault isr
    MemManage_isr,    //  MPU Fault isr
    BusFault_isr,     //  Bus Fault isr
    UsageFault_isr,   //  Usage Fault isr
    int_default_isr,  //  Reserved
    int_default_isr,  //  Reserved
    int_default_isr,  //  Reserved
    int_default_isr,  //  Reserved
    vPortSVCHandler,  //  SVCall isr
    DebugMon_isr,     //  Debug Monitor isr
    int_default_isr,  //  Reserved
    xPortPendSVHandler, //  PendSV isr
    systick_isr,      //  SysTick isr
    // External Interrupts
    WWDG_IRQisr,            //  Window Watchdog
    PVD_IRQisr,             //  PVD through EXTI Line detect
    TAMPER_IRQisr,          //  Tamper
    RTC_IRQisr,             //  RTC
    FLASH_IRQisr,           //  Flash
    RCC_IRQisr,             //  RCC
    EXTI0_IRQisr,           //  EXTI Line 0
    EXTI1_IRQisr,           //  EXTI Line 1
    EXTI2_IRQisr,           //  EXTI Line 2
    EXTI3_IRQisr,           //  EXTI Line 3
    EXTI4_IRQisr,           //  EXTI Line 4
    DMA1_Channel1_IRQisr,   //  DMA1 Channel 1
    DMA1_Channel2_IRQisr,   //  DMA1 Channel 2
    DMA1_Channel3_IRQisr,   //  DMA1 Channel 3
    DMA1_Channel4_IRQisr,   //  DMA1 Channel 4
    DMA1_Channel5_IRQisr,   //  DMA1 Channel 5
    DMA1_Channel6_IRQisr,   //  DMA1 Channel 6
    DMA1_Channel7_IRQisr,   //  DMA1 Channel 7
    ADC1_2_IRQisr,          //  ADC1 & ADC2
    USB_HP_CAN1_TX_IRQisr,  //  USB High Priority or CAN1 TX
    USB_LP_CAN1_RX0_IRQisr, //  USB Low  Priority or CAN1 RX0
    CAN1_RX1_IRQisr,        //  CAN1 RX1
    CAN1_SCE_IRQisr,        //  CAN1 SCE
    EXTI9_5_IRQisr,         //  EXTI Line 9..5
    TIM1_BRK_IRQisr,        //  TIM1 Break
    TIM1_UP_IRQisr,         //  TIM1 Update
    TIM1_TRG_COM_IRQisr,    //  TIM1 Trigger and Commutation
    TIM1_CC_IRQisr,         //  TIM1 Capture Compare
    TIM2_IRQisr,            //  TIM2
    TIM3_IRQisr,            //  TIM3
    TIM4_IRQisr,            //  TIM4
    I2C1_EV_IRQisr,         //  I2C1 Event
    I2C1_ER_IRQisr,         //  I2C1 Error
    I2C2_EV_IRQisr,         //  I2C2 Event
    I2C2_ER_IRQisr,         //  I2C2 Error
    SPI1_IRQisr,            //  SPI1
    SPI2_IRQisr,            //  SPI2
    USART1_IRQisr,          //  USART1
    USART2_IRQisr,          //  USART2
    USART3_IRQisr,          //  USART3
    EXTI15_10_IRQisr,       //  EXTI Line 15..10
    RTC_Alarm_IRQisr,       //  RTC Alarm through EXTI Line
    USBWakeUp_IRQisr,       //  USB Wakeup from suspend
};

// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.

#pragma call_graph_root="interrupt"         // interrupt category
void  reset_isr (void) // The reset isr
{
    __iar_program_start ();
}

// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
#pragma call_graph_root="interrupt"         // interrupt category
void  nm_isr (void) // NMI isr
{
    while(1)
    {
    }
}


/*******************************************************************************
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
*******************************************************************************/
#pragma call_graph_root="interrupt"         // interrupt category
void  int_default_isr (void) // Reserved
{
    while (1)
    {
    }
}

/*******************************************************************************
* прерывание системного счётчика времени
*
*******************************************************************************/
#pragma call_graph_root="interrupt"         // interrupt category
void  systick_isr (void) // SysTick isr
{
  //сюда попадаем каждую 1 миллисекунду
#ifdef BAREMETAL_TIMERS
	system_timer++;  
#endif
  xPortSysTickHandler ();
}


#pragma call_graph_root="interrupt"         // interrupt category
void HardFault_isr (void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

#pragma call_graph_root="interrupt"         // interrupt category
void MemManage_isr(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

#pragma call_graph_root="interrupt"         // interrupt category
void BusFault_isr (void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

#pragma call_graph_root="interrupt"         // interrupt category
void UsageFault_isr (void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

//#pragma call_graph_root="interrupt"         // interrupt category
//#pragma weak void vPortSVCHandler (void)
//{
//}

#pragma call_graph_root="interrupt"         // interrupt category
void DebugMon_isr (void)
{
}

//#pragma call_graph_root="interrupt"         // interrupt category
//#pragma weak void xPortPendSVHandler (void)
//{
//}


/*******************************************************************************
*******************************************************************************/

#pragma weak WWDG_IRQisr              = int_default_isr //  Window Watchdog
#pragma weak PVD_IRQisr               = int_default_isr //  PVD through EXTI Line detect
#pragma weak TAMPER_IRQisr            = int_default_isr //  Tamper
#pragma weak RTC_IRQisr               = int_default_isr //  RTC
#pragma weak FLASH_IRQisr             = int_default_isr //  Flash
#pragma weak RCC_IRQisr               = int_default_isr //  RCC
#pragma weak EXTI0_IRQisr             = int_default_isr //  EXTI Line 0
#pragma weak EXTI1_IRQisr             = int_default_isr //  EXTI Line 1
#pragma weak EXTI2_IRQisr             = int_default_isr //  EXTI Line 2
#pragma weak EXTI3_IRQisr             = int_default_isr //  EXTI Line 3
#pragma weak EXTI4_IRQisr             = int_default_isr //  EXTI Line 4
#pragma weak DMA1_Channel1_IRQisr     = int_default_isr //  DMA1 Channel 1
#pragma weak DMA1_Channel2_IRQisr     = int_default_isr //  DMA1 Channel 2
#pragma weak DMA1_Channel3_IRQisr     = int_default_isr //  DMA1 Channel 3
#pragma weak DMA1_Channel4_IRQisr     = int_default_isr //  DMA1 Channel 4
#pragma weak DMA1_Channel5_IRQisr     = int_default_isr //  DMA1 Channel 5
#pragma weak DMA1_Channel6_IRQisr     = int_default_isr //  DMA1 Channel 6
#pragma weak DMA1_Channel7_IRQisr     = int_default_isr //  DMA1 Channel 7
#pragma weak ADC1_2_IRQisr            = int_default_isr //  ADC1 & ADC2
#pragma weak USB_HP_CAN1_TX_IRQisr    = int_default_isr //  USB High Priority or CAN1 TX
#pragma weak USB_LP_CAN1_RX0_IRQisr   = int_default_isr //  USB Low  Priority or CAN1 RX0
#pragma weak CAN1_RX1_IRQisr          = int_default_isr //  CAN1 RX1
#pragma weak CAN1_SCE_IRQisr          = int_default_isr //  CAN1 SCE
#pragma weak EXTI9_5_IRQisr           = int_default_isr //  EXTI Line 9..5
#pragma weak TIM1_BRK_IRQisr          = int_default_isr //  TIM1 Break
#pragma weak TIM1_UP_IRQisr           = int_default_isr //  TIM1 Update
#pragma weak TIM1_TRG_COM_IRQisr      = int_default_isr //  TIM1 Trigger and Commutation
#pragma weak TIM1_CC_IRQisr           = int_default_isr //  TIM1 Capture Compare
#pragma weak TIM2_IRQisr              = int_default_isr //  TIM2
#pragma weak TIM3_IRQisr              = int_default_isr //  TIM3
#pragma weak TIM4_IRQisr              = int_default_isr //  TIM4
#pragma weak I2C1_EV_IRQisr           = int_default_isr //  I2C1 Event
#pragma weak I2C1_ER_IRQisr           = int_default_isr //  I2C1 Error
#pragma weak I2C2_EV_IRQisr           = int_default_isr //  I2C2 Event
#pragma weak I2C2_ER_IRQisr           = int_default_isr //  I2C2 Error
#pragma weak SPI1_IRQisr              = int_default_isr //  SPI1
#pragma weak SPI2_IRQisr              = int_default_isr //  SPI2
#pragma weak USART1_IRQisr            = int_default_isr //  USART1
#pragma weak USART2_IRQisr            = int_default_isr //  USART2
#pragma weak USART3_IRQisr            = int_default_isr //  USART3
#pragma weak EXTI15_10_IRQisr         = int_default_isr //  EXTI Line 15..10
#pragma weak RTC_Alarm_IRQisr         = int_default_isr //  RTC Alarm through EXTI Line
#pragma weak USBWakeUp_IRQisr         = int_default_isr //  USB Wakeup from suspend


/****END OF FILE****/
