// adc.h
// работа с аккумулятором и зарядкой


#ifndef	_ADC_POWER_H_
#define	_ADC_POWER_H_

void  ads_task              (void *param);
void	adc_func		          (void);
void	DMA1_Channel1_IRQisr  (void);
void  ADC1_2_IRQisr         (void);



#endif  // _ADC_POWER_H_