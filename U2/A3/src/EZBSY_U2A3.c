/******************************************************************************/
/** \file       EZBSY_U2A3.c
 *******************************************************************************
 *
 *  \brief      RTOS Exercise 2.3, Display Kit-State on LCD
 *
 *  \author     Roger Weber, Erwin Stucki, wht4
 *
 *  \date       24.08.2011
 *
 *  \remark     Last Modification
 *               \li WBR1, 30.10.2005, New BSP Versions used
 *               \li WBR1, 18.09.2006, Comments in doxygen format
 *               \li SCE2, 14.06.2007, New Carme Hardware
 *               \li wht4, 23.08.2011, Adapted to freeRTOS kernel
 *               \li wht4, 24.01.2014, Adapted to CARME-M4
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 21.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              main
 *  functions  local:
 *              vCreateTasks
 *              vCreateTimers
 *              SwitchCallback
 *              ButtonCallback
 *              LedCallback
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include <carme_io1.h>               /* CARMEIO1 Board Support Package     */
#include <carme_io2.h>               /* CARMEIO1 Board Support Package     */

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "lcdTask.h"

//----- Macros -----------------------------------------------------------------
#define PRIORITY_LCDTASK       ( 3 )      /* Priority of LCD Task             */

#define STACKSIZE_LCDTASK      ( 512 )    /* Stacksize of LCD Task            */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void vCreateTasks(void);
static void vCreateTimers(void);
static void SwitchCallback(xTimerHandle pxTimer);
static void ButtonCallback(xTimerHandle pxTimer);
static void LedCallback(xTimerHandle pxTimer);

//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    main
 ******************************************************************************/
/** \brief        Initialize BSP and OS
 *
 *  \type         global
 *
 *  \return       error code
 *
 ******************************************************************************/
int  main(void) {

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    CARME_IO1_Init();
    CARME_IO2_Init();
    CARME_IO1_LED_Set(0x00, 0xff);

    vCreateTasks();
    vCreateTimers();
    vTaskStartScheduler();

    /* code never reached */
	for (;;) {
	}
	return 0;
}

/*******************************************************************************
 *  function :    vCreateTasks
 ******************************************************************************/
/** \brief        Create all application task
 *
 *  \type         local
 *
 *  \return       void
 *
 ******************************************************************************/
static void vCreateTasks(void)  {
    /* Create LCD Task */
    xTaskCreate(vLCDTask,
                "LCD Task",
                STACKSIZE_LCDTASK,
                NULL,
                PRIORITY_LCDTASK,
                NULL);
}

/*******************************************************************************
 *  function :    vCreateTimers
 ******************************************************************************/
/** \brief        Create all application software timer
 *
 *  \type         local
 *
 *  \return       void
 *
 ******************************************************************************/
static void vCreateTimers(void)  {

    TimerHandle_t timerHandle;

    /* Create and start timer for switch state */
    timerHandle = xTimerCreate("Switch Timer",
                               100 / portTICK_RATE_MS,
                               pdTRUE,
                               NULL,
                               SwitchCallback);
    if(timerHandle != NULL) {
        xTimerStart(timerHandle, 0);
    }

    /* Create and start timer for button state */
    timerHandle = xTimerCreate("Button Timer",
                               50 / portTICK_RATE_MS,
                               pdTRUE,
                               NULL,
                               ButtonCallback);
    if(timerHandle != NULL) {
        xTimerStart(timerHandle, 0);
    }

    /* Create and start timer for led chaser light */
    timerHandle = xTimerCreate("LED Timer",
                               400 / portTICK_RATE_MS,
                               pdTRUE,
                               NULL,
                               LedCallback);
    if(timerHandle != NULL) {
        xTimerStart(timerHandle, 0);
    }
}


/*******************************************************************************
 *  function :    SwitchCallback
 ******************************************************************************/
/** \brief        Reads periodically switch state and stores them in global
 *                variable u8SwitchState. Called by software timer!
 *
 *  \type         local
 *
 *  \param[in]    unused
 *
 *  \return       error code
 *
 ******************************************************************************/
static void SwitchCallback(xTimerHandle pxTimer) {
    uint8_t switchState;

    CARME_IO1_SWITCH_Get(&switchState);
    /* copy switchState global variable u8SwitchState, this is an access to a
	 * critical section, so disable interrupts while reading the variable
	 */
	taskENTER_CRITICAL();	/* disable interrupts */
	u8SwitchState = switchState; /* access critical section */
	taskEXIT_CRITICAL();	/* enable interrupts again */
}


/*******************************************************************************
 *  function :    ButtonCallback
 ******************************************************************************/
/** \brief        Reads button state and stores them in global variable
 *                u8ButtonState. Called by software timer!
 *
 *  \type         local
 *
 *  \param[in]    unused
 *
 *  \return       error code
 *
 ******************************************************************************/
static void ButtonCallback(xTimerHandle pxTimer) {
    uint8_t buttonState;

    CARME_IO1_BUTTON_Get(&buttonState);
    /* copy buttonState to global variable u8ButtonState, this is an access to a
	 * critical section, so disable interrupts while writing to the variable
	 */
	taskENTER_CRITICAL();	/* disable interrupts */
	u8ButtonState = buttonState; /* access critical section */
	taskEXIT_CRITICAL();	/* enable interrupts again */
}


/*******************************************************************************
 *  function :    LedCallback
 ******************************************************************************/
/** \brief        Implements a LED chaser. Called by software timer!
 *
 *  \type         local
 *
 *  \param[in]    unused
 *
 *  \return       error code
 *
 ******************************************************************************/
static void LedCallback(xTimerHandle pxTimer) {

    static uint8_t u8Led = 0x01;

    if (u8Led == 0x80)   {
        u8Led = 0x01;
    }
    u8Led <<= 1;
    CARME_IO1_LED_Set(u8Led, 0xff);
}


