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
pLedCommandType psNew;
pLedCommandType psDemoPrint;
u32 u32Count=0;
/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u32 u32Time_Counter;                            /* From main.c */
extern u8 u8Gradient_Set;                              /* From main.c */
extern u8 u8Tera_Choose;                               /* From user_app1 */
extern LedRateType aeGradient[];                       /* From user_app1 */
extern pLedCommandType psUserHead;                     /* From user_app1 */
extern pLedCommandType psUserTail;                     /* From user_app1 */
extern LedCommandType asDemoArray_1[];                 /* From user_app1 */
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

/* LedCheck return 1:Format true and then can use psNew; renturn 0:Format false */
u8 LedCheck(u8 *pu8Array)
{
	u8 *pu8Point=pu8Array;
	bool bCheckOn=FALSE;
	psNew=(pLedCommandType)malloc(sizeof(LedCommandType));
	
	/* Temporary data */
	LedNumberType eLedNumTem;
	u32 u32LedOnTem=0;
	u32 u32LedOffTem=0;
	bool bGradientTem=FALSE;
	
	/* Check */
	if(*(pu8Point+1)=='-')
	{
		switch(*pu8Point)
		{
			case 'w':
			case 'W':
				eLedNumTem=WHITE;
				bCheckOn=TRUE;
				break;
				
			case 'p':
			case 'P':
				eLedNumTem=PURPLE;
				bCheckOn=TRUE;
				break;
				
			case 'b':
			case 'B':
				eLedNumTem=BLUE;
				bCheckOn=TRUE;
				break;
				
			case 'c':
			case 'C':
				eLedNumTem=CYAN;
				bCheckOn=TRUE;
				break;
				
			case 'g':
			case 'G':
				eLedNumTem=GREEN;
				bCheckOn=TRUE;
				break;
				
			case 'y':
			case 'Y':
				eLedNumTem=YELLOW;
				bCheckOn=TRUE;
				break;
				
			case 'o':
			case 'O':
				eLedNumTem=ORANGE;
				bCheckOn=TRUE;
				break;
				
			case 'r':
			case 'R':
				eLedNumTem=RED;
				bCheckOn=TRUE;
				break;
		}
	}
	
	if(bCheckOn)
	{
		bCheckOn=FALSE;
		
		for(pu8Point+=2;;pu8Point++)
		{
			if(*pu8Point>='0'&&*pu8Point<='9')
			{
				u32LedOnTem=10*u32LedOnTem+(*pu8Point-48);
				bCheckOn=TRUE;
			}		
			
			if(!bCheckOn)
			{
				if(*pu8Point=='-'&&*(pu8Point-1)!='-')
				{
					bCheckOn=TRUE;
					break;
				}
				else
				{
					break;
				}
			}
			
			bCheckOn=FALSE;	
		}
	}
	
	if(bCheckOn)
	{
		bCheckOn=FALSE;
		
		for(pu8Point++;;pu8Point++)
		{
			if(*pu8Point>='0'&&*pu8Point<='9')
			{
				u32LedOffTem=10*u32LedOffTem+(*pu8Point-48);
				bCheckOn=TRUE;
			}		
			
			if(!bCheckOn)
			{
				if(*pu8Point=='-'&&*(pu8Point-1)!='-')
				{
					bCheckOn=TRUE;
					break;
				}
				else
				{
					break;
				}
			}
			
			bCheckOn=FALSE;	
		}
	}
	
	if(bCheckOn)
	{
		bCheckOn=FALSE;
		
		if(*(pu8Point+2)=='\0')
		{
			switch(*(++pu8Point))
			{
				case 'y':
				case 'Y':
					bGradientTem=TRUE;
					bCheckOn=TRUE;
					break;
					
				case 'n':
				case 'N':
					bGradientTem=FALSE;
					bCheckOn=TRUE;
					break;
					
				default :
					break;
			}
		}
	}
	
	if(bCheckOn)
	{
		psNew->eLedNum=eLedNumTem;
		psNew->u32LedOn=u32LedOnTem;
		psNew->u32LedOff=u32LedOffTem;
		psNew->bGradient=bGradientTem;
		psNew->pNext=NULL;
		psNew->peGradient=aeGradient;
		psNew->bOn=FALSE;
		psNew->bUp=FALSE;
		psNew->u8Gradient_Time=u8Gradient_Set;
		return 1;
	}
	else
	{
		return 0;
	}
}/* Finish */

/* LedInput */
void LedInput()
{
	psNew->pNext=psUserTail->pNext;
	psUserTail=psNew;
}/* Finish */

/* LedAlter */
void LedAlter()
{
	psUserTail->eLedNum=psNew->eLedNum;
	psUserTail->u32LedOn=psNew->u32LedOn;
	psUserTail->u32LedOff=psNew->u32LedOff;
	psUserTail->bGradient=psNew->bGradient;
}/* Finish */
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
	psDemoPrint=(pLedCommandType)malloc(sizeof(LedCommandType));
	psDemoPrint->pNext=asDemoArray_1;
	
	/* Debug initialize */
	DebugPrintf("************************************************\n\r");
	DebugPrintf("*          Please choose a function            *\n\r");
	DebugPrintf("*       1:       User define mode              *\n\r");
	DebugPrintf("*       2:          Show lists                 *\n\r");
	DebugPrintf("************************************************\n\r");
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
	/*******************************    Variables    ******************************/
	/* In debug */
	static u8 au8Scanf[2];
	/***********************************   end   **********************************/

	/*********************************    Debug    ********************************/
	/*-------------------------   Initial interface   ----------------------------*/
	if(u8Tera_Choose==0)
	{
		DebugScanf(au8Scanf);
		
		if(au8Scanf[0]=='1')
		{
			u8Tera_Choose=1;
			DebugLineFeed();
			DebugPrintf("\n\r=========   User define mode   =========\n\r");
			DebugPrintf("|  LED   | TimeON | TimeOFF | Gradient |\n\r");
		}
		
		if(au8Scanf[0]=='2')
		{
			u8Tera_Choose=2;
			DebugLineFeed();
			DebugPrintf("\n\r============   Show lists   ============\n\r");
			DebugPrintf("|  LED   | TimeON | TimeOFF | Gradient |\n\r");
		}
	}
	/*--------------------------------    end    ---------------------------------*/
	
	/*-------------------------   Function Choose   ------------------------------*/
	if(u8Tera_Choose==1)
	{
		PrintLedCommand(psDemoPrint);
		u8Tera_Choose=0;
		au8Scanf[0]='0';
	}
	
	if(u8Tera_Choose==2)
	{
	}
	/*--------------------------------    end    ---------------------------------*/
	/**********************************    END    *********************************/
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
