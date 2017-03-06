/******************************************************************************/
/** \file       EZBSY_U1A2.c
 *******************************************************************************
 *
 *  \brief      EZBSY exercise U1A2, Task creation, starvation
 *
 *  \author     schma5
 *
 *  \date       06.03.2017
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
 *              vAppTask3
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

#define PRIORITY_TASK1         ( 1 )
#define PRIORITY_TASK2         ( 1 )
#define PRIORITY_TASK3         ( 1 )

/* GUI Definitions */
#define Y_NBR_OF_TASKS    ( 10 )   /* pixel y-pos to put the number of tasks  */
#define Y_HEAP_LOAD       ( 30 )   /* pixel y-pos to put the heap load        */
#define Y_TITLE           ( 55 )   /* pixel y-pos where to put the stack title*/
#define Y_STACK           ( 80 )   /* pixel y-pos where to put the startline  */
#define Y_INCREMENT       ( 16 )   /* pixel increment for each task           */
#define Y_SPEED           ( 170 )  /* pixel y-pos where to put the speedline  */
#define Y_HELP            ( 190 )  /* pixel y-pos where to put the helpline   */
#define X_VALUE1          ( 100 )  /* pixel x-pos where to put the values     */
#define X_VALUE2          ( 160 )  /* pixel x-pos where to put the values     */
#define X_COMMENT         ( 16 )   /* pixel x-pos where to put the comment    */

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void vAppTask1(void *pvData);
static void vAppTask2(void *pvData);
static void vAppTask3(void *pvData);
static void vGetStackInfo(char* pcBuffer, xTaskHandle pTaskHandle);

//----- Data -------------------------------------------------------------------
TaskHandle_t   taskLED;      /* handles to tasks to read stack info    */
TaskHandle_t   taskPoti;
TaskHandle_t   taskLCD;

uint16_t u16Speed = 100;

/* GUI Stuff */
static const char* pcNumberOfTasks = "Number of Tasks: ";/* Number of Task text*/
static const char* pcHeapLoad      = "Free Heap: ";       /* Free Heap storage */
static const char* pcTitle         = "Task-Stack   Free stack space";/* Title text */
static const char* pcLine          = "------------------------";/* Line text */
static const char* pcLEDTask       = "LED Task";         /* LED task text */
static const char* pcPotiTask      = "Poti Task";        /* Poti task text */
static const char* pcLCDTask       = "LCD Task";         /* LCD task text */
static const char* pcPotiSpeed     = "Speed = ";         /* Poti speed text */

static const char* pcStackData     = "  %d byte"; /* Stack data fromating */

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

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    CARME_IO1_Init();
    CARME_IO2_Init();
    CARME_IO1_LED_Set(0x01, 0xFF);  // Turn on LED 1 and 8
    LCD_Init();

    /* Print static text */
    LCD_SetFont(&font_8x13);
    LCD_DisplayStringXY(X_COMMENT, Y_NBR_OF_TASKS, pcNumberOfTasks);
    LCD_DisplayStringXY(X_COMMENT, Y_HEAP_LOAD, pcHeapLoad);
    LCD_DisplayStringXY(X_COMMENT, Y_TITLE, pcTitle);
    LCD_DisplayStringXY(X_COMMENT, Y_TITLE+16, pcLine);
    LCD_DisplayStringXY(X_COMMENT, Y_STACK, pcLEDTask);
    LCD_DisplayStringXY(X_COMMENT, Y_STACK + Y_INCREMENT, pcPotiTask);
    LCD_DisplayStringXY(X_COMMENT, Y_STACK + 2 * Y_INCREMENT, pcLCDTask);
    LCD_DisplayStringXY(X_COMMENT, Y_SPEED, pcPotiSpeed);

    xTaskCreate(vAppTask1,
                "Task1",
                STACKSIZE_TASK1,
                NULL,
                PRIORITY_TASK1,
                &taskLED);

    xTaskCreate(vAppTask2,
                "Task2",
                STACKSIZE_TASK2,
                NULL,
                PRIORITY_TASK2,
                &taskPoti);

    xTaskCreate(vAppTask3,
                "Task3",
                STACKSIZE_TASK3,
                NULL,
                PRIORITY_TASK3,
                &taskLCD);

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
   
        vTaskDelay(u16Speed);
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
static void vAppTask2(void *pvData) {

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
     
        vTaskDelay(u16Speed);
    }
}

/*******************************************************************************
 *  function :    vGetStackInfo
 ******************************************************************************/
/** \brief        Get the stack information of a task
 *
 *  \type         global
 *
 *  \param[in]	  pcBuffer    Formatted information of stack load
 *  \param[in]    pTaskHandle Handle of the task of which we would like stack
 *                            information
 *
 *  \return       void
 *
 ******************************************************************************/
static void vGetStackInfo(char* pcBuffer, xTaskHandle pTaskHandle) {

    uint32_t u32FreeMemory = 0;

    /* read statistics and store them in string */
    u32FreeMemory = uxTaskGetStackHighWaterMark(pTaskHandle);

    sprintf(pcBuffer, pcStackData, (int)(u32FreeMemory << 2));

}


/*******************************************************************************
 *  function :    AppTask3
 ******************************************************************************/
/** \brief        prints stack information on the lcd
 *
 *  \type         local
 *
 *  \param[in]    pvData    not used
 *
 *  \return       void
 *
 ******************************************************************************/
static void vAppTask3(void *pvData) {

    char cBuffer[42];
    uint32_t u32Temp;
    uint16_t ledSpeed;

    while(1) {    
        /* Get the number of tasks and display them */
        u32Temp = uxTaskGetNumberOfTasks();
        sprintf(cBuffer, "%d", (int) u32Temp);
        LCD_DisplayStringXY(X_VALUE2, Y_NBR_OF_TASKS, cBuffer);

        /* Get the available Heap */
        u32Temp = xPortGetFreeHeapSize();
        sprintf(cBuffer, pcStackData, (int) u32Temp);
        LCD_DisplayStringXY(X_VALUE2, Y_HEAP_LOAD, cBuffer);

        /* Get and display stack information of LED task */
        vGetStackInfo(cBuffer, taskLED);
        LCD_DisplayStringXY(X_VALUE1, Y_STACK, cBuffer);

        /* Get and display stack information of Poti task */
        vGetStackInfo(cBuffer, taskPoti);
        LCD_DisplayStringXY(X_VALUE1, Y_STACK + Y_INCREMENT, cBuffer);

        /* Get and display stack information of LCD task */
        vGetStackInfo(cBuffer, taskLCD);
        LCD_DisplayStringXY(X_VALUE1, Y_STACK + 2 * Y_INCREMENT, cBuffer);

        taskENTER_CRITICAL();   /* disable interrupts */
        ledSpeed = u16Speed; /* access critical section */
        taskEXIT_CRITICAL();    /* enable interrupts again */

        sprintf(cBuffer, "%d", (int) ledSpeed);
        LCD_DisplayStringXY(X_VALUE2, Y_SPEED, cBuffer);

        vTaskDelay(u16Speed);
    }
}
