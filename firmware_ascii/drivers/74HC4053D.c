/**********************************************************************************************************************
File: 74HC4053D.c                                                                

Description:
This is a 74HC4053D.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:



**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp2"
***********************************************************************************************************************/
/* New variables */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type _74HC4053D_APP_StateMachine;            /* The state machine function pointer */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------
Function: _74HC4053D_SWITCH_XY

Description:
Choose AX or AY, BX or BY, CX or CY

Promises:
*/
void _74HC4053D_SWITCH_XY(C4053DSwitchType SwitchType)
{
	switch(SwitchType)
	{
		case AX:
		{
			AT91C_BASE_PIOA->PIO_CODR = _74HC4053D_A;
			break;
		}
		
		case AY:
		{
			AT91C_BASE_PIOA->PIO_SODR = _74HC4053D_A;
			break;
		}
		
		case BX:
		{
			AT91C_BASE_PIOB->PIO_CODR = _74HC4053D_B;
			break;
		}
		
		case BY:
		{
			AT91C_BASE_PIOB->PIO_SODR = _74HC4053D_B;
			break;
		}
		
		case CX:
		{
			AT91C_BASE_PIOA->PIO_CODR = _74HC4053D_C;
			break;
		}
		
		case CY:
		{
			AT91C_BASE_PIOA->PIO_SODR = _74HC4053D_C;
			break;
		}
		
		default:
		{
			break;
		}
	}
} /* end _74HC4053D_SWITCH_XY */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: X9C103S_APP_INIT

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void _74HC4053D_APP_INIT(void)
{
	/* PIOA enable */
	AT91C_BASE_PIOA->PIO_PER |= 0x00010800;
	AT91C_BASE_PIOA->PIO_OER |= 0x00010800;
	
	/* PIOB enable */
	AT91C_BASE_PIOB->PIO_PER |= 0x00000010;
	AT91C_BASE_PIOB->PIO_OER |= 0x00000010;
	
	/* PIOA initialize */
	AT91C_BASE_PIOA->PIO_CODR = 0x00010800;
	
	/* PIOB initialize */
	AT91C_BASE_PIOB->PIO_CODR = 0x00000010;
	
	/* If good initialization, set state to Idle */
	if( 1 )
	{
		_74HC4053D_APP_StateMachine = _74HC4053D_SM_Idle;
	}
	else
	{
		/* The task isn't properly initialized, so shut it down and don't run */
		_74HC4053D_APP_StateMachine = _74HC4053D_SM_Error;
	}
} /* end X9C103S_APP_INIT() */


/*----------------------------------------------------------------------------------------------------------------------
Function X9C103S_APP_RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void _74HC4053D_APP_RunActiveState(void)
{
	_74HC4053D_APP_StateMachine();
} /* end X9C103S_APP_RunActiveState() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void _74HC4053D_SM_Idle(void)
{
} /* end _74HC4053D_SM_Idle() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void _74HC4053D_SM_Error(void)          
{
  
} /* end _74HC4053D_SM_Error() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
