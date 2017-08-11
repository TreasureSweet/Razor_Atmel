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

pLedCommandType psNew;    // A struct to storage data temporarily
u16 u16Amount=0;          // The number of the commands in user list
u8 u8Gradient_Set=20;     // Gradient speed. User can change this in debug
u32 u32NumCheck_Ok=0;     // Check format of number input in debug, if right, this element is the number.

u8 au8MenuArray[]="\n\r\n\r\n\r************************************************\n\r*          Please choose a function            *\n\r*       1:       User define mode              *\n\r*       2:          Show lists                 *\n\r*    Press Space to return menu at any time    *\n\r************************************************\n\rChoose a function:";
// This array is the debug menu.

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */
extern u8 G_au8DebugScanfBuffer[];                     /* From main.c */
extern u8 G_u8DebugScanfCharCount;                     /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u32 u32Time_Counter;                            /* From main.c */
extern u8 u8Tera_Choose;                               /* From user_app1.c */
extern LedRateType aeGradient[];                       /* From user_app1.c */
extern pLedCommandType psUserHead;                     /* From user_app1.c */
extern pLedCommandType psUserTail;                     /* From user_app1.c */
extern pLedCommandType psDemoHead_1;                   /* From user_app1.c */
extern pLedCommandType psDemoHead_2;                   /* From user_app1.c */
extern bool bDebug_Set;                       /* From user_app1.c */
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
	bool bCheckOn=FALSE;
	u8 au8Array[256]=NULL;
	u8 *pu8Point=au8Array;
	u8 *pu8Tail=pu8Point;
	psNew=(pLedCommandType)malloc(sizeof(LedCommandType));
	
	/* Temporary data */
	LedNumberType eLedNumTem;
	u32 u32LedOnTem=0;
	u32 u32LedOffTem=0;
	bool bGradientTem=FALSE;
	
	/* Special keys */
	for(;*pu8Array!='\0';pu8Array++)
	{
		switch(*pu8Array)
		{
			case '\b':	// Backspace Key
				if(pu8Point!=au8Array)
				{
					if(pu8Point==pu8Tail)
					{
						pu8Tail--;
					}
					
					pu8Point--;
					*pu8Point=NULL;
				}
				break;
				
			case '\r':	// Enter Key
				break;
				
			case '\x1B':	// Find a key maybe a Direct Key
				if((*(pu8Array+1)=='\x5B'))
				{
					switch(*(pu8Array+2))
					{
						case '\x44':	// Left Key
							if(pu8Point!=au8Array)
							{
								pu8Point--;
							}
							
							pu8Array+=2;
							break;
							
						case '\x43':	// Right Key
							if(pu8Point!=pu8Tail)
							{
								pu8Point++;
							}
							
							pu8Array+=2;
							break;
							
						case '\x41':	// Up Key
						case '\x42':	// Down Key
							pu8Array+=2;
							break;
						
						default :
							break;
					}
				}
				break;
				
			default :	// Data Keys
				*pu8Point=*pu8Array;
				pu8Point++;
				pu8Tail++;
				break;
		}
	}
	
	pu8Tail=NULL; 
	pu8Point=au8Array;
	
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
	/* End */
}/* Finish */

/* NumCheck return 1:Format true and then can use u32NumCheck_Ok; renturn 0:Format false */
bool NumCheck(u8 *pu8NumArray)
{
	u8 au8Array[256]=NULL;
	u8 *pu8Point=au8Array;
	u8 *pu8Tail=pu8Point;
	
	/* Special keys */
	for(;*pu8NumArray!='\0';pu8NumArray++)
	{
		switch(*pu8NumArray)
		{
			case '\b':	// Backspace Key
				if(pu8Point!=au8Array)
				{
					if(pu8Point==pu8Tail)
					{
						pu8Tail--;
					}
					
					pu8Point--;
					*pu8Point=NULL;
				}
				break;
				
			case '\r':	// Enter Key
				break;
				
			case '\x1B':	// Find a key maybe a Direct Key
				if((*(pu8NumArray+1)=='\x5B'))
				{
					switch(*(pu8NumArray+2))
					{
						case '\x44':	// Left Key
							if(pu8Point!=au8Array)
							{
								pu8Point--;
							}
							
							pu8NumArray+=2;
							break;
							
						case '\x43':	// Right Key
							if(pu8Point!=pu8Tail)
							{
								pu8Point++;
							}
							
							pu8NumArray+=2;
							break;
							
						case '\x41':	// Up Key
						case '\x42':	// Down Key
							pu8NumArray+=2;
							break;
						
						default :
							break;
					}
				}
				break;
				
			default :	// Data Keys
				*pu8Point=*pu8NumArray;
				pu8Point++;
				pu8Tail++;
				break;
		}
	}
	
	pu8Tail=NULL; 
	pu8Point=au8Array;
	
	/* Check */
	for(u32NumCheck_Ok=0;*pu8Point!='\0';pu8Point++)
	{
		if(*pu8Point>='0'&&*pu8Point<='9')
		{
			u32NumCheck_Ok=10*u32NumCheck_Ok+(*pu8Point-48);
		}
		else
		{
			return FALSE;
		}
	}
	
	return TRUE;
	/* End */
}
/* Finish */

