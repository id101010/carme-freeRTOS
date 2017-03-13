/******************************************************************************/
/** \file       EZBSY_U3A1.c
 *******************************************************************************
 *
 *  \brief      RTOS Exercise 3-1, The Dining Philosophers
 *              The five philosophers "thinking" and "eating" are using
 *              semaphores to access forks and the table.
 *
 *  \author     Roger Weber, Erwin Stucki, wht4
 *
 *  \date       22.08.2011
 *
 *  \remark     Last Modification
 *               \li SCE2, 15.06.2007, New Carme Hardware
 *               \li wht4, 23.08.2011, Adapted to freeRTOS kernel
 *               \li wht4, 11.02.2014, Adapted for CARME-M4
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 08.03.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              main
 *  functions  local:
 *              vCreateTasks
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include <stdio.h>                      /* Standard Input/Output              */

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "lcdFunction.h"
#include "philosopherTask.h"

//----- Macros -----------------------------------------------------------------
#define PRIORITY_PHILOSOPHER    ( 2 )       /* All Philosopher have same prio */

#define STACKSIZE_PHILOSOPHER   ( 256 )     /* Stacksize philosopher task     */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void vCreateTasks(void);

//----- Data -------------------------------------------------------------------
static const char* pcSemaphoreForkName[] = {
    "Fork0",
    "Fork1",
    "Fork2",
    "Fork3",
    "Fork4"
};
static const char* pcTableAccess = "TableAccess";

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    main
 ******************************************************************************/
/** \brief        Initialize LCD and OS
 *
 *  \type         global
 *
 *  \return       error code
 *
 ******************************************************************************/
int  main(void) {

    uint8_t i;
    char    cBuffer[12];

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Initialize the LCD and display the static text  */
    vInitDisplay();
    vDisplayStaticText();

    /* Create Semaphores for each fork */
    for (i = 0; i < NUMBER_OF_FORKS; i++) {
        semaphoreFork[i] = xSemaphoreCreateBinary();
        xSemaphoreGive(semaphoreFork[i]); /* make available */
        sprintf(cBuffer, "Fork%d", (int) i);
        vQueueAddToRegistry((xQueueHandle) semaphoreFork[i],
                            pcSemaphoreForkName[i]);
    }
    /* Counting semaphore for table, protect table access to avoid deadlock */
    semaphoreTable = xSemaphoreCreateCounting(NUMBER_OF_PHILOSOPHERS - 2,
                     NUMBER_OF_PHILOSOPHERS - 2);
    vQueueAddToRegistry((xQueueHandle) semaphoreTable,
                        pcTableAccess);

    /* Create all application tasks and launch the scheduler */
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
 *
 *  \type         local
 *
 *  \return       error code
 *
 ******************************************************************************/
static void vCreateTasks(void)  {

    uint8_t i;
    char    cBuffer[32];

    /* Create all philosopher tasks */
    for (i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        sprintf(cBuffer, "Philosopher %d", (int) i);  /* Prepare the taskname */

        xTaskCreate(vPhilosopherTask,
                    cBuffer,
                    STACKSIZE_PHILOSOPHER,
                    (void *) (uint32_t) i,
                    PRIORITY_PHILOSOPHER,
                    NULL);
    }
}

