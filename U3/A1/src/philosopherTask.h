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
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

//----- Macros -----------------------------------------------------------------
#define NUMBER_OF_PHILOSOPHERS  ( 5 )   /* number of eating philosophers      */
#define NUMBER_OF_FORKS         ( 5 )   /* Number of forks for the philosopher*/

//----- Data types -------------------------------------------------------------
/* philosopher states */
typedef enum {
    THINKING        = 0,     /* Thinking */
    WAIT_TABLE      = 1,     /* Waiting for table */
    WAIT_LEFT_FORK  = 2,     /* Waiting for left fork */
    WAIT_RIGHT_FORK = 3,     /* Waiting for right fork */
    EATING          = 4      /* Eating */
} PhilosopherStates;

//----- Function prototypes ----------------------------------------------------
extern void  vPhilosopherTask(void *pvData);

//----- Data -------------------------------------------------------------------
extern SemaphoreHandle_t semaphoreFork[NUMBER_OF_FORKS];
extern SemaphoreHandle_t semaphoreTable;

#endif /* PHILOSOPHERTASK_H_ */
