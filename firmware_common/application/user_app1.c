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


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];						/* From debug.c */
extern u8 G_u8DebugScanfCharCount;						/* From debug.c */

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
	LedOff(ORANGE);
	LedOff(PURPLE);
	LedOff(WHITE);  //Button press
	LedOff(GREEN);  //When not pause
	LedOff(RED);    //When pause

	/* Welcome interface initialize */
	LCDCommand(LCD_CLEAR_CMD);
	LCDCommand(LCD_DISPLAY_CMD);
	LCDMessage(LINE1_START_ADDR+6,"Welcome!");
	
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
	static u8 u8ModeSelect=0;                    //0:WelcomeInterface 1:NameInterface 2:Display
	static u16 u16Time=0;
	static u8 u8Count=0;
	static u8 au8DisplayArray[256]=NULL;         //Keyboard input
	static bool bPause=FALSE;                    //Pause the modes except mode0
	static bool bKeyPress=FALSE;                 //Used to blink a led when press
	static bool bDisplayOn=FALSE;                //Control the blink of LCD
	
	/* Pause (Button3) */
	if(WasButtonPressed(BUTTON3)&&u8ModeSelect!=0)
	{
		ButtonAcknowledge(BUTTON3);
		LedToggle(GREEN);
		LedToggle(RED);
		LedOn(WHITE);
		bKeyPress=TRUE;
		bPause=!bPause;
	}/* Finish */
	
	
	/* Use keyboard to change the display */
	if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
	{
		G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]='\0';
		DebugScanf(au8DisplayArray);
		DebugPrintf("\n\rChange display successfully!\n\rPress button2 to show it.\n\r");
	}/* Finish */
	
	/* Welcome Interface */
	if(u8ModeSelect==0)
	{
		if(u16Time++==500)
		{
			u16Time=0;

			if(bDisplayOn)
			{
				LCDCommand(LCD_DISPLAY_CMD);
				LedToggle(PURPLE);
			}
			else
			{
				LCDCommand(LCD_DISPLAY_CMD | LCD_DISPLAY_ON);
				LedToggle(ORANGE);
			}

			bDisplayOn=!bDisplayOn;

			if(u8Count++==8)
			{
				u8Count=0;
				u8ModeSelect=1;
				LCDCommand(LCD_CLEAR_CMD);
				LedOff(PURPLE);
				LedOff(ORANGE);
				LedOn(GREEN);
			}
		}
	}/* Finish (can add lcd_led) */
	
	/* Every application needs to be paused should be written at the following if */
	if(!bPause)
	{
		/* Every Button */
		if(WasButtonPressed(BUTTON0))
		{
			ButtonAcknowledge(BUTTON0);
			LedOn(WHITE);
			bKeyPress=TRUE;
		}

		if(WasButtonPressed(BUTTON1))
		{
			ButtonAcknowledge(BUTTON1);
			LedOn(WHITE);
			bKeyPress=TRUE;
		}

		if(WasButtonPressed(BUTTON2))
		{
			ButtonAcknowledge(BUTTON2);		//Button2 used to display what keyboard input
			LedOn(WHITE);
			bKeyPress=TRUE;
			u8ModeSelect=2;
		}/*Not Finish*/

		/* Name Interface */
		if(u8ModeSelect==1)
		{
		}

		/* Display */
		if(u8ModeSelect==2)
		{
		}
	}
	
	/* Blink when button is pressed */
	if(bKeyPress)
	{
		if(u16Time++==200)
		{
			u16Time=0;
			LedOff(WHITE);
		}
	}	
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
