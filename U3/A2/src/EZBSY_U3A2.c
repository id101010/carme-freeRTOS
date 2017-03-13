/******************************************************************************/
/** \file       main.c
 *******************************************************************************
 *
 *  \brief      RTOS Exercise 3-2, Access UART from different tasks, using MUTEX
 *
 *  \author     Roger Weber, Erwin Stucki, wht4
 *
 *  \date       20.09.2006
 *
 *  \remark     Last Modification
 *               \li SCE2, 15.06.2007, New Carme Hardware
 *               \li WBR1, 03.03.2008, MUTEX instead of semaphore
 *               \li wht4, 25.08.2011, Adapted to freeRTOS kernel
 *               \li wht4, 16.01.2014, Adapted to CARME-M4
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
#include <uart.h>                       /* CARME BSP UART port                */

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

//----- Macros -----------------------------------------------------------------
#define PRIORITY_TASK1      ( 2 )      /* Priority of Task1                   */
#define PRIORITY_TASK2      ( 2 )      /* Priority of Task2                   */

#define STACKSIZE_TASK1     ( 256 )    /* Stacksize of Task1                  */
#define STACKSIZE_TASK2     ( 256 )    /* Stacksize of Task2                  */

#define Y_HEADERLINE        ( 1 )     /* pixel y-pos for headerline          */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void vCreateTasks(void);

//----- Data -------------------------------------------------------------------
/* strings to write to UART */
static const char* pcUARTTask1Text = {"TASK #1 22222222222222222222222222222222222222222222\r\n"};
static const char* pcUARTTask2Text = {"TASK #2 THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG!\r\n"};

/* welcome text */
static const char* pcHello = "UART Mutex"; ///< Welcome text

/* Semaphore and MUTEX names */
#ifdef USE_UART_MUTEX
static const char* pcMutexName = "UART Mutex"; ///< Mutex to access UART
#endif

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
int  main(void) {

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Initialize CARME IO1 for LEDs */
    CARME_IO1_Init();
    CARME_IO1_LED_Set(0x00, 0xff);

    /* Initialize Display */
    LCD_Init();
    LCD_SetFont(&font_8x16B);
    LCD_DisplayStringCenterLine(Y_HEADERLINE, pcHello);

    /* Initialize UART */
    USART_InitTypeDef USART_InitStruct;
    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = 115200;
    CARME_UART_Init(CARME_UART0, &USART_InitStruct);

    /* Create mutex to control access to UART */
#ifdef USE_UART_MUTEX
    mutexUART = xSemaphoreCreateMutex();
    vQueueAddToRegistry((xQueueHandle) mutexUART, pcMutexName);
#endif

    vCreateTasks();
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
 *                The text for each UART Task is passed by the 4th parameter
 *
 *  \type         local
 *
 *  \return       void
 *
 ******************************************************************************/
static void vCreateTasks(void)  {
    /* Create UART Task1 */
    xTaskCreate(vUARTTask,
                "UART Task1",
                STACKSIZE_TASK1,
                (void *) pcUARTTask1Text,
                PRIORITY_TASK1,
                NULL);
    /* Create UART Task2 */
    xTaskCreate(vUARTTask,
                "UART Task2",
                STACKSIZE_TASK2,
                (void *) pcUARTTask2Text,
                PRIORITY_TASK2,
                NULL);
}

