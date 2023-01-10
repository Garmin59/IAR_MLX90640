/*----------------------------------------------------------------------------*/


#include "global.h"


/* Task priorities. */
#define DISP_TASK_PRIORITY        (tskIDLE_PRIORITY + 3)
#define LCD_TASK_PRIORITY         (tskIDLE_PRIORITY)
#define I2C_TASK_PRIORITY         (tskIDLE_PRIORITY + 2)
#define BTN_TASK_PRIORITY         (tskIDLE_PRIORITY + 2)
#define MENU_TASK_PRIORITY        (tskIDLE_PRIORITY + 4)
#define PWR_TASK_PRIORITY         (tskIDLE_PRIORITY + 4)
#define ADC_TASK_PRIORITY         (tskIDLE_PRIORITY + 5)

/*-----------------------------------------------------------*/
#define TCB_SIZE        (32)
#define IDLE_STACK      (64)
#define TIMER_STACK     (64)
#define DISP_STACK      (128)
#define LCD_STACK       (128)
#define I2C_STACK       (128)
#define BTN_STACK       (128)
#define MENU_STACK      (128)
#define PWR_STACK       (128)
#define ADC_STACK       (64)

global g;

static StaticTask_t     disp_buffer;
static StaticTask_t     lcd_buffer;
static StaticTask_t     i2c_buffer;
static StaticTask_t     btn_buffer;
static StaticTask_t     menu_buffer;
static StaticTask_t     pwr_buffer;
static StaticTask_t     adc_buffer;
static TaskHandle_t     disp_handle = NULL;
static TaskHandle_t     lcd_handle = NULL;
static TaskHandle_t     i2c_handle = NULL;
static TaskHandle_t     btn_handle = NULL;
static TaskHandle_t     menu_handle = NULL;
static TaskHandle_t     pwr_handle = NULL;
static TaskHandle_t     adc_handle = NULL;
static u32              disp_stack[DISP_STACK + TCB_SIZE];  // size include tcb (92) and stack
static u32              lcd_stack[LCD_STACK + TCB_SIZE];  // size include tcb (92) and stack
static u32              i2c_stack[I2C_STACK + TCB_SIZE];  // size include tcb (92) and stack
static u32              btn_stack[BTN_STACK + TCB_SIZE];  // size include tcb (92) and stack
static u32              menu_stack[MENU_STACK + TCB_SIZE];  // size include tcb (92) and stack
static u32              pwr_stack[PWR_STACK + TCB_SIZE];  // size include tcb (92) and stack
static u32              adc_stack[ADC_STACK + TCB_SIZE];  // size include tcb (92) and stack




/*-----------------------------------------------------------*/
#define LCD_BUFFER_SIZE   256

static  StaticEventGroup_t  global_event_buffer; //

static  StaticStreamBuffer_t lcd_stream_struct;
static uint8_t lcd_stream_buffer[LCD_BUFFER_SIZE];


void assert_failed (unsigned char* pcFile, unsigned long ulLine);
/*-----------------------------------------------------------*/

int main( void )
{
  SystemInit ();
	GPIO_init ();
  init_irq ();
  __enable_irq ();
 
  
  g.events = xEventGroupCreateStatic (&global_event_buffer);
  
  g.lcd_stream =  xStreamBufferCreateStatic (LCD_BUFFER_SIZE, 1, lcd_stream_buffer, &lcd_stream_struct);
  
//* TaskHandle_t xTaskCreateStatic( TaskFunction_t pvTaskCode,
// *                               const char * const pcName,
// *                               uint32_t ulStackDepth,
// *                               void *pvParameters,
// *                               UBaseType_t uxPriority,
// *                               StackType_t *pxStackBuffer,
// *                               StaticTask_t *pxTaskBuffer );

  disp_handle = xTaskCreateStatic (disp_task, "DISP", DISP_STACK, NULL, DISP_TASK_PRIORITY, disp_stack, &disp_buffer);
  lcd_handle  = xTaskCreateStatic (lcd_task, "LCD", LCD_STACK, NULL, LCD_TASK_PRIORITY, lcd_stack, &lcd_buffer);
  i2c_handle  = xTaskCreateStatic (i2c_task, "I2C", I2C_STACK, NULL, I2C_TASK_PRIORITY, i2c_stack, &i2c_buffer);
  btn_handle  = xTaskCreateStatic (button_task, "BTN", BTN_STACK, NULL, BTN_TASK_PRIORITY, btn_stack, &btn_buffer);
  menu_handle = xTaskCreateStatic (menu_task, "MENU", MENU_STACK, NULL, MENU_TASK_PRIORITY, menu_stack, &menu_buffer);
  pwr_handle  = xTaskCreateStatic (power_task, "POWER", PWR_STACK, NULL, PWR_TASK_PRIORITY, pwr_stack, &pwr_buffer);
  adc_handle  = xTaskCreateStatic (ads_task, "ADC", ADC_STACK, NULL, ADC_TASK_PRIORITY, adc_stack, &adc_buffer);

  (void)disp_handle;
  (void)lcd_handle;
  (void)i2c_handle;
  (void)btn_handle;
  (void)menu_handle;
  (void)pwr_handle;
  (void)adc_handle;

	/* Start the scheduler. */
	vTaskStartScheduler ();

	return 0;
}
/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

#ifdef  DEBUG
/* Keep the linker happy. */
void assert_failed (unsigned char* pcFile, unsigned long ulLine)
{
	for( ;; )
	{
	}
}
#endif


/*******************************************************************************
*configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must provide an
*implementation of vApplicationGetIdleTaskMemory() to provide the memory that is
*used by the Idle task. 
*******************************************************************************/
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE + 64 ];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}



/*******************************************************************************
* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
* application must provide an implementation of vApplicationGetTimerTaskMemory()
* to provide the memory that is used by the Timer service task. 
*******************************************************************************/
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
/* If the buffers to be provided to the Timer task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack [configMINIMAL_STACK_SIZE + 64];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configMINIMAL_STACK_SIZE;
}