/* LedInput */
u8 LedInput()// If press space , return 1 . So use this to sign out this mode . Mode function : Add commands at the end of the list . Need to input one element.
{
	static u8 u8FunctionChoose=0;
	u8 au8Input[256]=NULL;   // An array to save commands inputed by keyboard
	
	if(u8FunctionChoose==0)
	{
		u8FunctionChoose=1;
		DebugPrintf("\n\r\n\r------------    Led Input Mode    ------------\n\rFunction : Increase a command at the end\n\rFormat: LedColor-LedOnTime-LedOffTime-Gradient\n\r");
		DebugPrintf("LedColor:White->w,W\n\r         PURPLE->p,P\n\r         Blue->b,B\n\r         Cyan->c,C\n\r         Green->g,G\n\r         Yellow->y,Y\n\r         Orange->o,O\n\r         Red->r,R\n\r");
		DebugPrintf("LedOntime and LedOffTime : 0~9999 (ms)\n\rGradient:y,Y or n,N\n\r         Y:use the effect of gradient\n\r         N:not use the effect of gradient\n\rFor example: r-500-4000-y  G-2000-3000-N\n\r------------          END         ------------");
	}
	
	if(u8FunctionChoose==1)
	{
		if(PrintLedCommand(psUserHead))
		{
			u8FunctionChoose=2;
			DebugPrintf("\n\rInput a command:");
		}
	}
	
	if(u8FunctionChoose==2)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
		{
			DebugScanf(au8Input);
			
			if(LedCheck(au8Input))
			{
				psNew->pNext=psUserTail->pNext;
				psUserTail->pNext=psNew;
				
				for(;psUserTail->pNext!=NULL;psUserTail=psUserTail->pNext);
				
				u8FunctionChoose=1;
				bDebug_Set=TRUE;
				u16Amount++;
			}
			else
			{
				DebugPrintf("\n\r\n\rFomart Error !\n\rInput a command:");
			}
		}
		
		if(G_u8DebugScanfCharCount>0)
		{
			if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]==' ')
			{
				DebugScanf(au8Input);
				u8FunctionChoose=0;
				return 1;
			}
		}
	}
	
	return 0;
}/* Finish */

/* LedInsert */
u8 LedInsert()// If press space , return 1 . So use this to sign out this mode . Mode function : Insert commands at the anywhere . Need to input 2 elements.
{
	static u8 u8FunctionChoose=0;
	u8 au8Input[256]=NULL;   // An array to save commands inputed by keyboard
	
	if(u8FunctionChoose==0)
	{
		u8FunctionChoose=1;
		DebugPrintf("\n\r\n\r------------    Led Insert Mode    ------------\n\rFunction : Insert a command after the specified command\n\rNum: 0 ~ Max number of the list\n\rCommand: LedColor-LedOnTime-LedOffTime-Gradient\n\r");
		DebugPrintf("LedColor:White->w,W\n\r         PURPLE->p,P\n\r         Blue->b,B\n\r         Cyan->c,C\n\r         Green->g,G\n\r         Yellow->y,Y\n\r         Orange->o,O\n\r         Red->r,R\n\r");
		DebugPrintf("LedOntime and LedOffTime : 0~9999 (ms)\n\rGradient:y,Y or n,N\n\r         Y:use the effect of gradient\n\r         N:not use the effect of gradient\n\rFor example: r-500-4000-y  G-2000-3000-N\n\r------------          END         ------------");
	}
	
	if(u8FunctionChoose==1)
	{
		if(PrintLedCommand(psUserHead))
		{
			u8FunctionChoose=2;
			DebugPrintf("\n\rInsert after which command ?\n\rInput a Num:");
		}
	}

	if(u8FunctionChoose==2)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
		{
			DebugScanf(au8Input);
			
			if(NumCheck(au8Input))
			{
				if(u32NumCheck_Ok<=u16Amount)
				{
					u8FunctionChoose=3;
					psUserTail=psUserHead;
					DebugPrintf("\n\rInput a command:");
					
					for(;u32NumCheck_Ok>0;u32NumCheck_Ok--)
					{
						psUserTail=psUserTail->pNext;
					}
				}
				else
				{
					DebugPrintf("\n\r\n\rInvalid Command!\n\rInsert after which command ?\n\rInput a Num:");
				}
			}
			else
			{
				DebugPrintf("\n\r\n\rInvalid Command!\n\rInsert after which command ?\n\rInput a Num:");
			}
		}
	}
	
	if(u8FunctionChoose==3)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
		{
			DebugScanf(au8Input);
			
			if(LedCheck(au8Input))
			{
				psNew->pNext=psUserTail->pNext;
				psUserTail->pNext=psNew;
				u8FunctionChoose=1;
				bDebug_Set=TRUE;
				
				for(;psUserTail->pNext!=NULL;psUserTail=psUserTail->pNext);
				u16Amount++;
			}
			else
			{
				u8FunctionChoose=2;
				DebugPrintf("\n\r\n\rInvalid Command!\n\rInsert after which command ?\n\rInput a Num:");
			}
		}
	}
	
	if(G_u8DebugScanfCharCount>0)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]==' ')
		{
			DebugScanf(au8Input);
			u8FunctionChoose=0;
			return 1;
		}
	}
	
	return 0;
}/* Finish */

