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
//#include <carme_io1.h>                  /* CARMEIO1 Board Support Package     */
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

//#define USE_vWAIT               // TODO: set this compiler switch to use CPU-wait
                                // instead of vTastDelay

//ADC stuff
#define SQR3_SQ_SET            ((uint32_t)0x0000001F)  
#define SMPR1_SMP_SET          ((uint32_t)0x00000007)
//Mask for the ADC value (to cut away bits which cannot be valid)
#define ADC_MASK 0x3FF 
//Value that should be threated as maximum
#define ADC_MAX 0x3A0
//Tolerance value. Changes below this value will be ignored.
#define ADC_TOLERANCE 0x08

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
static void  vAppTask1(void *pvData);
static void  vAppTask2(void *pvData);
static void  adc_init(void);
static uint16_t adc_read(void);

#ifdef USE_vWAIT
static void  vWait(void);
#endif

//----- Data -------------------------------------------------------------------
uint16_t u16Speed;
volatile unsigned char* LED = (volatile unsigned char*)0x6C000200;

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

    CARME_IO1_Init();               // Initialize the CARMEIO1
    CARME_IO1_LED_Set(0, 0xFF);     // Reset led state

    /* initialize adc */
    adc_init();

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

    uint8_t     u8LED1to6;
    uint8_t     u8Led;

    while(1) {

    /* Get LED state, shift left, check overflow and show again */
    CARME_IO1_LED_Get(&u8Led);
    u8LED1to6 = u8Led & 0xFF;                /* Mask all bits */
    u8LED1to6 <<= 1;                         /* Shift LED left one position */
    if (u8LED1to6 == 0x80)   {
        u8LED1to6 = 1;                       /* LED 7 on?, set LED1 */
    }
    CARME_IO1_LED_Set(u8LED1to6, 0xFF);

#ifdef USE_vWAIT
        vWait();
#else
        vTaskDelay(100);
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
        u16Speed = adc_read();
#ifdef USE_vWAIT
        vWait();
#else
        vTaskDelay(100);
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

/*******************************************************************************
 *  function :    read_init
 ******************************************************************************/
/** \brief        initializes the analog to digital converter
 *
 *  \type         local
 *
 *  \return       void
 *
 ******************************************************************************/
void adc_init() {
    //Enable the peripheral clock of GPIOB
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOB;
    //Choose the working mode of PB0 with the GPIO port mode register
    GPIOB->MODER &= ~GPIO_MODER_MODER0;
    GPIOB->MODER |=  GPIO_Mode_AN;
    //Configure the GPIO port pull-up/pull-down register for PB0
    //Page 282/1718 of "RM0090 Reference Reference Manual (October 2014)"
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0;
    GPIOB->PUPDR |=  GPIO_PuPd_UP;
    //Initialize the ADC
    //Enable the peripheral clock of the ADC
    RCC->APB2ENR |= RCC_APB2Periph_ADC;
    //Configure ADC1: scan conversion mode and resolution
    //Set SCAN bit according to ADC_ScanConvMode value
    //Set RES bit according to ADC_Resolution value
    ADC1->CR1 = ADC_Resolution_10b;
    // Configure ADC1: regular channel sequence length
    // Set L bits according to ADC_NbrOfConversion value
    ADC1->SQR1 = 0;
    // Set the ADON bit to enable the ADC
    ADC1->CR2 = ADC_CR2_ADON;
}

/*******************************************************************************
 *  function :    read_adc
 ******************************************************************************/
/** \brief        reads the adc to which the potentiometer is connected
 *
 *  \type         local
 *
 *  \return       uint16_t adcvalue
 *
 ******************************************************************************/
uint16_t adc_read(){
    uint16_t value = 0;
    
    // Specify the sample time for the conversion
    ADC1->SMPR2 &= SMPR1_SMP_SET           << (3 * ADC_Channel_8);
    ADC1->SMPR2 |= ADC_SampleTime_15Cycles << (3 * ADC_Channel_8);
    // Set the channel 8 as the first conversion in the ADC reg seq register
    ADC1->SQR3  &= ~SQR3_SQ_SET;
    ADC1->SQR3  |= ADC_Channel_8;
    // Start the conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;   
    // Wait until the conversion has been done
    while( (ADC1->SR & ADC_FLAG_EOC) == 0 );
    // Read the value
    value = ADC1->DR;
    value &= ADC_MASK;
    
    return value;
}
