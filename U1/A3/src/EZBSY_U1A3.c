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
//#include <carme_io1.h>                /* CARMEIO1 Board Support Package     */
#include <carme_io2.h>

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

#define PRIORITY_TASK1         ( 1 )
#define PRIORITY_TASK2         ( 1 )

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void  vAppTask1(void *pvData);
static void  vAppTask2(void *pvData);

#ifdef USE_vWAIT
static void  vWait(void);
#endif

//----- Data -------------------------------------------------------------------
uint16_t u16Speed = 100;

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
    CARME_IO1_LED_Set(0x01, 0xFF);  // Turn on LED 1 and 8

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

    vTaskStartScheduler();

    /* code never reached */
    for (;;) {
    }

    return 0;
}

/*******************************************************************************
 *  function :    AppTask1
 ******************************************************************************/
/** \brief        Toggles led 0-7
 *
 *  \type         local
 *
 *  \param[in]    pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
static void vAppTask1(void *pvData) {

    uint8_t     u8LED1to8;
    uint8_t     u8Led = 0;

    u16Speed = 100;

    while(1){

        /* Get LED state, shift left, check overflow and show again */
        CARME_IO1_LED_Get(&u8Led);
     
        u8LED1to8 = u8Led & 0b11111111;          /* Mask all bits */
     
        if (u8LED1to8 == 0b10000000){
            u8LED1to8 = 0b00000001;              /* LED 8 on?, set LED1 */
        }else{
            u8LED1to8 <<= 1;                     /* Shift LED left one position */
        }
     
        CARME_IO1_LED_Set(u8LED1to8, 0xFF);
   
#ifdef USE_vWAIT
        vWait();
#else
        vTaskDelay(u16Speed);
#endif
    }
}

/*******************************************************************************
 *  function :    AppTask2
 ******************************************************************************/
/** \brief        reads the potentiometer
 *
 *  \type         local
 *
 *  \param[in]    pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
static void  vAppTask2(void *pvData) {

    uint16_t potiValue;

    while(1) {
        CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0, &potiValue);

        if(potiValue < 50){
            potiValue = 50;
        } 
        if(potiValue > 999){
          potiValue = 999;
        }

        taskENTER_CRITICAL();
        u16Speed = potiValue;
        taskEXIT_CRITICAL();
#ifdef USE_vWAIT
        vWait();
#else
        vTaskDelay(u16Speed);
#endif
    }
}
