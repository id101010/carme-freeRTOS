/******************************************************************************/
/** \file       philosopherTask.c
 *******************************************************************************
 *
 *  \brief      Definition of a philosopher. We are using incarnation to
 *              initialize several philosophers.
 *
 *  \author     wht4
 *
 *  \date       24.08.2011
 *
 *  \remark     Last Modification
 *               \li wht4, 24.08.2011, Created
 *               \li wht4, 13.02.2014, Adapted to CARME-M4
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 08.03.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              vPhilosopherTask
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>

#include "philosopherTask.h"
#include "lcdFunction.h"
#include "cookTask.h"

//----- Macros -----------------------------------------------------------------
#define THINKING_TIME       ( 5000 )   /* Default timeperiod for thinking      */
#define EATING_TIME         ( 5000 )   /* default timeperiod for eating        */
#define DEADLOCK_DELAY      ( 50 )     /* Increase danger of deadlock          */

/* Compiler switches to enable or disable functionality */
#define LIMIT_TABLE_ACCESS             /* Set if access to table is limited    */
//#define USE_DEADLOCK_DELAY           /* Set if delay is enabled              */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------
/* Binary semaphore for each fork */
SemaphoreHandle_t semaphoreFork[NUMBER_OF_FORKS];
/* counting semaphore for the table */
SemaphoreHandle_t semaphoreTable;

/* States of the philosophers */
static PhilosopherStates ePhilosopherStates[NUMBER_OF_PHILOSOPHERS];

/* Portions eaten by a philosopher */
static uint32_t u32Portions[NUMBER_OF_PHILOSOPHERS];

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    vPhilosopherTask
 ******************************************************************************/
/** \brief        Definition of a philosopher. We are using incarnation to
 *                initialize several philosophers.
 *
 *  \type         global
 *
 *  \param[in]	  pvData    ID-Number of the philosopher
 *
 *  \return       void
 *
 ******************************************************************************/
void  vPhilosopherTask(void *pvData)
{

    uint8_t u8Philosopher = (uint32_t) pvData;/* Philosopher number           */
    u32Portions[u8Philosopher] = 0;           /* Reset eaten portions         */
    SpaghettiTypes eSpaghettiTypes;           /* The philosophers meal        */

    /* We don't want to start each philosopher task at the same time */
    /* therefore we wait some time depending on philosopher-number */
    vTaskDelay(10 * u8Philosopher);

    for (;;) {
        /* Thinking first, thinking time depends on Philosopher */
        ePhilosopherStates[u8Philosopher] = THINKING;
        vDisplayState(u8Philosopher, THINKING);
        vTaskDelay(THINKING_TIME + (100 * u8Philosopher));

        /* Waiting for table */
#ifdef LIMIT_TABLE_ACCESS
        ePhilosopherStates[u8Philosopher] = WAIT_TABLE;
        vDisplayState(u8Philosopher, WAIT_TABLE);
        if (xSemaphoreTake(semaphoreTable, portMAX_DELAY) == pdTRUE) {
#endif

            /* Waiting for left fork */
            ePhilosopherStates[u8Philosopher] = WAIT_LEFT_FORK;
            vDisplayState(u8Philosopher, WAIT_LEFT_FORK);
            if (xSemaphoreTake(semaphoreFork[u8Philosopher], portMAX_DELAY) == pdTRUE) {
                /* philosopher gets left fork */
                /* increase danger of deadlock if compiler switch is set */
#ifdef USE_DEADLOCK_DELAY
                vTaskDelay(DEADLOCK_DELAY);
#endif

                /* Waiting for right fork */
                ePhilosopherStates[u8Philosopher] = WAIT_RIGHT_FORK;
                vDisplayState(u8Philosopher, WAIT_RIGHT_FORK);
                if (xSemaphoreTake(semaphoreFork[(u8Philosopher+1)%NUMBER_OF_PHILOSOPHERS],
                                   portMAX_DELAY) == pdTRUE) {

                    /* check if spaghetti available */
                    ePhilosopherStates[u8Philosopher] = WAIT_SPAGHETTI;
                    vDisplayState(u8Philosopher, WAIT_SPAGHETTI);
                    if (xQueueReceive(queueSpaghetti,
                                      &(eSpaghettiTypes),
                                      portMAX_DELAY) == pdTRUE)   {

                        /* spaghetti are available, enjoy your meal */
                        vDisplayQueuedSpaghetti(uxQueueMessagesWaiting(queueSpaghetti));

                        /* Eating */
                        ePhilosopherStates[u8Philosopher] = EATING_CARBONARA + eSpaghettiTypes;
                        vDisplayState(u8Philosopher, EATING_CARBONARA + eSpaghettiTypes);
                        vTaskDelay(EATING_TIME);
                        u32Portions[u8Philosopher]++;
                        vDisplayPortions(u8Philosopher, u32Portions[u8Philosopher]);
                    }
                    xSemaphoreGive(semaphoreFork[(u8Philosopher+1)%NUMBER_OF_PHILOSOPHERS]);
                }
                xSemaphoreGive(semaphoreFork[u8Philosopher]);
            }

#ifdef LIMIT_TABLE_ACCESS
            xSemaphoreGive(semaphoreTable);
        }
#endif
    }
}
