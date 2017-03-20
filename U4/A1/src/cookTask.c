/******************************************************************************/
/** \file       cookTask.c
 *******************************************************************************
 *
 *  \brief      The cook produces spaghetti. Each time a portion of spaghetti
 *              is cooked, he puts them into the message queue.
 *
 *  \author     wht4
 *
 *  \date       25.08.2011
 *
 *  \remark     Last Modification
 *               \li wht4, 25.08.2011, Created
 *               \li wht4, 13.02.2014, Adapted to CARME-M4
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 08.03.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              vCookTask
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>

#include "cookTask.h"
#include "lcdFunction.h"

//----- Macros -----------------------------------------------------------------
#define COOK_TIME    ( 1000 )     /* Time it takes to cook a portion spaghetti */
#define NAP_TIME     ( 1000 )     /* Time it takes for a short nap             */
#define HOLIDAY_TIME ( 15000 )    /* Time it takes for holiday                 */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------
QueueHandle_t queueSpaghetti;      /* For cooked spaghetti portions            */

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    vCookTask
 ******************************************************************************/
/** \brief        The cook produces spaghetti. Each time a portion of spaghetti
 *                is cooked, he puts them into the message queue.
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  vCookTask(void *pvData)
{

    SpaghettiTypes eSpaghettiTypes = 0;
    CookState      eCookState = COOKING;
    uint8_t        u8SpaghettiPortionsQueued = 0;

    for (;;) {
        eCookState = COOKING;
        /* Decide which spaghetti type */
        eSpaghettiTypes++;
        if (eSpaghettiTypes >= NBR_OF_SPAGHETTI) {
            eSpaghettiTypes = 0;
        }
        /* Cook spaghetti */
        vDisplayCookState(eCookState, eSpaghettiTypes);
        vTaskDelay(COOK_TIME);

        /* Put the spaghetti portion in the queue */
        if (xQueueSend(queueSpaghetti,
                       (void *) &eSpaghettiTypes,
                       portMAX_DELAY) == pdTRUE)   {

            /* Display available spaghetti portions */
            u8SpaghettiPortionsQueued = uxQueueMessagesWaiting(queueSpaghetti);
            vDisplayQueuedSpaghetti(u8SpaghettiPortionsQueued);
            /* Time for a break? */
            if (u8SpaghettiPortionsQueued <= 4) {
                /* Not enough spaghetti queued. We have just time for a nap */
                eCookState = NAP;
                vDisplayCookState(eCookState, eSpaghettiTypes);
                vTaskDelay(NAP_TIME);
            } else {
                /* Enough spaghetti queued. We have time for holiday */
                eCookState = HOLIDAY;
                vDisplayCookState(eCookState, eSpaghettiTypes);
                vTaskDelay(HOLIDAY_TIME);
            }
        }
    }
}

