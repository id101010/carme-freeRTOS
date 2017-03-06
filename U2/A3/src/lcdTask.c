/******************************************************************************/
/** \file       lcdTask.c
 *******************************************************************************
 *
 *  \brief      Display the state of the buttons and the switches. This
 *              Information is obtained out of the global variables
 *              u8SwitchState and u8ButtonState.
 *
 *  \author     wht4
 *
 *  \date       24.08.2011
 *
 *  \remark     Last Modification
 *               \li wht4, 24.08.2011, Created
 *               \li wht4, 24.01.2014, Adapted to CARME-M4
 *               \li WBR1, 21.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              vLCDTask
 *  functions  local:
 *              .
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <lcd.h>                        /* GUI Library                        */
#include <carme_io1.h>                  /* CARMEIO1 Board Support Package     */

#include <stdio.h>
#include <stdlib.h>

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

//----- Macros -----------------------------------------------------------------
#define Y_HEADERLINE    ( 1 )          /* Pixel y-pos for headerline          */
#define Y_SWITCH        ( 80 )         /* Pixel y-pos for the switch state    */
#define Y_BUTTON        ( 100 )        /* Pixel y-pos for the button state    */
#define X_BORDER        ( 10 )         /* Pixel x-pos for normal boarder      */
#define X_VALUE         ( 100 )        /* Pixel x-pos where to put the values */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void Number2BinaryString(uint32_t u32Number,
                                uint32_t u32NumberOfBit,
                                char * pcBuffer);

//----- Data -------------------------------------------------------------------
static const char* pcHello = "CARME-Kit states"; /* Welcome text                 */
static const char* pcSwitchText = "Switch: ";    /* Text to display switch state */
static const char* pcButtonText = "Button: ";    /* Text to display button state */

uint8_t u8ButtonState = 0;                       /* Button State */
uint8_t u8SwitchState = 0;                       /* Switch State */

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    vLCDTask
 ******************************************************************************/
/** \brief        Display the state of the buttons and the switches. This
 *                Information is obtained out of the global variables
 *                u8SwitchState and u8ButtonState.
 *
 *  \type         global
 *
 *  \param[in]	  pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
void  vLCDTask(void *pvData) {

    char cBuffer[12];
    uint8_t switchState;			/* local copy for switch state			*/
    uint8_t buttonState;			/* local copy for button state			*/

    /* Initialize the Display and display static text */
    LCD_Init();
    LCD_SetFont(&font_8x16B);
    LCD_DisplayStringCenterLine(Y_HEADERLINE, pcHello);
    LCD_SetFont(&font_8x13);
    LCD_DisplayStringXY(X_BORDER, Y_SWITCH, pcSwitchText);
    LCD_DisplayStringXY(X_BORDER, Y_BUTTON, pcButtonText);

	for (;;) {
        /* copy switchState and buttonState from global variables, this is an access to a
		 * critical section, so disable interrupts while reading the variable
		 */
		taskENTER_CRITICAL();	/* disable interrupts */
		switchState = u8SwitchState; /* access critical section */
		buttonState = u8ButtonState; /* access critical section */
		taskEXIT_CRITICAL();	/* enable interrupts again */

        /* Display switch state */
        Number2BinaryString((uint32_t) switchState, 8, cBuffer);
        LCD_DisplayStringXY(X_VALUE, Y_SWITCH, cBuffer);

        /* Display button state */
        Number2BinaryString((uint32_t) buttonState, 4, cBuffer);
        LCD_DisplayStringXY(X_VALUE, Y_BUTTON, cBuffer);

        vTaskDelay(50 / portTICK_RATE_MS);
    }
}


/*******************************************************************************
 *  function :    Number2BinaryString
 ******************************************************************************/
/** \brief        Generate the binary representation of a number as binary
 *                string
 *
 *  \type         static
 *
 *  \param[in]	  u32Number      number to decode to it's binary representation
 *  \param[in]    u32NumberOfBit number of bits to decode
 *  \param[out]   pcBuffer       buffer to store the binary represantation of
 *                               <CODE>u32Number</CODE> as a '/0' terminated
 *                               string. Thus the size of pcBuffer must be
 *                               equal or greater as
 *                               (<Code>u32NumberOfBit</CODE> + 1)
 *
 *  \return       void
 *
 ******************************************************************************/
static void Number2BinaryString(uint32_t u32Number,
                                uint32_t u32NumberOfBit,
                                char * pcBuffer) {

    uint32_t i = 0;

    if(pcBuffer != NULL) {

        for(i = 0; i < u32NumberOfBit; i++) {

            if(((1 << i) & u32Number) > 0) {

                pcBuffer[(u32NumberOfBit - 1) - i] = '1';

            } else {

                pcBuffer[(u32NumberOfBit - 1) - i] = '0';
            }
        }

        pcBuffer[u32NumberOfBit] = '\0';
    }
}
