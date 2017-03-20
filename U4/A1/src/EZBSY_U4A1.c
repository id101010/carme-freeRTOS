/******************************************************************************/
/** \file       EZBSY_U4A1.c
 *******************************************************************************
 *
 *  \brief      RTOS Exercise 4-1, The Dining Philosophers and the Sphaghetti Cook
 *              The five philosophers "thinking" and "eating" are using
 *              semaphores to access forks and the table.
 *              A cook is using a message queue to produce sphaghetti,
 *              each message is a portion of spaghetti
 *
 *  \author     Roger Weber, Erwin Stucki, wht4
 *
 *  \date       30.01.2007
 *
 *  \remark     Last Modification
 *               \li WBR1, 21.11.2005, LCD title added, delay times modified
 *               \li WBR1, 18.09.2006, Comments in doxygen format
 *               \li WBR1, 30.01.2007, Running on CARME
 *               \li SCE2, 03.03.2008, MuWatch added
 *               \li wht4, 23.08.2011, Adapted to freeRTOS kernel
 *               \li wht4, 22.02.2014, Adapted to CARME-M4
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
#include <stdio.h>                      /* Standard Input/Output              */

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "lcdFunction.h"
#include "philosopherTask.h"
#include "cookTask.h"

//----- Macros -----------------------------------------------------------------
#define PRIORITY_PHILOSOPHER    ( 2 )       /* All Philosopher have same prio */
#define PRIORITY_COOK           ( 3 )       /* Priority of the cook           */

#define STACKSIZE_PHILOSOPHER   ( 512 )     /* Stacksize in Number of bytes   */
#define STACKSIZE_COOK          ( 512 )     /* Stacksize of the cook          */

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
static const char* pcSemaphoreTableName = "TableSemaphore";
static const char* pcQueueSpaghetti = "SpaghettiQueue";
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
int  main(void)
{

    uint8_t i;

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    /* Initialize the LCD and display the static text  */
    vInitDisplay();
    vDisplayStaticText();

    /* Create Semaphores for each fork */
    for (i = 0; i < NUMBER_OF_FORKS; i++) {
        semaphoreFork[i] = xSemaphoreCreateBinary();
        xSemaphoreGive(semaphoreFork[i]); /* make available */
        vQueueAddToRegistry((xQueueHandle) semaphoreFork[i],
                            pcSemaphoreForkName[i]);

    }

    /* Counting semaphore for table, protect table access to avoid deadlock */
    semaphoreTable = xSemaphoreCreateCounting(NUMBER_OF_PHILOSOPHERS - 2,
                     NUMBER_OF_PHILOSOPHERS - 2);
    vQueueAddToRegistry((xQueueHandle) semaphoreTable, pcSemaphoreTableName);

    /* Message Queue for cooked spaghetti portions */
    queueSpaghetti = xQueueCreate(MAX_NBR_QUEUED_SPAGHETTI,
                                  sizeof(SpaghettiTypes));
    vQueueAddToRegistry((xQueueHandle) queueSpaghetti, pcQueueSpaghetti);

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
static void vCreateTasks(void)
{

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
    /* Create cook task */
    xTaskCreate(vCookTask,
                "Cook Task",
                STACKSIZE_COOK,
                NULL,
                PRIORITY_COOK,
                NULL);
}

