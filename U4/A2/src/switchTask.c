/******************************************************************************/
/** \file       switchTask.c
 *******************************************************************************
 *
 *  \brief      Get state of switch bar and display state with the LED's
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
 *              SwitchTask
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme_io1.h>
#include <stdio.h>

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "switchTask.h"
#include "uartTask.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    SwitchTask
 ******************************************************************************/
/** \brief        Get state of switch bar and display state with the LED's
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  SwitchTask(void *pvData)
{

    uint8_t u8SwitchStatePrev = 0;
    uint8_t u8SwitchState;
    char cLogMsg[LOG_MESSAGE_SIZE];


    CARME_IO1_Init();
    CARME_IO1_LED_Set(0x00, 0xff);

    for (;;) {
        CARME_IO1_SWITCH_Get(&u8SwitchState);

        if(u8SwitchStatePrev != u8SwitchState) {

            CARME_IO1_LED_Set(u8SwitchState, 0xff);
            sprintf(cLogMsg, "Switch state chaged to: 0x%x", u8SwitchState);
            logMsg("SwitchTask", cLogMsg, portMAX_DELAY);
            u8SwitchStatePrev = u8SwitchState;
        }

        vTaskDelay(200 / portTICK_RATE_MS);
    }
}

