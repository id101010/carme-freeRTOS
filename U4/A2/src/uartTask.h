#ifndef UARTTASK_H_
#define UARTTASK_H_
/******************************************************************************/
/** \file       uartTask.h
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
 *  function    UartTask
 *              logMsg
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
#define LOG_MESSAGE_SIZE ( 64 )

//----- Data types -------------------------------------------------------------
typedef struct _LogMsg {

    char         cTaskName[configMAX_TASK_NAME_LEN];
    char         cMsg[LOG_MESSAGE_SIZE];
    portTickType xTimeTag;
} LogMsg;

//----- Function prototypes ----------------------------------------------------
extern void  UartTask(void *pvData);
extern void logMsg(char * pcTaskName, char * pcMsg, portTickType xWaitTime);

//----- Data -------------------------------------------------------------------
extern xQueueHandle queueUart;

#endif /* UARTTASK_H_ */
