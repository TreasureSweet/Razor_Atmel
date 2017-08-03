/**********************************************************************************************************************
File: user_app3.c                                                                

Description:
This is a user_app3.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp3Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp3RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp3"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp3Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */
extern u32 u32Time_Counter;                            /* From main.c */
extern bool bButtonPressed;                            /* From user_app1 */
extern bool bEmpty;                                    /* From user_app1 */
extern bool bUserChoose;                               /* From user_app1 */
extern bool bLcdOn;                                    /* From user_app1 */
extern u16 u16LcdOnTime;                             /* From user_app1 */
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp3_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp3_StateMachine;            /* The state machine function pointer */
//static u32 UserApp3_u32Timeout;                      /* Timeout counter used across states */


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp3Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp3Initialize(void)
{
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp3_StateMachine = UserApp3SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp3_StateMachine = UserApp3SM_FailedInit;
  }

} /* end UserApp3Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp3RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp3RunActiveState(void)
{
  UserApp3_StateMachine();

} /* end UserApp3RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp3SM_Idle(void)
{
	static u8 u8ButtonTime=0;
	static u16 u16ClearLcdTime=0;
	static bool bOn=FALSE;
	
	if(bEmpty)
	{
		bEmpty=FALSE;
		bUserChoose=FALSE;
		LCDClearChars(LINE2_START_ADDR+9,1);
		bOn=TRUE;
		u16ClearLcdTime=0;
	}
	
	if(bOn)
	{
		if(u16ClearLcdTime++==2000)
		{
			u16ClearLcdTime=0;
			bOn=FALSE;
			LCDClearChars(LINE1_START_ADDR,20);
			LCDMessage(LINE1_START_ADDR+5,"Having Fun !");
		}
	}
	
	if(bLcdOn)
	{
		if(u16LcdOnTime++==5000)
		{
			u16LcdOnTime=0;
			bLcdOn=FALSE;
			LedOff(LCD_RED);
			LedOff(LCD_GREEN);
			LedOff(LCD_BLUE);
		}
	}
	
	if(bButtonPressed)
	{
		if(u8ButtonTime++==200)
		{
			u8ButtonTime=0;
			bButtonPressed=FALSE;
			PWMAudioOff(BUZZER1);
		}
	}
} /* end UserApp3SM_Idle() */
     
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp3SM_Error(void)          
{

} /* end UserApp3SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp3SM_FailedInit(void)          
{
    
} /* end UserApp3SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
