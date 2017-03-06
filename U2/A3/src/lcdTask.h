#ifndef LCDTASK_H_
#define LCDTASK_H_
/******************************************************************************/
/** \file       lcdTask.h
 *******************************************************************************
 *
 *  \brief      Display the state of the buttons and the switches. This
 *              Information is obtained out of the global variables
 *              u8SwitchState and u8ButtonState.
 *
 *  \author     wht4
 *
 ******************************************************************************/
/*
 *  function    vLCDTask
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
extern uint8_t u8ButtonState;          /* Switch state                        */
extern uint8_t u8SwitchState;          /* Switch state                        */

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
extern void vLCDTask(void *pvData);

//----- Data -------------------------------------------------------------------

#endif /* LCDTASK_H_ */
