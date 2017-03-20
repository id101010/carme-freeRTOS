#ifndef LCDFUNCTION_H_
#define LCDFUNCTION_H_
/******************************************************************************/
/** \file       lcdFunction.h
 *******************************************************************************
 *
 *  \brief      Functions to display the data for the philosophers task
 *
 *  \author     wht4
 *
 ******************************************************************************/
/*
 *  function    vInitDisplay
 *              vDisplayStaticText
 *              vDisplayState
 *              vDisplayPortions
 *              vDisplayCookState
 *              vDisplayQueuedSpaghetti
 *
 ******************************************************************************/

//----- Header-Files -----------------------------------------------------------
#include <carme.h>
#include "philosopherTask.h"
#include "cookTask.h"

//----- Macros -----------------------------------------------------------------

//----- Data types -------------------------------------------------------------

//----- Function prototypes ----------------------------------------------------
extern  void  vInitDisplay(void);
extern  void  vDisplayStaticText(void);
extern  void  vDisplayState(uint8_t u8Philosopher, PhilosopherStates ePhilosopherStates);
extern  void  vDisplayPortions(uint8_t u8Philosopher, uint32_t u32Portions);
extern  void  vDisplayCookState(CookState eCookState, SpaghettiTypes eSpaghettiTypes);
extern  void  vDisplayQueuedSpaghetti(uint8_t u8SpaghettiQueued);

//----- Data -------------------------------------------------------------------

#endif /* LCDFUNCTION_H_ */
