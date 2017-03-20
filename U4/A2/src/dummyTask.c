/******************************************************************************/
/** \file       dummyTask.c
 *******************************************************************************
 *
 *  \brief      Just a dummy task
 *
 *  \author     wht4
 *
 *  \date       13.02.2014
 *
 *  \remark     Last Modification
 *               \li wht4, 13.02.2014, Created
 *               \li WBR1, 09.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              DummyTask
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <stdio.h>

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "dummyTask.h"
#include "uartTask.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    DummyTask
 ******************************************************************************/
/** \brief        Just a dummy task
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  DummyTask(void *pvData)
{

    for (;;) {
        logMsg("DummyTask", "keep running ...", portMAX_DELAY);
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

