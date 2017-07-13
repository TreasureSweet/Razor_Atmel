/**********************************************************************************************************************
File: user_app2.c                                                                

Description:
This is a user_app2.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp2Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp2RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_UserApp2"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp2Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
//static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */


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
Function: UserApp2Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp2Initialize(void)
{
	LedOff(WHITE);
	LedOff(RED);
	LedOff(GREEN);
	LedOn(YELLOW);
	LedOn(ORANGE);
	LedOff(PURPLE);
	PWMAudioOff(BUZZER1);
	
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp2_StateMachine = UserApp2SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp2_StateMachine = UserApp2SM_FailedInit;
  }

} /* end UserApp2Initialize() */

  
/*----------------------------------------------------------------------------------------------------------------------
Function UserApp2RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp2RunActiveState(void)
{
  UserApp2_StateMachine();

} /* end UserApp2RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp2SM_Idle(void)
{
	static u32 u32Password=1111111111;
	static u32 u32PasswordJudge=0;
	static u32 u32Choose=0;
	static bool bLightOn=FALSE;
	static u8 u8PreeBlink=0;
	static u16 u16ChangeBlink=0;

	u8 u8ButtonNum=6;

	/*Give every button a num*/
	if(WasButtonPressed(BUTTON0))
	{
		ButtonAcknowledge(BUTTON0);
		u8ButtonNum=1;
		LedOn(WHITE);
		bLightOn=TRUE;
		PWMAudioSetFrequency(BUZZER1,1046);
		PWMAudioOn(BUZZER1);
	}

	if(WasButtonPressed(BUTTON1))
	{
		ButtonAcknowledge(BUTTON1);
		u8ButtonNum=2;
		LedOn(WHITE);
		bLightOn=TRUE;
		PWMAudioSetFrequency(BUZZER1,1174);
		PWMAudioOn(BUZZER1);
	}

	if(WasButtonPressed(BUTTON2))
	{
		ButtonAcknowledge(BUTTON2);
		u8ButtonNum=3;
		LedOn(WHITE);
		bLightOn=TRUE;
		PWMAudioSetFrequency(BUZZER1,1318);
		PWMAudioOn(BUZZER1);
	}

	if(WasButtonPressed(BUTTON3))
	{
		ButtonAcknowledge(BUTTON3);
		u8ButtonNum=4;
		LedOn(WHITE);
		bLightOn=TRUE;
		PWMAudioSetFrequency(BUZZER1,1396);
		PWMAudioOn(BUZZER1);
	}
	
	if(IsButtonHeld(BUTTON3,1500))
	{
		u8ButtonNum=5;
	}

	/*If you want to change password,you must hold BUTTON3 the first 2 scends*/
	if(u32Choose<2000)
	{
		u32Choose++;

		/*Password change when hold BUTTON3 1.5s*/
		if(u8ButtonNum==5)
		{
			LedOff(ORANGE);
			LedOff(YELLOW);
			LedOn(RED);
			u32Choose=2001;
			u32Password=0;
		}
		
		if(u32Choose==2000)
		{
			LedOff(ORANGE);
			LedOff(YELLOW);
			LedOn(RED);
		}
	}
	
	/*The lock mode.When u32Choose==2000*/
	if(u32Choose==2000)
	{
		/*Inport password and distinguish true or false*/
		if(u8ButtonNum<4)
		{
			u32PasswordJudge=10*u32PasswordJudge+u8ButtonNum;
		}

		/*Distinguish when BUTTON4 is pressed*/
		if(u8ButtonNum==4)
		{
			if(u32PasswordJudge==u32Password)//Green blink when true and turn off red
			{
				LedOff(RED);
				LedOn(GREEN);
				u8ButtonNum=6;
				u32Choose=2002;
			}
			else//Red blink when false
			{
				LedBlink(RED,LED_4HZ);
				u32Choose=2003;
			}
		}
	}
	
	/*The password change mode.When u32Choose==2001*/
	if(u32Choose==2001)
	{
		/*Blink the red an green light ever 1s*/
		u16ChangeBlink++;

		if(u16ChangeBlink==500)
		{
			LedToggle(RED);
			LedToggle(GREEN);
			u16ChangeBlink=0;
		}

		/*Import password with button0~2,and use button3 to save*/
		if(u8ButtonNum<4)
		{
			u32Password=10*u32Password+u8ButtonNum;
		}

		/*Identify and pop-up mode*/
		if(u8ButtonNum==4)
		{
			u32Choose=2000;
			LedOn(RED);
			LedOff(GREEN);
			u8ButtonNum=0;
			u16ChangeBlink=0;
		}
	}

	/*If true,go to lock when button3 pressed*/
	if(u32Choose==2002&&u8ButtonNum==4)
	{
		u32Choose=2000;
		u32PasswordJudge=0;
		LedOn(RED);
		LedOff(GREEN);
	}
	
	/*If false,blink red 2s then go to lock*/
	if(u32Choose>=2003&&u32Choose!=4000)
	{
		u32Choose++;
	}
	
	if(u32Choose==4000)
	{
		u32Choose=2000;
		u32PasswordJudge=0;
		LedOn(RED);
		LedOff(GREEN);
	}
	
	/*Push button light*/
	if(bLightOn)
	{
		if(u8PreeBlink++==200)
		{
			u8PreeBlink=0;
			LedOff(WHITE);
			LedOff(PURPLE);
			PWMAudioOff(BUZZER1);
			bLightOn=FALSE;
		}
	}
} /* end UserApp2SM_Idle() */
     
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp2SM_Error(void)          
{

} /* end UserApp2SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp2SM_FailedInit(void)          
{
    
} /* end UserApp2SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
