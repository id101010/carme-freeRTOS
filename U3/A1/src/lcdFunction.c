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
 *               \li wht4, 11.02.2014, Adapted for CARME-M4
 *               \li WBR1, 09.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              vInitDisplay
 *              vDisplayStaticText
 *              vDisplayState
 *              vDisplayPortions
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <lcd.h>                        /* GUI Library                        */

#include <stdio.h>                      /* Standard Input/Output              */

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

#include "lcdFunction.h"

//----- Macros -----------------------------------------------------------------
#define X_HEADER         ( 50 )     /* Pixel x-pos for the headerline         */
#define X_START_LINE     ( 10 )     /* Pixel x-pos for start of division line */
#define X_END_LINE       ( 300 )    /* Pixel x-pos for end of division line   */
#define X_PHILOSOPHER    ( 10 )     /* Pixel x-pos Philosopher column         */
#define X_STATE          ( 80 )     /* Pixel x-pos Philosopher state column   */
#define X_PORTIONS       ( 237 )    /* Pixel x-pos Philosopher portions column*/

#define Y_HEADER         ( 1 )      /* Pixel y-pos for headerline             */
#define Y_TITLE          ( 25 )     /* Pixel y-pos for titles                 */
#define Y_PHILOSOPHER    ( 55 )     /* Pixel y-pos for first philosopher      */
#define Y_INCREMENT      ( 14 )     /* Pixel between philosopher              */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------

//----- Data -------------------------------------------------------------------
static SemaphoreHandle_t mutexLCD;

/* Philosopher State Text */
static const char* pcPhilosopherState[] = {
    "thinking         ",  ///< thinking text
    "Wait table access",  ///< table text
    "Wait left fork   ",  ///< left fork text
    "Wait right fork  ",  ///< right fork text
    "eating           "   ///< eating text
};

static const char* pcHello       = "The Dining Philosophers"; /* Welcome text      */
static const char* pcPhilosopher = "Who";                     /* Philosopher text  */
static const char* pcState       = "Action";                  /* Philosopher state */
static const char* pcPortions    = "Portions";                /* Portions eaten    */

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
void vInitDisplay(void) {

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
void vDisplayStaticText(void) {

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
                 Y_TITLE + Y_INCREMENT,
                 X_END_LINE,
                 Y_TITLE + Y_INCREMENT,
                 GUI_COLOR_GREEN);

    /* Display philosopher */
    for (i = 0; i < NUMBER_OF_PHILOSOPHERS; i++) {
        sprintf(cBuffer, "P%d:", (int)i + 1);
        LCD_DisplayStringXY(X_START_LINE,
                            Y_PHILOSOPHER + (i * Y_INCREMENT * 2),
                            cBuffer);
    }
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
void  vDisplayState(uint8_t u8Philosopher, PhilosopherStates ePhilosopherStates) {

    if (xSemaphoreTake(mutexLCD, portMAX_DELAY) == pdTRUE) {

        LCD_DisplayStringXY(X_STATE,
                            Y_PHILOSOPHER + (u8Philosopher * Y_INCREMENT * 2),
                            pcPhilosopherState[ePhilosopherStates]);

        xSemaphoreGive(mutexLCD);  /* Release semaphore */
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
void  vDisplayPortions(uint8_t u8Philosopher, uint32_t u32Portions) {

    char   cBuffer[8];

    if (xSemaphoreTake(mutexLCD, portMAX_DELAY) == pdTRUE) {
        sprintf(cBuffer, "%d", (int) u32Portions);
        LCD_DisplayStringXY(X_PORTIONS,
                            Y_PHILOSOPHER + (u8Philosopher * Y_INCREMENT * 2),
                            cBuffer);

        xSemaphoreGive(mutexLCD);  /* Release semaphore */
    }
}
