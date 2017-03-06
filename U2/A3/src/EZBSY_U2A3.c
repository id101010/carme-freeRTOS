/******************************************************************************/
/** \file       EZBSY_U2A3.c
 *******************************************************************************
 *
 *  \brief      EZBSY exercise U2A3
 *
 *  \author     schma5
 *
 *  \date       06.03.2017
 *
 *  \remark     Last Modification
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
#define PRIORITY_TASK2         ( 1 )  // TODO: modify priority, low priority number
                                      // denotes low priority task

//#define USE_vWAIT                   // TODO: set this compiler switch to use CPU-wait
                                      // instead of vTastDelay

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

    while(1) {
        CARME_IO2_ADC_Get(CARME_IO2_ADC_PORT0, &u16Speed);
#ifdef USE_vWAIT
        vWait();
#else
        vTaskDelay(u16Speed);
#endif
    }
}

/*******************************************************************************
 *  function :    wait
 ******************************************************************************/
/** \brief        Wait some time using CPU
 *
 *  \type         local
 *
 *  \return       void
 *
 ******************************************************************************/
#ifdef USE_vWAIT
static void vWait(void) {

    uint32_t i, j;

    for (i=0; i<1000; i++) {
        for (j=0; j<2000; j++) {
        }
    }
}
#endif
