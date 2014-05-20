#include "stm32f4xx.h"

#include "stm32f4_discovery.h"
#include <math.h>

#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/semphr.h"

uint64_t u64IdleTicksCnt=0; // Counts when the OS has no task to execute.
uint64_t tickTime=0;        // Counts OS ticks (default = 1000Hz).

void GPIO_A0_Init(void);

void vButtonTask (void * pvparameters);
void vToggleLED5Task (void *pvParameters);
void vToggleLED6Task (void *pvParameters);

/*
 * When FreeRTOS crashes, you often end up in a hard fault.
 */
void HardFault_Handler (void){
	STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED6);
}

/*-----------------------------------------------------------*/
static void prvSetupHardware( void )
{
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	SystemInit();
#if 0
	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Setup the LED outputs. */
	vParTestInitialise();

	/* Configure the button input.  This configures the interrupt to use the
	lowest interrupt priority, so it is ok to use the ISR safe FreeRTOS API
	from the button interrupt handler. */
	STM_EVAL_PBInit( BUTTON_USER, BUTTON_MODE_EXTI );
#endif
	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
    STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
}

int main(void)
{
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	prvSetupHardware();

	xTaskCreate( vButtonTask, ( signed char * ) "Button Task", 100, NULL, 1, NULL );
	xTaskCreate( vToggleLED5Task, (signed portCHAR *)"Toggle LED5 Task", 192, NULL, 1, NULL);
	xTaskCreate( vToggleLED6Task, (signed portCHAR *)"Toggle LED6 Task", 192, NULL, 1, NULL);

    vTaskStartScheduler(); // This should never return.


    while(1)
    {

    }

    return 1;
}

/*
 * A task that read reads and debounces the user's button
 */
void vButtonTask( void *pvparameters )
{

	uint8_t wasPressed = 0;

	for (;;){

		while (STM_EVAL_PBGetState(BUTTON_USER) == 1) {
		    wasPressed = 1;
		}

		if (wasPressed){
			STM_EVAL_LEDToggle(LED4);
    	    wasPressed = 0;
		}

		vTaskDelay(30 / portTICK_RATE_MS);
	}
}

void vToggleLED5Task (void *pvParameters)
{
	int8_t *pcTaskName;
	pcTaskName = (int8_t *) pvParameters;
	while(1)
	{
		STM_EVAL_LEDToggle(LED5);
		vTaskDelay(500 / portTICK_RATE_MS);
	}

}

void vToggleLED6Task (void *pvParameters)
{
	int8_t *pcTaskName;
	pcTaskName = (int8_t *) pvParameters;
	while(1)
	{
		STM_EVAL_LEDToggle(LED6);
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

// This FreeRTOS callback function gets called once per tick (default = 1000Hz).
// ----------------------------------------------------------------------------
void vApplicationTickHook( void ) {
    ++tickTime;
}

// This FreeRTOS call-back function gets when no other task is ready to execute.
// On a completely unloaded system this is getting called at over 2.5MHz!
// ----------------------------------------------------------------------------
void vApplicationIdleHook( void ) {
    ++u64IdleTicksCnt;
}

// A required FreeRTOS function.
// ----------------------------------------------------------------------------
void vApplicationMallocFailedHook( void ) {
    configASSERT( 0 );  // Latch on any failure / error.
}


/*
 * Required callbacks for audio playback
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
	return 1;
}

void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
}

void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size)
{
}

void EVAL_AUDIO_Error_CallBack(void* pData)
{
	while(1);
}

uint32_t Codec_TIMEOUT_UserCallback(void)
{
	while(1);
	return 1;
}
