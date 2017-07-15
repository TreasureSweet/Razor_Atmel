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

extern u8 G_au8DebugScanfBuffer[];					 /* From debug.c */
extern u8 G_u8DebugScanfCharCount;					 /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp2_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp2_StateMachine;            /* The state machine function pointer */
//static u32 UserApp2_u32Timeout;                      /* Timeout counter used across states */
static u8 au8InputBuffer[USER_INPUT_BUFFER_SIZE];    /* Char buffer */
static u8 au8NameArray[]=NAME;

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
	static u8 u8Counter=0;
	static u32 u32CountSize=0;
	static u32 u32CountSize_1=0;
	static u8 au8CountSize[sizeof(u32)];
	static bool bJudge=FALSE;
	static bool bCheck=FALSE;
	
	/*Do Judge when the first alphabet is right*/
	if(!bJudge)
	{
		if(G_au8DebugScanfBuffer[0]==au8NameArray[0])
		{
			bJudge=TRUE;
		}
		else
		{
			if(G_u8DebugScanfCharCount!=0)
			{
				DebugScanf(au8InputBuffer);//Clear
			}
		}
	}
	
	if(bJudge)
	{
		if(G_au8DebugScanfBuffer[u8Counter+1]!='\0')
		{
			bCheck=TRUE;
		}
	}
	
	/*Check when keyboard press a key*/
	if(bCheck)
	{
		bCheck=FALSE;
		
		if(G_au8DebugScanfBuffer[++u8Counter]!=au8NameArray[u8Counter])
		{
			u8Counter=0;
			DebugScanf(au8InputBuffer);//Clear
			bJudge=FALSE;
		}
	}
	
	/*Print when the name is right*/
	if(u8Counter==sizeof(au8NameArray)-2)
	{
		u32CountSize++;
		DebugScanf(au8InputBuffer);//Clear
		
		for(u32CountSize_1=u32CountSize*10,u8Counter=0;u32CountSize_1>0;u32CountSize_1/=10)
		{
			u8Counter++;
		}
		
		for(u8Counter++;u8Counter>=1;u8Counter--)
		{
			au8CountSize[u8Counter-1]='*';
		}
		
		bJudge=FALSE;
		DebugLineFeed();
		DebugLineFeed();
		DebugPrintf(au8CountSize);
		DebugPrintf("\n\r*");
		DebugPrintNumber(u32CountSize);
		DebugPrintf("*\n\r");
		DebugPrintf(au8CountSize);
		DebugLineFeed();
		DebugLineFeed();
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
