/******************************************************************************/
/** \file       lcdFunction.c
 *******************************************************************************
 *
 *  \brief      Functions to display the data for the philosophers task
 *
 *  \author     wht4
 *
 *  \date       24.08.2011
 *
 *  \remark     Last Modification
 *               \li wht4, 24.08.2011, Created
 *               \li wht4, 13.02.2014, Adapted to CARME-M4
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 09.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              vInitDisplay
 *              vDisplayStaticText
 *              vDisplayState
 *              vDisplayPortions
 *              vDisplayCookState
 *              vDisplayQueuedSpaghetti
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include <lcd.h>                        /* GUI Library                        */
#include <ff.h>
#include <rtc.h>

#include <stdio.h>                      /* Standard Input/Output              */

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "lcdFunction.h"

//----- Macros -----------------------------------------------------------------
#define X_HEADER          ( 50 )    /* Pixel x-pos for the headerline         */
#define	X_START_LINE      ( 10 )    /* Pixel x-pos for start of division line */
#define	X_END_LINE        ( 300 )   /* Pixel x-pos for end of division line   */
#define	X_PHILOSOPHER	  ( 10 )    /* Pixel x-pos Philosopher column         */
#define	X_STATE           ( 85 )    /* Pixel x-pos Philosopher state column   */
#define	X_PORTIONS        ( 237 )   /* Pixel x-pos Philosopher portions column*/

#define Y_HEADER          ( 1 )      /* Pixel y-pos for headerline             */
#define Y_TITLE           ( 25 )     /* Pixel y-pos for titles                 */
#define Y_PHILOSOPHER     ( 45 )     /* Pixel y-pos for first philosopher      */
#define Y_INCREMENT       ( 10 )     /* Pixel between philosopher              */
#define Y_COOK            ( 145 )    /* Pixel y-pos for spaghetti cook         */
#define Y_SPAGHETTI       ( 180 )    /* Pixel y-pos for spaghetti in queue     */

#define CIRCLE_DIA        ( 20 )
#define CIRCLE_RADIUS     ( CIRCLE_DIA >> 1 )
#define CIRCLE_GAP        ( 10 )

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------
FATFS fsMain;

static SemaphoreHandle_t mutexLCD;

/* Philosopher State Text */
static const char* pcPhilosopherState[] = {
    "thinking          ",  /* Thinking text      */
    "Wait table access ",  /* Table text         */
    "Wait left fork    ",  /* Left fork text     */
    "Wait right fork   ",  /* Right fork text    */
    "wait for spaghetti", /* Wait for spaghetti */
    "eating carbonara  ",   /* Eating carbonara   */
    "eating pesto      ",   /* Eating pesto       */
    "eating pomodoro   ",   /* Eating pomodoro    */
    "eating vesuvio    "    /* eating vesuvio     */
};

/* Cook State Text */
static const char* pcCookState[] = {
    NULL,                            /* Cook is cooking spaghetti  */
    "Taking a short nap         ",  /* Cook is taking a short nap */
    "Cook is on holiday         "   /* Cook is on holiday         */
};

/* Type of spaghetti the cook is cooking */
static const char* pcSpaghettiType[] =   {
    "Cooking spaghetti carbonara",
    "Cooking spaghetti pesto    ",
    "Cooking spaghetti pomodoro ",
    "Cooking spaghetti vesuvio  "
};

static const char* pcHello	     = "The Dining Philosophers"; /* Welcome text      */
static const char* pcPhilosopher = "Who";                     /* Philosopher text  */
static const char* pcState       = "Action";                  /* Philosopher state */
static const char* pcPortions    = "Portions";                /* Portions eaten    */
static const char* pcCook        = "Cook";                    /* Cook text  */
static const char* pcQueueSpaghetti1 = "Spaghetti";           /* Queued spaghetti  */
static const char* pcQueueSpaghetti2 = "in queue";

/* Previous spaghetti portions displayed. This variable is needed by the      */
/* function vDisplayQueuedSpaghetti                                           */
static  uint8_t  u8PrevSpaghettiQueued = 0;

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    vInitDisplay
 ******************************************************************************/
/** \brief        Initialize the display and the semaphore to access the
 *                display.
 *
 *  \type         global
 *
 *  \return       void
 *
 ******************************************************************************/
