#ifndef UARTTASK_H_
#define UARTTASK_H_
/******************************************************************************/
/** \file       uartTask1.h
 *******************************************************************************
 *
 *  \brief      Writes some text to the UART. Access to the UART is granted
 *              if mutexUART is possessed by the Task. This access control
 *              can be switched out with the macro USE_UART_MUTEX (uartTask1.h)
 *
 *  \author     wht4
 *
 ******************************************************************************/
/*
 *  function    vUARTTask1
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
/* Enable or disable MUTEX to access UART */
#define USE_UART_MUTEX

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
extern void  vUARTTask(void *pvData);

//----- Data -------------------------------------------------------------------
#ifdef USE_UART_MUTEX
extern SemaphoreHandle_t mutexUART;
#endif

#endif /* UARTTASK_H_ */
