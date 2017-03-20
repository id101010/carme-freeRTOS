#ifndef COOKTASK_H_
#define COOKTASK_H_
/******************************************************************************/
/** \file       cookTask.h
 *******************************************************************************
 *
 *  \brief      The cook produces spaghetti. Each time a portion of spaghetti
 *              is cooked, he puts them into the message queue.
 *
 *  \author     wht4
 *
 ******************************************************************************/
/*
 *  function    vCookTask
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
#define NBR_OF_SPAGHETTI         ( 4 )        /* Number of Spaghetti types    */

#define MAX_NBR_QUEUED_SPAGHETTI ( 10 )       /* Max. number of Messages      */

//----- Data types -------------------------------------------------------------
/* Available Spaghetti types */
typedef enum {
    CARBONARA   = 0,     /* Cook is cooking spaghetti carbonara               */
    PESTO       = 1,     /* Cook is cooking spaghetti pesto                   */
    POMODORO    = 2,     /* Cook is cooking spaghetti pomodoro                */
    VESUVIO     = 3,     /* Cook is cooking spaghetti vesuvio                 */
} SpaghettiTypes;

/* State of the cook */
typedef enum {
    COOKING   = 0,       /* Cook is cooking spaghetti                         */
    NAP       = 1,       /* Cook is taking a short nap                        */
    HOLIDAY   = 2,       /* Cook is on holiday                                */
} CookState;

//----- Function prototypes ----------------------------------------------------
extern void  vCookTask(void *pvData);

//----- Data -------------------------------------------------------------------
extern QueueHandle_t queueSpaghetti;

#endif /* COOKTASK_H_ */