void vInitDisplay(void)
{

    LCD_Init();
    mutexLCD = xSemaphoreCreateMutex();
}

/*******************************************************************************
 *  function :    vDisplayStaticText
 ******************************************************************************/
/** \brief        Displays static text on LCD. Uses mutexLCD to access
 *                the display. vInitDisplay has to be called first.
 *
 *  \type         global
 *
 *  \return       void
 *
 ******************************************************************************/
void vDisplayStaticText(void)
{

    char    cBuffer[20];
    uint8_t i;

    /* Display welcome */
    LCD_SetTextColor(GUI_COLOR_BLUE);
    LCD_SetFont(&font_9x15B);
    LCD_DisplayStringXY(X_HEADER, Y_HEADER, pcHello);

    /* Display title */
    LCD_SetTextColor(GUI_COLOR_WHITE);
    LCD_SetFont(&font_8x16);
    LCD_DisplayStringXY(X_PHILOSOPHER, Y_TITLE, pcPhilosopher);
    LCD_DisplayStringXY(X_STATE, Y_TITLE, pcState);
    LCD_DisplayStringXY(X_PORTIONS, Y_TITLE, pcPortions);
    LCD_DrawLine(X_START_LINE,
                 Y_TITLE + Y_INCREMENT + 5,
                 X_END_LINE,
                 Y_TITLE + Y_INCREMENT + 5,
                 GUI_COLOR_GREEN);

    /* Display philosopher */
    for (i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        sprintf(cBuffer, "P%d:", (int)i + 1);
        LCD_DisplayStringXY(X_START_LINE,
                            Y_PHILOSOPHER + (i * Y_INCREMENT * 2),
                            cBuffer);
    }

    LCD_DisplayStringXY(X_START_LINE, Y_COOK + 10, pcCook);
    LCD_DisplayStringXY(X_START_LINE, Y_SPAGHETTI, pcQueueSpaghetti1);
    LCD_DisplayStringXY(X_START_LINE, Y_SPAGHETTI + 20, pcQueueSpaghetti2);
}

/*******************************************************************************
 *  function :    vDisplayState
 ******************************************************************************/
/** \brief        Displays the state of a philosopher on the LCD. Uses
 *                mutexLCD to access the display. vInitDisplay has to
 *                be called first.
 *
 *  \type         global
 *
 *  \param[in]    u8Philosopher        Philosopher to display state
 *  \param[in]    ePhilosopherStates   State of the philosopher
 *
 *  \return       void
 *
 ******************************************************************************/
void  vDisplayState(uint8_t u8Philosopher, PhilosopherStates ePhilosopherStates)
{

    if (xSemaphoreTake(mutexLCD, portMAX_DELAY) == pdTRUE) {
        LCD_DisplayStringXY(X_STATE,
                            Y_PHILOSOPHER + (u8Philosopher * Y_INCREMENT * 2),
                            pcPhilosopherState[ePhilosopherStates]);

        xSemaphoreGive(mutexLCD);
    }
}

/*******************************************************************************
 *  function :    vDisplayPortions
 ******************************************************************************/
/** \brief        Displays portions eaten by a philosopher. Uses
 *                mutexLCD to access the display. vInitDisplay has to
 *                be called first.
 *
 *  \type         global
 *
 *  \param[in]    u8Philosopher        Philosopher to display portions
 *  \param[in]    u32Portions          Portions eaten by the philosopher
 *
 *  \return       void
 *
 ******************************************************************************/
void  vDisplayPortions(uint8_t u8Philosopher, uint32_t u32Portions)
{

    char   cBuffer[8];

    if (xSemaphoreTake(mutexLCD, portMAX_DELAY) == pdTRUE) {
        sprintf(cBuffer, "%d", (int) u32Portions);
        LCD_DisplayStringXY(X_PORTIONS,
                            Y_PHILOSOPHER + (u8Philosopher * Y_INCREMENT * 2),
                            cBuffer);

        xSemaphoreGive(mutexLCD);  /* Release semaphore */
    }
}

/*******************************************************************************
 *  function :    vDisplayCookState
 ******************************************************************************/
