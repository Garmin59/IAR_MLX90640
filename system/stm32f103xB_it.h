// stm32f103xB_it.h

#ifndef __STM32F103xB_IT_H
#define __STM32F103xB_IT_H

#include  <stdbool.h>
#include	"stm32f10x.h"				// определения регистров и битовых переменных

#ifdef __cplusplus
 extern "C" {
#endif 
void  nm_isr              (void); // NMI Handler
void  int_default_handler (void); // Reserved
void  xPortSysTickHandler     (void); // SysTick Handler
void  HardFault_Handler   (void);
void  MemManage_Handler   (void);
void  BusFault_Handler    (void);
void  UsageFault_Handler  (void);
void  SVC_Handler         (void);
void  DebugMon_Handler    (void);
void  PendSV_Handler      (void);



#ifdef __cplusplus
}
#endif


#endif  // #ifndef __STM32F103xB_IT_H