/******************************************************************************/
/** \file       EZBSY_U1A2.c
 *******************************************************************************
 *
 *  \brief      EZBSY exercise U1A2, Task creation, starvation
 *
 *  \author     wht4
 *
 *  \date       22.08.2011
 *
 *  \remark     Last Modification
 *               \li wht4, 24.01.2014, Created
 *               \li wht4, 06.01.2015, Migrated to FreeRTOS V8.0.0
 *               \li WBR1, 09.02.2017, minor optimizations
 *
 ******************************************************************************/
/*
 *  functions  global:
 *              main
 *  functions  local:
 *              vAppTask1
 *              vAppTask2
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>                      /* CARME Module                       */
#include <carme_io1.h>                  /* CARMEIO1 Board Support Package     */
#include <carme_io2.h>
#include <lcd.h>

#include <stdio.h>                      /* Standard Input/Output              */
#include <stdlib.h>                     /* General Utilities                  */
#include <string.h>                     /* String handling                    */

#include <FreeRTOS.h>                   /* All freeRTOS headers               */
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <memPoolService.h>

//----- Macros -----------------------------------------------------------------
#define STACKSIZE_TASK1        ( 256 )
#define STACKSIZE_TASK2        ( 256 )
#define STACKSIZE_TASK3        ( 256 )

#define SPEED                  ( 100 )
#define PRIORITY_TASK1         ( 1 )
#define PRIORITY_TASK2         ( 1 )
#define PRIORITY_TASK3         ( 1 )

#define Y_HEADERLINE  ( 1 )     /* Pixel y-pos for headerline                 */
#define Y_VALUE       ( 100 )   /* Pixel y-pos for value                      */
#define Y_INSTR1      ( 180 )   /* Pixel y-pos for instruction1 text          */
#define Y_INSTR2      ( 200 )   /* Pixel y-pos for instruction2 text          */
#define X_BORDER      ( 18 )    /* Pixel x-pos for border                     */

#define BUTTON_T0     ( 0x01 )  /* Value returned if Button T0 is pushed      */
#define BUTTON_T1     ( 0x02 )  /* Value returned if Button T1 is pushed      */
#define BUTTON_T2     ( 0x04 )  /* Value returned if Button T2 is pushed      */
#define BUTTON_T3     ( 0x08 )  /* Value returned if Button T3 is pushed      */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void  vAppTask1(void *pvData);
static void  vAppTask2(void *pvData);
static void  vAppTask3(void *pvData);

//----- Data -------------------------------------------------------------------
static uint8_t u8SwitchState;
static uint8_t u8ButtonState;
static const char *pcHello = "EZBSY U1A4";
static const char *pcButtonsValueText = "Buttons = %d";
static const char *pcSwitchesValueText = "Switches = %d";

//----- Implementation ---------------------------------------------------------

/*******************************************************************************
 *  function :    main
 ******************************************************************************/
/** \brief        Initialize GUI, BSP and OS
 *
 *  \type         global
 *
 *  \return       error code
 *
 ******************************************************************************/
int  main(void) {

    TaskHandle_t taskHandle;

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    CARME_IO1_Init();
    CARME_IO2_Init();
    CARME_Init();
    CARME_IO1_LED_Set(0x01, 0xFF);  // Turn on LED 1 and 8

    LCD_Init();
    LCD_SetFont(&font_8x16B);
    LCD_DisplayStringCenterLine(Y_HEADERLINE, pcHello);
    LCD_SetFont(&font_8x13);


    xTaskCreate(vAppTask1,
                "Task1",
                STACKSIZE_TASK1,
                NULL,
                PRIORITY_TASK1,
                &taskHandle);

    xTaskCreate(vAppTask2,
                "Task2",
                STACKSIZE_TASK2,
                NULL,
                PRIORITY_TASK2,
                &taskHandle);
    
    xTaskCreate(vAppTask3,
                "Task3",
                STACKSIZE_TASK3,
                NULL,
                PRIORITY_TASK3,
                &taskHandle);

    vTaskStartScheduler();

    /* code never reached */
    for (;;) {
    }

    return 0;
}

/*******************************************************************************
 *  function :    AppTask1
 ******************************************************************************/
/** \brief        reads the switches
 *
 *  \type         local
 *
 *  \param[in]    pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
static void vAppTask1(void *pvData) {

    while(1){

        /* Get switch states */
        CARME_IO1_SWITCH_Get(&u8SwitchState);
        vTaskDelay(SPEED);
    }
}

/*******************************************************************************
 *  function :    AppTask2
 ******************************************************************************/
/** \brief        reads the buttons
 *
 *  \type         local
 *
 *  \param[in]    pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
static void  vAppTask2(void *pvData) {

    while(1) {

        /* Get button states */
        CARME_IO1_BUTTON_Get(&u8ButtonState);
        vTaskDelay(SPEED);
    }
}

/*******************************************************************************
 *  function :    AppTask3
 ******************************************************************************/
/** \brief        Prints the values on the lcd
 *
 *  \type         local
 *
 *  \param[in]    pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
static void  vAppTask3(void *pvData) {

    char cBuffer[64];

    while(1) {
        sprintf(cBuffer, pcButtonsValueText, (int) u8ButtonState);
        LCD_DisplayStringCenterLine(10, cBuffer);
        sprintf(cBuffer, pcSwitchesValueText, (int) u8SwitchState);
        LCD_DisplayStringCenterLine(11, cBuffer);

        vTaskDelay(SPEED);
    }
}