/** \brief        Displays the state of the cook on the LCD. Uses
 *                mutexLCD to access the display. vInitDisplay has to
 *                be called first.
 *
 *  \type         global
 *
 *  \param[in]    eCookState        State of the cook
 *  \param[in]    eSpaghettiTypes   What spaghetti is cooked by the chef
 *
 *  \return       void
 *
 ******************************************************************************/
void  vDisplayCookState(CookState eCookState, SpaghettiTypes eSpaghettiTypes)
{

    if (xSemaphoreTake(mutexLCD, portMAX_DELAY) == pdTRUE) {

        switch (eCookState)   {
        /* If the chef is cooking, we display the type of spaghetti */
        case COOKING:
            LCD_DisplayStringXY(X_STATE,
                                Y_COOK + 10,
                                pcSpaghettiType[eSpaghettiTypes]);
            break;
        /* There are enough spaghetti in the queue. So take a nap */
        case NAP:
            LCD_DisplayStringXY(X_STATE,
                                Y_COOK + 10,
                                pcCookState[NAP]);
            break;
        /* The spaghetti queue is almost full. The cook can go on vacation */
        case HOLIDAY:
            LCD_DisplayStringXY(X_STATE,
                                Y_COOK + 10,
                                pcCookState[HOLIDAY]);
            break;

        default:
            break;
        }
        xSemaphoreGive(mutexLCD);
    }
}

/*******************************************************************************
 *  function :    vDisplayCookState
 ******************************************************************************/
/** \brief        Displays the number of spaghetti portions in the message
 *                queue. This function uses the static variable
 *                u8PrevSpaghettiQueuedUses mutexLCD to access the
 *                display. vInitDisplay has to be called first.
 *
 *  \type         global
 *
 *  \param[in]    u8SpaghettiQueued   Spaghetti portions in the queue
 *
 *  \return       void
 *
 ******************************************************************************/
void  vDisplayQueuedSpaghetti(uint8_t u8SpaghettiQueued)
{

    uint8_t i;

    if (xSemaphoreTake(mutexLCD, portMAX_DELAY) == pdTRUE) {
        /* Just draw spaghetti pots which are not already displayed */
        if (u8PrevSpaghettiQueued < u8SpaghettiQueued)   {

            for (i = (u8PrevSpaghettiQueued); i < u8SpaghettiQueued; i++)   {
                /* Draw first five pots on the first line */
                if (i < 5)   {
                    LCD_DrawCircle(X_STATE + (i * CIRCLE_DIA) + (i * CIRCLE_GAP) + CIRCLE_RADIUS,
                                   Y_SPAGHETTI + CIRCLE_RADIUS,
                                   CIRCLE_RADIUS,
                                   GUI_COLOR_RED);
                }
                /* Draw the other five pots on the second line */
                else   {
                    LCD_DrawCircle(X_STATE + ((i - 5) * CIRCLE_DIA) + ((i- 5) * CIRCLE_GAP) + CIRCLE_RADIUS,
                                   Y_SPAGHETTI + CIRCLE_DIA + CIRCLE_RADIUS + CIRCLE_GAP,
                                   CIRCLE_RADIUS,
                                   GUI_COLOR_RED);
                }
            }
        }
        /* If philosopher have eaten more than the cook could prepare, */
        /* then we have to delete this portions. Just draw black rectangle */
        if (u8PrevSpaghettiQueued > u8SpaghettiQueued) {

            for (i = (u8SpaghettiQueued); i < u8PrevSpaghettiQueued; i++)   {
                if (i < 5)   {
                    LCD_DrawRectF(X_STATE + (i * CIRCLE_DIA) + (i * CIRCLE_GAP),
                                  Y_SPAGHETTI,
                                  CIRCLE_DIA,
                                  CIRCLE_DIA,
                                  GUI_COLOR_BLACK);
                } else   {

                    LCD_DrawRectF(X_STATE + ((i - 5) * CIRCLE_DIA) + ((i- 5) * CIRCLE_GAP),
                                  Y_SPAGHETTI + CIRCLE_DIA + CIRCLE_GAP,
                                  CIRCLE_DIA,
                                  CIRCLE_DIA,
                                  GUI_COLOR_BLACK);
                }
            }
        }
        u8PrevSpaghettiQueued = u8SpaghettiQueued;

        xSemaphoreGive(mutexLCD);
    }
}
