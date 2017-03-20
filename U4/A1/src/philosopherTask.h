#ifndef PHILOSOPHERTASK_H_
#define PHILOSOPHERTASK_H_
/******************************************************************************/
/** \file       philosopherTask.h
 *******************************************************************************
 *
 *  \brief      Definition of a philosopher. We are using incarnation to
 *              initialize several philosophers.
 *
 *  \author     wht4
 *
 ******************************************************************************/
/*
 *  function    vPhilosopherTask
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

//----- Macros -----------------------------------------------------------------
#define NUMBER_OF_PHILOSOPHERS  ( 5 )   /* number of eating philosophers      */
#define NUMBER_OF_FORKS         ( 5 )   /* Number of forks for the philosopher*/

//----- Data types -------------------------------------------------------------
/* philosopher states */
typedef enum {
    THINKING         = 0,     /* Thinking */
    WAIT_TABLE       = 1,     /* Waiting for table */
    WAIT_LEFT_FORK   = 2,     /* Waiting for left fork */
    WAIT_RIGHT_FORK  = 3,     /* Waiting for right fork */
    WAIT_SPAGHETTI   = 4,     /* Wait for spaghetti */
    EATING_CARBONARA = 5,     /* Eating carbonara */
    EATING_PESTO     = 6,     /* Eating pesto */
    EATING_POMODORO  = 7,     /* Eating pomodoro */
    EATING_VESUVIO   = 8      /* Eating vesuvio */
} PhilosopherStates;

//----- Function prototypes ----------------------------------------------------
extern void  vPhilosopherTask(void *pvData);

//----- Data -------------------------------------------------------------------
extern xSemaphoreHandle semaphoreFork[NUMBER_OF_FORKS];
extern xSemaphoreHandle semaphoreTable;

#endif /* PHILOSOPHERTASK_H_ */