/* LedAlter */
u8 LedAlter()
{
	static u8 u8FunctionChoose=0;
	u8 au8Input[256]=NULL;   // An array to save commands inputed by keyboard
	
	if(u8FunctionChoose==0)
	{
		u8FunctionChoose=1;
		DebugPrintf("\n\r\n\r------------    Led Alter Mode    ------------\n\rFunction : Alter the specified command\n\rNum: 1 ~ Max number of the list\n\rCommand: LedColor-LedOnTime-LedOffTime-Gradient\n\r");
		DebugPrintf("LedColor:White->w,W\n\r         PURPLE->p,P\n\r         Blue->b,B\n\r         Cyan->c,C\n\r         Green->g,G\n\r         Yellow->y,Y\n\r         Orange->o,O\n\r         Red->r,R\n\r");
		DebugPrintf("LedOntime and LedOffTime : 0~9999 (ms)\n\rGradient:y,Y or n,N\n\r         Y:use the effect of gradient\n\r         N:not use the effect of gradient\n\rFor example: r-500-4000-y  G-2000-3000-N\n\r------------          END         ------------");
	}
	
	if(u8FunctionChoose==1)
	{
		if(PrintLedCommand(psUserHead))
		{
			u8FunctionChoose=2;
			DebugPrintf("\n\rAlter which command ?\n\rInput a Num:");
		}
	}
	
	if(u8FunctionChoose==2)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
		{
			DebugScanf(au8Input);
			
			if(NumCheck(au8Input))
			{
				if(u32NumCheck_Ok>0&&u32NumCheck_Ok<=u16Amount)
				{
					u8FunctionChoose=3;
					psUserTail=psUserHead;
					DebugPrintf("\n\rInput a command:");

					for(;u32NumCheck_Ok>0;u32NumCheck_Ok--)
					{
						psUserTail=psUserTail->pNext;
					}
				}
				else
				{
					DebugPrintf("\n\r\n\rInvalid Command!\n\rAlter which command ?\n\rInput a Num:");
				}
			}
			else
			{
				DebugPrintf("\n\r\n\rInvalid Command!\n\rAlter which command ?\n\rInput a Num:");
			}
		}
	}
	
	if(u8FunctionChoose==3)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
		{
			DebugScanf(au8Input);
			
			if(LedCheck(au8Input))
			{
				psUserTail->eLedNum=psNew->eLedNum;
				psUserTail->u32LedOn=psNew->u32LedOn;
				psUserTail->u32LedOff=psNew->u32LedOff;
				psUserTail->bGradient=psNew->bGradient;
				u8FunctionChoose=1;
				bDebug_Set=TRUE;
				
				for(;psUserTail->pNext!=NULL;psUserTail=psUserTail->pNext);
			}
			else
			{
				u8FunctionChoose=2;
				DebugPrintf("\n\r\n\rInvalid Command!\n\rAlter which command ?\n\rInput a Num:");
			}
		}
	}
	
	if(G_u8DebugScanfCharCount>0)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]==' ')
		{
			DebugScanf(au8Input);
			u8FunctionChoose=0;
			return 1;
		}
	}
	
	return 0;
}/* Finish */

