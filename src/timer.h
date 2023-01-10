/*
 * timer.h
 *
 *  Created on: 13.9.2012
 *      Author: one
 */

#ifndef TIMER_H_
#define TIMER_H_

// Choose one variant for using timers
//#define BAREMETAL_TIMERS

#ifndef BAREMETAL_TIMERS
  #define FREERTOS_TIMERS
#endif


   
static inline void  timer_reset     (u32 *timer);
static inline bool  timer_end       (u32 *timer, u32 delay);
static inline bool  timer_active    (u32 *timer, u32 delay);
static inline u32	  timer_passed    (u32 *timer);
static inline u32	  system_time			(void);
   
#ifdef BAREMETAL_TIMERS
extern volatile u32 system_timer;	// системный счётчик времени в мс.
#endif

/*******************************************************************************
* прошло времени с начала таймера
*******************************************************************************/
static inline u32 timer_passed (u32 *timer)
{
#ifdef BAREMETAL_TIMERS
	return (system_time () - *timer);
#else
  return (xTaskGetTickCount () - *timer);
#endif
	
}

/*******************************************************************************
* получение системного времени
*******************************************************************************/
static inline u32 system_time (void)
{
#ifdef BAREMETAL_TIMERS
	return (system_timer);
#else
  return (xTaskGetTickCount ());
#endif
}


/*******************************************************************************
* сброс таймера
*******************************************************************************/
static inline void timer_reset (u32 *timer)
{
#ifdef BAREMETAL_TIMERS
	*timer = system_time ();
#else
  *timer = xTaskGetTickCount ();
#endif   
}

/*******************************************************************************
* условие срабатывания таймера
*******************************************************************************/
static inline bool timer_end (u32 *timer, u32 delay)
{
#ifdef BAREMETAL_TIMERS
	return ((system_time () - *timer) >= delay);
#else
  return ((xTaskGetTickCount () - *timer) >= delay);
#endif  
}

/*******************************************************************************
* условие установленного таймера
*******************************************************************************/
static inline bool timer_active (u32 *timer, u32 delay)
{
#ifdef BAREMETAL_TIMERS
	return ((system_time () - *timer) < delay);
#else
  return ((xTaskGetTickCount () - *timer) < delay);
#endif
}



#endif /* TIMER_H_ */
