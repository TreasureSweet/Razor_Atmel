/**********************************************************************************************************************
File: X9C103S.c                                                                

Description:
This is a X9C103S.c file template 

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
static fnCode_type X9C103S_APP_StateMachine;            /* The state machine function pointer */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------
Function: DelayU8

Description:
Do <u8> times loop to simulate delay.

Promises:
Need an u8 input 0 ~ 255.
*/
void DelayU8(u8 u8Time)
{
	for(u8 i = u8Time; i; i--)
	{
	};
} /* end DelayU8 */

/*-------------------------------------------------------------------------------------------------------------------
Function: X9C103S_RESISTANCE_UP

Description:
Add X9C103S resistance u8time times (wiper down)

Promises:
*/
void X9C103S_RESISTANCE_UP(u8 u8time)
{
	for(u8 i = u8time; i; i--)
	{
		/* Enable wiper control */
		AT91C_BASE_PIOA->PIO_CODR = X9C103S_CS;  // CS to low (Enable wiper change)
		AT91C_BASE_PIOA->PIO_CODR = X9C103S_UD;  // UD to low (Wiper down)
		DelayU8(2);

		/* INC change from high to low (change wiper once) */
		AT91C_BASE_PIOA->PIO_SODR = X9C103S_INC; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_CODR = X9C103S_INC; // INC to low
		DelayU8(2);

		/* CS change from low to high (store wiper position)*/
		AT91C_BASE_PIOA->PIO_SODR = X9C103S_INC; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_SODR = X9C103S_CS;  // CS to high
		DelayU8(2);
	}
	
} /* end X9C103S_RESISTANCE_UP() */

/*-------------------------------------------------------------------------------------------------------------------
Function: X9C103S_RESISTANCE_UP

Description:
Reduce X9C103S resistance u8time times (wiper up)

Promises:
*/
void X9C103S_RESISTANCE_DOWN(u8 u8time)
{
	for(u8 i = u8time; i; i--)
	{
		/* Enable wiper control */
		AT91C_BASE_PIOA->PIO_CODR = X9C103S_CS;  // CS to low (Enable wiper change)
		AT91C_BASE_PIOA->PIO_SODR = X9C103S_UD;  // UD to high (Wiper up)
		DelayU8(2);

		/* INC change from high to low (change wiper once) */
		AT91C_BASE_PIOA->PIO_SODR = X9C103S_INC; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_CODR = X9C103S_INC; // INC to low
		DelayU8(2);

		/* CS change from low to high (store wiper position)*/
		AT91C_BASE_PIOA->PIO_SODR = X9C103S_INC; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_SODR = X9C103S_CS;  // CS to high
		DelayU8(2);
	}
	
} /* end X9C103S_RESISTANCE_DOWN() */


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
void X9C103S_APP_INIT(void)
{
	/* PIOA enable */
	AT91C_BASE_PIOA->PIO_PER |= 0x00007000;
	AT91C_BASE_PIOA->PIO_OER |= 0x00007000;

	/* PIOA initialize */
	AT91C_BASE_PIOA->PIO_SODR = 0x00002000;
	AT91C_BASE_PIOA->PIO_CODR = 0x00005000;
	
	/* If good initialization, set state to Idle */
	if( 1 )
	{
		X9C103S_APP_StateMachine = X9C103S_SM_Idle;
	}
	else
	{
		/* The task isn't properly initialized, so shut it down and don't run */
		X9C103S_APP_StateMachine = X9C103S_SM_Error;
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
void X9C103S_APP_RunActiveState(void)
{
	X9C103S_APP_StateMachine();
} /* end X9C103S_APP_RunActiveState() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void X9C103S_SM_Idle(void)
{
} /* end X9C103S_SM_Idle() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void X9C103S_SM_Error(void)          
{
  
} /* end X9C103S_SM_Error() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
