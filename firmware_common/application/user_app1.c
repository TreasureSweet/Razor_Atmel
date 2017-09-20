/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */

bool bState = TRUE;	// A value used to choose a module

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
	/* Buzzer initialize */
	PWMAudioSetFrequency(BUZZER1,200);
	PWMAudioOff(BUZZER1);

	/* Led initialize */
	LedOn(WHITE);
	LedOn(PURPLE);
	LedOn(BLUE);
	LedOn(CYAN);
	LedOff(GREEN);
	LedOff(YELLOW);
	LedOff(ORANGE);
	LedOff(RED);
	LedOn(LCD_RED);
	LedOn(LCD_BLUE);
	LedOff(LCD_GREEN);

	/* Lcd initialize */
	LCDCommand(LCD_CLEAR_CMD);
	LCDMessage(LINE1_START_ADDR,"STATE 1");
	
	/* Debug initialize */
	DebugPrintf("\n\r\n\rState 1\n\r");
	
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{	
	if(bState)	// Goto UserApp1SM_State1()
	{
		UserApp1_StateMachine = UserApp1SM_State1;
	}
	
	if(!bState)	// Goto UserApp1SM_State2()
	{
		UserApp1_StateMachine = UserApp1SM_State2;
	}
	
} /* end UserApp1SM_Idle() */

static void UserApp1SM_State1(void)
{
	/* Variables */
	static u8 u8Index = 0;
	static u8 au8DebugInput[256] = NULL;
	bool bInputCheck = FALSE;
	
	/* Module 1 */
	if(DebugScanf(&au8DebugInput[u8Index]) >= 1)
	{
		u8Index++;
		
		if((au8DebugInput[u8Index-1] == '\r'))	// Check if input "enter"
		{
			if((au8DebugInput[u8Index-2] == '2') && (2 == u8Index))	// Right format : 1<RC>
			{
				bInputCheck = TRUE;
			}
			else	// Default
			{
				DebugPrintf("\n\rInvalid Command\n\r");
			}
			
			u8Index = 0;
		}
	}
	
	/* If button2 is pressed or debug input 2 */
	if(WasButtonPressed(BUTTON2) || bInputCheck)
	{
		ButtonAcknowledge(BUTTON2);
		
		LedOff(WHITE);
		LedOff(PURPLE);
		LedOff(BLUE);
		LedOff(CYAN);
		LedBlink(GREEN,LED_1HZ);
		LedBlink(YELLOW,LED_2HZ);
		LedBlink(ORANGE,LED_4HZ);
		LedBlink(RED,LED_8HZ);
		LedOff(LCD_BLUE);
		LedPWM(LCD_GREEN,LED_PWM_20);
		
		LCDCommand(LCD_CLEAR_CMD);
		LCDMessage(LINE1_START_ADDR,"STATE 2");
		
		DebugPrintf("\n\r\n\rState 2\n\r");
		
		PWMAudioOn(BUZZER1);
		
		UserApp1_StateMachine = UserApp1SM_Idle;
		bState=FALSE;
	}
	
} /* end UserApp1SM_State1() */

static void UserApp1SM_State2(void)
{
	/* Variables */
	static u16 u16BuzzerTimeCount = 0;
	static u8 u8Index = 0;
	static u8 au8DebugInput[256] = NULL;
	bool bInputCheck = FALSE;
	
	/* Module 2 */
	if(DebugScanf(&au8DebugInput[u8Index]) >= 1)
	{
		u8Index++;
		
		if((au8DebugInput[u8Index-1] == '\r'))	// Check if input "enter"
		{
			if((au8DebugInput[u8Index-2] == '1') && (2 == u8Index)) // Right format : 1<RC>
			{
				bInputCheck = TRUE;
			}
			else	// Default
			{
				DebugPrintf("\n\rInvalid Command\n\r");
			}
			
			u8Index = 0;
		}
	}
	
	/*- Buzzer -*/
	u16BuzzerTimeCount++;
	
	if(100 == u16BuzzerTimeCount)// Turn off buzzer 100ms evey 1s
	{
		PWMAudioOff(BUZZER1);
	}
	
	if(1000 == u16BuzzerTimeCount)// Turn on buzzer evey 1s
	{
		u16BuzzerTimeCount = 0;
		PWMAudioOn(BUZZER1);
	}
	
	/* If button1 is pressed or debug input 1 */
	if(WasButtonPressed(BUTTON1) || bInputCheck)
	{
		ButtonAcknowledge(BUTTON1);
		
		LedOn(WHITE);
		LedOn(PURPLE);
		LedOn(BLUE);
		LedOn(CYAN);
		LedOff(GREEN);
		LedOff(YELLOW);
		LedOff(ORANGE);
		LedOff(RED);
		LedOn(LCD_BLUE);
		LedOff(LCD_GREEN);

		LCDCommand(LCD_CLEAR_CMD);
		LCDMessage(LINE1_START_ADDR,"STATE 1");
		
		DebugPrintf("\n\r\n\rState 1\n\r");
		
		u16BuzzerTimeCount = 0;
		PWMAudioOff(BUZZER1);
		
		UserApp1_StateMachine = UserApp1SM_Idle;
		bState=TRUE;
	}
	
} /* end UserApp1SM_State2() */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
