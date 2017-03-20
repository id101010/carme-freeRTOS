/******************************************************************************/
/** \file       EZBSY_U4A2.c
 *******************************************************************************
 *
 *  \brief      RTOS Exercise 4.2, Send log messages to uart gatekeeper task
 *
 *  \author     wht4
 *
 *  \date       13.02.2014
 *
 *  \remark     Last Modification
 *               \li wht4, 13.02.2014, Created
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 09.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              main
 *  functions  local:
 *              vCreateTasks
 *              vCreateTimers
 *              LedCallback
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <lcd.h>                        /* GUI Library                        */
#include <carme.h>
#include <carme_io1.h>                  /* CARMEIO1 Board Support Package     */

#include <stdio.h>                      /* Standard Input/Output              */
#include <stdlib.h>                     /* General Utilities                  */
#include <string.h>                     /* String handling                    */

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "uartTask.h"
#include "switchTask.h"
#include "dummyTask.h"

//----- Macros -----------------------------------------------------------------
#define PRIORITY_UART_TASK    ( 1 )
#define PRIORITY_SWITCH_TASK  ( 4 )
#define PRIORITY_DUMMY_TASK   ( 2 )

#define STACKSIZE_UART_TASK   ( 512 )
#define STACKSIZE_SWITCH_TASK ( 256 )
#define STACKSIZE_DUMMY_TASK  ( 256 )

#define Y_HEADERLINE          ( 1 )     /* pixel y-pos for headerline */

//----- Data types -------------------------------------------------------------
static const char* pcQueueLog = "LogQueue";

//----- Function prototypes ----------------------------------------------------
static void vCreateTasks(void);
static void vCreateTimers(void);
static void ButtonCallback(xTimerHandle pxTimer);

//----- Data -------------------------------------------------------------------
/* welcome text */
static const char* pcHello = "Log Message";

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    main
 ******************************************************************************/
/** \brief        Initialize GUI, BSP and OS
 *
 *  \type         global
 *
 *  \return       error code
 *
 ******************************************************************************/
int  main(void)
{

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Initilalize display */
    LCD_Init();
    LCD_SetFont(&font_8x16B);
    LCD_DisplayStringCenterLine(Y_HEADERLINE, pcHello);

    /* Iniitialize and register Message Queue for Log-Message */
    queueUart = xQueueCreate(10, sizeof(LogMsg));
    vQueueAddToRegistry((xQueueHandle) queueUart, pcQueueLog);

    /* Create tasks, timers and start OS */
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
static void vCreateTasks(void)
{

    xTaskCreate(UartTask,
                "Uart",
                STACKSIZE_UART_TASK,
                NULL,
                PRIORITY_UART_TASK,
                NULL);
    xTaskCreate(SwitchTask,
                "SwitchTask",
                STACKSIZE_SWITCH_TASK,
                NULL,
                PRIORITY_SWITCH_TASK,
                NULL);
    xTaskCreate(DummyTask,
                "DummyTask",
                STACKSIZE_DUMMY_TASK,
                NULL,
                PRIORITY_DUMMY_TASK,
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
static void vCreateTimers(void)
{

    TimerHandle_t timerHandle;

    /* Create and start timer for led chaser light */
    timerHandle = xTimerCreate("Button",
                               250 / portTICK_RATE_MS,
                               pdTRUE,
                               NULL,
                               ButtonCallback);
    if(timerHandle != NULL) {
        xTimerStart(timerHandle, 0);
    }
}

/*******************************************************************************
 *  function :    ButtonCallback
 ******************************************************************************/
/** \brief        Get state of buttons and log a message on button press/release.
 *                Called by software timer!
 *
 *  \type         local
 *
 *  \param[in]    unused
 *
 *  \return       error code
 *
 ******************************************************************************/
static void ButtonCallback(TimerHandle_t pxTimer)
{

    static uint8_t u8PrevBtnState = 0;
    uint8_t u8BtnState;
    uint8_t u8Event;

    CARME_IO1_BUTTON_Get(&u8BtnState);

    /* Some changes occurred? */
    if((u8Event = u8PrevBtnState ^ u8BtnState) != 0x00) {

        /* Button 0 event */
        if(u8Event & 0x01) {
            if(u8PrevBtnState & 0x01) {
                logMsg("BtnCallback", "Btn0 falling edge", 0);
            } else {
                logMsg("BtnCallback", "Btn0 rising edge", 0);
            }
        }
        /* Button 1 event */
        if(u8Event & 0x02) {
            if(u8PrevBtnState & 0x02) {
                logMsg("BtnCallback", "Btn1 falling edge", 0);
            } else {
                logMsg("BtnCallback", "Btn1 rising edge", 0);
            }
        }
        /* Button 2 event */
        if(u8Event & 0x04) {
            if(u8PrevBtnState & 0x04) {
                logMsg("BtnCallback", "Btn2 falling edge", 0);
            } else {
                logMsg("BtnCallback", "Btn2 rising edge", 0);
            }
        }
        /* Button 3 event */
        if(u8Event & 0x08) {
            if(u8PrevBtnState & 0x08) {
                logMsg("BtnCallback", "Btn3 falling edge", 0);
            } else {
                logMsg("BtnCallback", "Btn3 rising edge", 0);
            }
        }
    }
    u8PrevBtnState = u8BtnState;
}

