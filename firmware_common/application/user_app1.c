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
	static u32 u32AllLedValue = PB_13_LED_WHT | PB_14_LED_PRP | PB_15_LED_ORG | PB_16_LED_CYN | PB_17_LED_YLW | PB_18_LED_BLU | PB_19_LED_GRN | PB_20_LED_RED;
	static u8 u8ToggleTime = 0;
	static bool bLedOn = FALSE;
	/*1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2*/
	static u16 au16NotesRight[]    = {G6, E6, F6, G6, E6, F6, G6, G5, A5, B5, C6, D6, E6, F6, E6, C6, D6, E6, E5, F5, G5, A5, G5, F5, G5, C6, B5, C6, A5, C6, B5, A5, G5, F5, G5, F5, E5, F5, G5, A5, B5, C6, A5, C6, B5, C6, B5, A5, B5, A5, B5, C6, D6, E6, F6, G6};
	static u16 au16DurationRight[] = {QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN};
	static u16 au16NoteTypeRight[] = {RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT};
	static u8 u8IndexRight   = 0;
	static u8 u8CurrentIndexRight = 0;
	static u32 u32RightTimer = 0;
	static u16 u16CurrentDurationRight = 0;
	static u16 u16NoteSilentDurationRight = 0;
	static bool bNoteActiveNextRight = TRUE;
	/*1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2   3   4   1   2*/
	static u16 au16NotesLeft[]    = {G5, E5, F5, G5, E5, F5, G5, G4, A4, B4, C5, D5, E5, F5, E5, C5, D5, E5, E4, F4, G4, A4, G4, F4, G4, C5, B4, C5, A4, C5, B4, A4, G4, F4, G4, F4, E4, F4, G4, A4, B4, C5, A4, C5, B4, C5, B4, A4, B4, A4, B4, C5, D5, E5, F5, G5};
	static u16 au16DurationLeft[] = {QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN, QN, EN, EN, QN, EN, EN, EN, EN, EN, EN, EN, EN, EN, EN};
	static u16 au16NoteTypeLeft[] = {RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT, RT};
	static u8 u8IndexLeft   = 0;
	static u8 u8CurrentIndexLeft = 0;
	static u32 u32LeftTimer = 0;
	static u16 u16CurrentDurationLeft = 0;
	static u16 u16NoteSilentDurationLeft = 0;
	static bool bNoteActiveNextLeft = TRUE;
	
	if( !(AT91C_BASE_PIOA->PIO_PDSR & PA_17_BUTTON0) ) // Press
	{
		if( u8ToggleTime++ == 250)
		{
			u8ToggleTime = 0;
			
			bLedOn = !bLedOn;
		}
		
		if(bLedOn)
		{
			AT91C_BASE_PIOB->PIO_SODR &= ~u32AllLedValue;
		}
		else
		{
			AT91C_BASE_PIOB->PIO_SODR |= u32AllLedValue;
		}
		
		/* Left Hand */
		if(IsTimeUp(&u32LeftTimer, (u32)u16CurrentDurationLeft))
		{
			u32LeftTimer = G_u32SystemTime1ms;
			u8CurrentIndexLeft = u8IndexLeft;

			/* Set up to play current note */     
			if(bNoteActiveNextLeft)     
			{
				if(au16NoteTypeLeft[u8CurrentIndexLeft] == RT)
				{
					u16CurrentDurationLeft = au16DurationLeft[u8CurrentIndexLeft] - REGULAR_NOTE_ADJUSTMENT;
					u16NoteSilentDurationLeft = REGULAR_NOTE_ADJUSTMENT;
					bNoteActiveNextLeft = FALSE;
				}
				else if(au16NoteTypeLeft[u8CurrentIndexLeft] == ST)
				{
					u16CurrentDurationLeft = STACCATO_NOTE_TIME;
					u16NoteSilentDurationLeft = au16DurationLeft[u8CurrentIndexLeft] - STACCATO_NOTE_TIME;
					bNoteActiveNextLeft = FALSE;
				}
				else if(au16NoteTypeLeft[u8CurrentIndexLeft] == HT)
				{
					u16CurrentDurationLeft = au16DurationLeft[u8CurrentIndexLeft];
					u16NoteSilentDurationLeft = 0;
					bNoteActiveNextLeft = TRUE;
					u8IndexLeft++;
					
					if(u8IndexLeft == sizeof(au16NotesLeft) / sizeof(u16) )
					{
						u8IndexLeft = 0;
					}
				}

				/* Set the buzzer frequency for the note (handle NO special case) */
				if(au16NotesLeft[u8CurrentIndexLeft] != NO)
				{
					PWMAudioSetFrequency(BUZZER1, au16NotesLeft[u8CurrentIndexLeft]);
					PWMAudioOn(BUZZER1);
				}
				else
				{
					PWMAudioOff(BUZZER1);
				}
			}
			else
			{
				PWMAudioOff(BUZZER1);
				u32LeftTimer = G_u32SystemTime1ms;
				u16CurrentDurationLeft = u16NoteSilentDurationLeft;
				bNoteActiveNextLeft = TRUE;
				u8IndexLeft++;
				
				if(u8IndexLeft == sizeof(au16NotesLeft) / sizeof(u16) )
				{
					u8IndexLeft = 0;
				}
			} /* end else if(bNoteActiveNextLeft) */
		} /* end if(IsTimeUp(&u32LeftTimer, (u32)u16CurrentDurationLeft)) */ 
		
		/* Right Hand */
		if(IsTimeUp(&u32RightTimer, (u32)u16CurrentDurationRight))
		{
			u32RightTimer = G_u32SystemTime1ms;
			u8CurrentIndexRight = u8IndexRight;

			/* Set up to play current note */     
			if(bNoteActiveNextRight)     
			{
				if(au16NoteTypeRight[u8CurrentIndexRight] == RT)
				{
					u16CurrentDurationRight = au16DurationRight[u8CurrentIndexRight] - REGULAR_NOTE_ADJUSTMENT;
					u16NoteSilentDurationRight = REGULAR_NOTE_ADJUSTMENT;
					bNoteActiveNextRight = FALSE;
				}
				else if(au16NoteTypeRight[u8CurrentIndexRight] == ST)
				{
					u16CurrentDurationRight = STACCATO_NOTE_TIME;
					u16NoteSilentDurationRight = au16DurationRight[u8CurrentIndexRight] - STACCATO_NOTE_TIME;
					bNoteActiveNextRight = FALSE;
				}
				else if(au16NoteTypeRight[u8CurrentIndexRight] == HT)
				{
					u16CurrentDurationRight = au16DurationRight[u8CurrentIndexRight];
					u16NoteSilentDurationRight = 0;
					bNoteActiveNextRight = TRUE;
					u8IndexRight++;
					
					if(u8IndexRight == sizeof(au16NotesRight) / sizeof(u16) )
					{
						u8IndexRight = 0;
					}
				}

				/* Set the buzzer frequency for the note (handle NO special case) */
				if(au16NotesRight[u8CurrentIndexRight] != NO)
				{
					PWMAudioSetFrequency(BUZZER2, au16NotesRight[u8CurrentIndexRight]);
					PWMAudioOn(BUZZER2);
				}
				else
				{
					PWMAudioOff(BUZZER2);
				}
			}
			else
			{
				PWMAudioOff(BUZZER2);
				u32RightTimer = G_u32SystemTime1ms;
				u16CurrentDurationRight = u16NoteSilentDurationRight;
				bNoteActiveNextRight = TRUE;
				u8IndexRight++;
				
				if(u8IndexRight == sizeof(au16NotesRight) / sizeof(u16) )
				{
					u8IndexRight = 0;
				}
			} /* end else if(bNoteActiveNextLeft) */
		} /* end if(IsTimeUp(&u32LeftTimer, (u32)u16CurrentDurationLeft)) */ 
	}
	else                                               // No press
	{
		u8ToggleTime = 0;
		bLedOn = FALSE;
		
		PWMAudioOff(BUZZER1);
		u8IndexLeft   = 0;
		u8CurrentIndexLeft = 0;
		u32LeftTimer = 0;
		u16CurrentDurationLeft = 0;
		u16NoteSilentDurationLeft = 0;
		bNoteActiveNextLeft = TRUE;
		
		PWMAudioOff(BUZZER2);
		u8IndexRight   = 0;
		u8CurrentIndexRight = 0;
		u32RightTimer = 0;
		u16CurrentDurationRight = 0;
		u16NoteSilentDurationRight = 0;
		bNoteActiveNextRight = TRUE;
		
		AT91C_BASE_PIOB->PIO_SODR &= ~u32AllLedValue;
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
