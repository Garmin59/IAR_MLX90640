/*------------------------------------------------------------------------------
* файл power_control.c
* поддержка управления питанием микроконтроллера и периферии
*
*-----------------------------------------------------------------------------*/

#include  "global.h"


#define   SLEEP_TIME  (5 * 60 * 1000)   // 5 минут до засыпания
#define   OFF_TIME    (8 * 60 * 1000)   // 3 минуты до выключения после засыпания
#define   GUARD_TIME  (10)              // задержка 10мс после сообщения выключения

void power_task (void *param)
{
  (void)param;
  while (1)
  {
    power_control_func ();
    vTaskDelay (1);
  }
}


void power_control_func (void)
{
  typedef enum _power_states
  {
    S_POWER_INIT = 0,
    S_POWER_ACTIVE,
    S_POWER_SLEEP,
    S_POWER_OFF
  } power_states;
  
  static power_states power_state = S_POWER_INIT;
  static u32  power_timer;
  
  switch (power_state)
  {
  case S_POWER_INIT:
    POWER_ON;
    timer_reset (&power_timer);
    xEventGroupSetBits (g.events, M_POWER_ACTIVE);
    power_state = S_POWER_ACTIVE;
    break;
    
  case S_POWER_ACTIVE:
    if (xEventGroupGetBits (g.events) 
        & (M_ENC | M_SHORT_PRESS | M_LONG_PRESS | M_PUSH_RIGHT | M_PUSH_LEFT))
//    if (get_message (M_ENC)
//      || get_message (M_SHORT_PRESS)
//      || get_message (M_LONG_PRESS)
//      || get_message (M_PUSH_RIGHT)
//      || get_message (M_PUSH_LEFT))
    {
      timer_reset (&power_timer);
    }
    if (timer_end (&power_timer, SLEEP_TIME))
    {
      timer_reset (&power_timer);
      xEventGroupSetBits (g.events, M_POWER_SLEEP);
      power_state = S_POWER_SLEEP;
    }
    if (xEventGroupGetBits (g.events) & M_POWER_OFF)
    {
      timer_reset (&power_timer);
      power_state = S_POWER_OFF;
    } 
    break;
    
  case S_POWER_SLEEP:
    if (xEventGroupGetBits (g.events) 
        & (M_ENC | M_SHORT_PRESS | M_LONG_PRESS | M_PUSH_RIGHT | M_PUSH_LEFT))
//     if (get_message (M_ENC)
//      || get_message (M_SHORT_PRESS)
//      || get_message (M_LONG_PRESS)
//      || get_message (M_PUSH_RIGHT)
//      || get_message (M_PUSH_LEFT))
    {
      timer_reset (&power_timer);
      xEventGroupSetBits (g.events, M_POWER_ACTIVE);
      power_state = S_POWER_ACTIVE;
    }
    if (timer_end (&power_timer, OFF_TIME) 
        || (xEventGroupGetBits (g.events) & M_POWER_OFF))
    {
      xEventGroupSetBits (g.events, M_POWER_OFF);
      timer_reset (&power_timer);
      power_state = S_POWER_OFF;
    }       
    break;
    
  case S_POWER_OFF:
    if (timer_end (&power_timer, GUARD_TIME))
    {
      POWER_OFF;
      while (1)
      {
        // endless to power off
      }
    }
    break;

  default:
    timer_reset (&power_timer);
    power_state = S_POWER_INIT;
    break;    
  }
  

}