/* LedDelete */
u8 LedDelete()
{
	static u8 u8FunctionChoose=0;
	pLedCommandType psPoint=NULL;
	u8 au8Input[256]=NULL;   // An array to save commands inputed by keyboard
	
	if(u8FunctionChoose==0)
	{
		u8FunctionChoose=1;
		DebugPrintf("\n\r\n\r------------    Led Delete Mode    -----------\n\rFunction : Delete the specified command\n\rNum: 1 ~ Max number of the list\n\r------------          END         ------------");
	}
	
	if(u8FunctionChoose==1)
	{
		if(PrintLedCommand(psUserHead))
		{
			u8FunctionChoose=2;
			DebugPrintf("\n\rDelete which command ?\n\rInput a Num:");
		}
	}
	
	if(u8FunctionChoose==2)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
		{
			DebugScanf(au8Input);
			
			if(NumCheck(au8Input))
			{
				if(u32NumCheck_Ok>0&&u32NumCheck_Ok<=u16Amount)
				{
					u8FunctionChoose=3;
					psUserTail=psUserHead;
					DebugPrintf("\n\rInput a command:");
					
					for(u32NumCheck_Ok--;u32NumCheck_Ok>0;u32NumCheck_Ok--)
					{
						psUserTail=psUserTail->pNext;
					}
				}
				else
				{
					DebugPrintf("\n\r\n\rInvalid Command!\n\rDelete which command ?\n\rInput a Num:");
				}
			}
			else
			{
				DebugPrintf("\n\r\n\rInvalid Command!\n\rDelete which command ?\n\rInput a Num:");
			}
		}
	}
	
	if(u8FunctionChoose==3)
	{
		u8FunctionChoose=1;
		bDebug_Set=TRUE;
		psPoint=psUserTail->pNext;
		psUserTail->pNext=psPoint->pNext;
		psPoint=NULL;
		
		for(;psUserTail->pNext!=NULL;psUserTail=psUserTail->pNext);
		u16Amount--;
	}
	
	if(G_u8DebugScanfCharCount>0)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]==' ')
		{
			DebugScanf(au8Input);
			u8FunctionChoose=0;
			return 1;
		}
	}
	
	return 0;
}/* Finish */

