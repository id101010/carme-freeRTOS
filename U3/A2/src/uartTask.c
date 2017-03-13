/******************************************************************************/
/** \file       uartTask.c
 *******************************************************************************
 *
 *  \brief      Writes some text to the UART. Access to the UART is granted
 *              if mutexUART is possessed by the Task. This access control
 *              can be switched out with the macro USE_UART_MUTEX (uartTask1.h)
 *
 *  \author     wht4
 *
 *  \date       25.08.2011
 *
 *  \remark     Last Modification
 *               \li wht4, 25.08.2011, Created
 *               \li wht4, 11.02.2014, Adapted to CARME-M4
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 01.03.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              vUARTTask
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include <uart.h>

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
/* Mutex to control access to UART */
#ifdef USE_UART_MUTEX
SemaphoreHandle_t mutexUART;
#endif

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    vUARTTask1
 ******************************************************************************/
/** \brief        Writes some text to the UART. Access to the UART is granted
 *                if mutexUART is possessed by the Task. This access control
 *                can be switched out with the macro USE_UART_MUTEX (uartTask1.h)
 *
 *  \type         global
 *
 *  \param[in]    pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  vUARTTask(void *pvData) {

    char* pcText = (char *) pvData;

    for (;;) {
        /* Get mutex to access UART if compiler switch is set */
#ifdef USE_UART_MUTEX
        if (xSemaphoreTake(mutexUART, portMAX_DELAY) == pdTRUE) {
#endif

            CARME_UART_SendString(CARME_UART0, pcText);

#ifdef USE_UART_MUTEX
            xSemaphoreGive(mutexUART);
        }
#endif

        vTaskDelay(2);  /* Wait 2 tick */
    }
}
