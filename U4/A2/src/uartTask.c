/******************************************************************************/
/** \file       uartTask.c
 *******************************************************************************
 *
 *  \brief      Gatekeeper task for uart0. Write arriving Log-Msg to the uart
 *
 *  \author     wht4
 *
 *  \date       13.02.2014
 *
 *  \remark     Last Modification
 *               \li wht4, 13.02.2014, Created
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li wbr1, 01.04.2016, Comments modified
 *               \li WBR1, 09.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              UartTask
 *              logMsg
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include <uart.h>
#include <stdio.h>

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "uartTask.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------
QueueHandle_t queueUart;

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    UartTask
 ******************************************************************************/
/** \brief        Writes arriving Log msg to the Uart.
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  UartTask(void *pvData)
{

    USART_InitTypeDef USART_InitStruct;
    LogMsg            sLogMsg;

    /* Initialize UART */
    USART_StructInit(&USART_InitStruct);
    USART_InitStruct.USART_BaudRate = 115200;
    CARME_UART_Init(CARME_UART0, &USART_InitStruct);

    for (;;) {
        if(xQueueReceive(queueUart, &sLogMsg, portMAX_DELAY) == pdTRUE) {

            printf("%d:  %s  '%s'\n\r", (int) sLogMsg.xTimeTag,
                   sLogMsg.cTaskName,
                   sLogMsg.cMsg);
        }
    }
}

/*******************************************************************************
 *  function :    logMsg
 ******************************************************************************/
/** \brief        Send a log message to the uart gatekeeper task.
 *
 *  \type         global
 *
 *  \param[in]	  pcTaskName    name of the task putting the log message
 *  \param[in]	  pcMsg         log message to send
 *  \param[in]	  xWaitTime     waiting time if queue is full
 *
 *  \return       void
 *
 ******************************************************************************/
void logMsg(char * pcTaskName, char * pcMsg, portTickType xWaitTime)
{

    LogMsg sLogMsg;

    sLogMsg.xTimeTag = xTaskGetTickCount();
    sprintf(sLogMsg.cTaskName, "%s", pcTaskName);
    sprintf(sLogMsg.cMsg, "%s", pcMsg);

    /* Send msg to gatekeeper task */
    xQueueSend(queueUart, &sLogMsg, xWaitTime);
}
