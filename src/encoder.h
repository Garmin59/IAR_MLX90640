/******************************************************************************
* Файл Encoder.h
* Определения глобальных функций
******************************************************************************/

#ifndef ENCODER_H
#define ENCODER_H

void	        encoder_init	  (void);
s8	          encoder			    (void);
inline void   btn_procedure   (void);
void          EXTI9_5_IRQisr  (void);

void          button_func     (void);
void          button_task     (void *param);


#endif		// #ifndef ENCODER_H