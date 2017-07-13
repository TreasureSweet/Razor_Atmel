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

extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                     /* From debug.c */

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
	PWMAudioOn(BUZZER1);
	
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
	static u8 au8ScanfArray[2];
	static u8 u8ButtonNum=1;
	static u8 u8Choose=0;
	u8 u8String[]="\n\rInput error!\n\r";
	u8 u8String1[]="\n\rSet OK!\n\r";
	u16 au16VoiceHz[]={65,69,73,78,82,87,92,98,104,110,117,123};

	if(WasButtonPressed(BUTTON0))
	{
		ButtonAcknowledge(BUTTON0);
		u8ButtonNum=1;
	}
	
	if(WasButtonPressed(BUTTON1))
	{
		ButtonAcknowledge(BUTTON1);
		u8ButtonNum=2;
	}
	
	if(WasButtonPressed(BUTTON2))
	{
		ButtonAcknowledge(BUTTON2);
		u8ButtonNum=3;
	}
	
	if(WasButtonPressed(BUTTON3))
	{
		ButtonAcknowledge(BUTTON3);
		u8ButtonNum=4;
	}
		
	if(G_u8DebugScanfCharCount==2)
	{
		DebugScanf(au8ScanfArray);
		
		if(au8ScanfArray[0]=='0'||au8ScanfArray[0]=='1')
		{
			if(au8ScanfArray[0]=='0')
			{
				if(au8ScanfArray[1]>='1'&&au8ScanfArray[1]<='9')
				{
					u8Choose+=(au8ScanfArray[1]-48);
					PWMAudioSetFrequency(BUZZER1,u8ButtonNum*au16VoiceHz[u8Choose-1]);
					DebugPrintf(u8String1);
				}
				else
				{
					DebugPrintf(u8String);
				}
			}
			
			if(au8ScanfArray[0]=='1')
			{
				u8Choose=10*(au8ScanfArray[0]-48);
				
				if(au8ScanfArray[1]>='0'&&au8ScanfArray[1]<='2')
				{
					u8Choose+=(au8ScanfArray[1]-48);
					PWMAudioSetFrequency(BUZZER1,u8ButtonNum*au16VoiceHz[u8Choose-1]);
					DebugPrintf(u8String1);
				}
				else
				{
					DebugPrintf(u8String);
				}
			}
		}
		else
		{
			DebugPrintf(u8String);
		}
		
		u8Choose=0;
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