/* LedGradient */
u8 LedGradient()
{
	static u8 u8FunctionChoose=0;
	u8 au8Input[256]=NULL;   // An array to save commands inputed by keyboard
	
	if(u8FunctionChoose==0)
	{
		u8FunctionChoose=1;
		DebugPrintf("\n\r\n\r----------    Led Set Gradient Mode   ---------\n\rFunction : Set the speed of led gradient\n\rGradient Speed: 10 ~ 200\n\rWarning : It needs nearly 20*(Gradient Speed)(ms)\n\rafter the On time or the Off time,\n\ror it will be not so effective\n\r------------          END         ------------");
	}
	
	if(u8FunctionChoose==1)
	{
		u8FunctionChoose=2;
		DebugPrintf("\n\r\n\rGradient Speed: ");
		DebugPrintNumber(u8Gradient_Set);
		DebugPrintf("ms\n\rNew Gradient Speed:");
	}
	
	if(u8FunctionChoose==2)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]=='\r')
		{
			DebugScanf(au8Input);
			
			if(NumCheck(au8Input))
			{
				if(u32NumCheck_Ok>=10&&u32NumCheck_Ok<=200)
				{
					u8FunctionChoose=1;
					bDebug_Set=TRUE;
					u8Gradient_Set=u32NumCheck_Ok;
					u32NumCheck_Ok=0;
				}
				else
				{
					DebugPrintf("\n\r\n\rInvalid Command!\n\rNew Gradient Speed:");
				}
			}
			else
			{
				DebugPrintf("\n\r\n\rInvalid Command!\n\rNew Gradient Speed:");
			}
		}
	}
	
	if(G_u8DebugScanfCharCount>0)
	{
		if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]==' ')
		{
			DebugScanf(au8Input);
			u8FunctionChoose=0;
			return 1;
		}
	}
	
	return 0;
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
	/* Debug Menu */
	DebugPrintf(au8MenuArray);
	
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
	/*****************************    Variables    ********************************/
	/* In debug */
	static u8 au8Scanf[2];     // An element to choose function
	/*********************************   end   ************************************/

	/*******************************    Debug    **********************************/
	/*--------------------      Menu ( Function Choose )      -------------------------*/
	if(u8Tera_Choose==0)
	{
		if(DebugScanf(au8Scanf))
		{
			switch(au8Scanf[0])
			{
				case '1':
					u8Tera_Choose=1;	// Goto User define mode
					DebugPrintf("\n\r\n\r============   User define mode   ============\n\r\n\r1:              Led Input Mode\n\r2:              Led Insert Mode\n\r3:              Led Alter Mode\n\r4:              Led Delete Mode\n\r5:              Led Set Gradient Mode\n\r\n\r=============        END        ==============\n\rChoose a mode:");
					break;
					
				case '2':
					u8Tera_Choose=2;	// Goto Show lists mode
					DebugPrintf("\n\r\n\r===============   Show lists   ===============\n\r\n\r1:                Demo_List_1\n\r2:                Demo_List_2\n\r3:                User_List\n\r\n\r=============        END        ==============\n\rChoose a list:");
					break;
					
				default :               // Invalid command
					DebugPrintf("\n\r\n\rInvalid Command!(You should input 1 or 2)\n\rChoose a function:");
					break;
			}
			
			au8Scanf[0]=NULL;
		}
	}
	/*--------------------------------    end    ---------------------------------*/
	
	/*-------------------------   User define mode    ----------------------------*/
	if(u8Tera_Choose==1)
	{
		if(DebugScanf(au8Scanf))
		{
			switch(au8Scanf[0])
			{
				case '1':
					u8Tera_Choose=3;	// Goto Input Mode
					break;
					
				case '2':
					u8Tera_Choose=4;	// Goto Insert Mode
					break;
					
				case '3':
					u8Tera_Choose=5;	// Goto Alter Mode
					break;
					
				case '4':
					u8Tera_Choose=6;	// Goto Delete Mode
					break;
					
				case '5':
					u8Tera_Choose=7;	// Goto Set Gradient Mode
					break;
					
				case ' ':               // Return to menu
					u8Tera_Choose=0;
					DebugPrintf(au8MenuArray);
					break;
					
				default :               // Invalid command
					DebugPrintf("\n\r\n\rInvalid command ! (You should input 1,2,3 or 4)\n\rChoose a Mode:");
					break;
			}
			
			au8Scanf[0]=NULL;
		}
	}
	
	if(u8Tera_Choose==3)// Led Input Mode
	{
		if(LedInput())
		{
			u8Tera_Choose=0;
			DebugPrintf(au8MenuArray);
		}
	}
	
	if(u8Tera_Choose==4)// Led Insert Mode
	{
		if(LedInsert())
		{
			u8Tera_Choose=0;
			DebugPrintf(au8MenuArray);
		}
	}
	
	if(u8Tera_Choose==5)// Led Alter Mode
	{
		if(LedAlter())
		{
			u8Tera_Choose=0;
			DebugPrintf(au8MenuArray);
		}
	}
	
	if(u8Tera_Choose==6)// Led Delete Mode
	{
		if(LedDelete())
		{
			u8Tera_Choose=0;
			DebugPrintf(au8MenuArray);
		}
	}
	
	if(u8Tera_Choose==7)// Led Set Gradient
	{	
		if(LedGradient())
		{
			u8Tera_Choose=0;
			DebugPrintf(au8MenuArray);
		}
	}
	/*--------------------------------    end    ---------------------------------*/
	
	/*--------------------------   Show lists mode    ----------------------------*/
	if(u8Tera_Choose==2)
	{
		if(DebugScanf(au8Scanf))
		{
			switch(au8Scanf[0])
			{
				case '1':                // Goto Show Demo List 1
					u8Tera_Choose=8;
					break;
					
				case '2':                // Goto Show Demo List 2
					u8Tera_Choose=9;
					break;
					
				case '3':                // Goto Show User List
					u8Tera_Choose=10;
					break;
					
				case ' ':                // Return to menu
					u8Tera_Choose=0;
					DebugPrintf(au8MenuArray);
					break;
					
				default :                // Invalid command
					DebugPrintf("\n\r\n\rInvalid command ! (You should input 1,2 or 3)\n\rChoose a List:");
					break;
			}
			
			au8Scanf[0]=NULL;
		}
	}
	
	if(u8Tera_Choose==8)// Show Demo List 1
	{
		if(PrintLedCommand(psDemoHead_1))
		{
			DebugPrintf("\n\rChoose a List:");
			u8Tera_Choose=2;
		}
	}
	
	if(u8Tera_Choose==9)// Show Demo List 2
	{
		if(PrintLedCommand(psDemoHead_2))
		{
			DebugPrintf("\n\rChoose a List:");
			u8Tera_Choose=2;
		}
	}
	
	if(u8Tera_Choose==10)// Show User List
	{
		if(PrintLedCommand(psUserHead))
		{
			DebugPrintf("\n\rChoose a List:");
			u8Tera_Choose=2;
		}
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
