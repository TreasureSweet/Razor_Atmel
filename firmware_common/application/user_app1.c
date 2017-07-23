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

u8 u8Button_Choose=0;
u8 u8Tera_Choose=0;
u8 u8LengthCount_1;
u8 u8LengthCount_2;
bool bPause=FALSE;
bool bButtonPressed=FALSE;
LedRateType aeGradient[]={LED_PWM_0,LED_PWM_5,LED_PWM_10,LED_PWM_15,LED_PWM_20, 
							LED_PWM_25,LED_PWM_30,LED_PWM_35,LED_PWM_40,LED_PWM_45, 
							LED_PWM_50,LED_PWM_55,LED_PWM_60,LED_PWM_65,LED_PWM_70, 
							LED_PWM_75, LED_PWM_80,LED_PWM_85, LED_PWM_90,LED_PWM_95, 
							LED_PWM_100};
LedCommandType asDemoArray_1[]={{WHITE,0,1000,TRUE},
								{PURPLE,500,1500,TRUE},
								{BLUE,1000,2000,TRUE},
								{CYAN,1500,2500,TRUE},
								{GREEN,2000,3000,TRUE},
								{YELLOW,2500,3500,TRUE},
								{ORANGE,3000,4000,TRUE},
								{RED,3500,4500,TRUE},
								{RED,4500,5500,TRUE},
								{ORANGE,5000,6000,TRUE},
								{YELLOW,5500,6500,TRUE},
								{GREEN,6000,7000,TRUE},
								{CYAN,6500,7500,TRUE},
								{BLUE,7000,8000,TRUE},
								{PURPLE,7500,8500,TRUE},
								{WHITE,8000,9000,TRUE},
								};
LedCommandType asDemoArray_2[]={{CYAN,0,2000,TRUE},
								{GREEN,0,2000,TRUE},
								{BLUE,500,2500,TRUE},
								{YELLOW,500,2500,TRUE},
								{PURPLE,1000,3000,TRUE},
								{ORANGE,1000,3000,TRUE},
								{WHITE,1500,3500,TRUE},
								{RED,1500,3500,TRUE},
								{WHITE,5000,5500,FALSE},
								{PURPLE,5500,6000,FALSE},
								{BLUE,5000,5500,FALSE},
								{CYAN,5500,6000,FALSE},
								{GREEN,5000,5500,FALSE},
								{YELLOW,5500,6000,FALSE},
								{ORANGE,5000,5500,FALSE},
								{RED,5500,6000,FALSE},
								{WHITE,6000,6500,FALSE},
								{PURPLE,6500,7000,FALSE},
								{BLUE,6000,6500,FALSE},
								{CYAN,6500,7000,FALSE},
								{GREEN,6000,6500,FALSE},
								{YELLOW,6500,7000,FALSE},
								{ORANGE,6000,6500,FALSE},
								{RED,6500,7000,FALSE},
								{WHITE,7000,7500,FALSE},
								{PURPLE,7500,8000,FALSE},
								{BLUE,7000,7500,FALSE},
								{CYAN,7500,8000,FALSE},
								{GREEN,7000,7500,FALSE},
								{YELLOW,7500,8000,FALSE},
								{ORANGE,7000,7500,FALSE},
								{RED,7500,8000,FALSE}
								};
/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u32 u32Time_Counter;                            /* From main.c */
extern u8 u8Gradient_Set;                              /* From main.c */
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 au8LedAddr[]={1,3,6,8,11,13,16,18};		/* The address to change led state appearence */

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/
void RunLedCommand(LedCommandType asOutputArray[])
{
	static LedCommandType *pPoint;
	
	for(pPoint=asOutputArray;;pPoint=pPoint->pNext)
	{
		if(u32Time_Counter==pPoint->u32LedOn)
		{
			pPoint->bOn=TRUE;
			pPoint->bUp=TRUE;
		}
		
		if(u32Time_Counter==pPoint->u32LedOff)
		{
			pPoint->bOn=TRUE;
			pPoint->bUp=FALSE;
		}
		
		if(pPoint->pNext==NULL)
		{
			break;
		}
	}
	
	for(pPoint=asOutputArray;;pPoint=pPoint->pNext)
	{
		if(pPoint->bOn)
		{
			if(pPoint->bGradient)//Gradient
			{
				if(pPoint->u8Gradient_Time++==u8Gradient_Set)
				{
					pPoint->u8Gradient_Time=0;
					 
					if(pPoint->bUp)
					{
						LedPWM(pPoint->eLedNum,*(++pPoint->pGradient));
					}
					else
					{
						LedPWM(pPoint->eLedNum,*(--pPoint->pGradient));
					}
					
					if(*(pPoint->pGradient)==LED_PWM_0||*(pPoint->pGradient)==LED_PWM_100)
					{
						pPoint->bOn=FALSE;
						pPoint->u8Gradient_Time=u8Gradient_Set;
					}
				}
			}

			if(!pPoint->bGradient)//No gradient
			{
				pPoint->bOn=FALSE;
				
				if(pPoint->bUp)
				{
					LedPWM(pPoint->eLedNum,LED_PWM_100);
				}
				else
				{
					LedPWM(pPoint->eLedNum,LED_PWM_0);
				}
			}
		}
		
		if(pPoint->pNext==NULL)
		{
			break;
		}
	}
}

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
	/****************************    Initializations    ***************************/
	/* Led */
	LedOff(WHITE);
	LedOff(PURPLE);
	LedOff(BLUE);
	LedOff(CYAN);
	LedOff(GREEN);
	LedOff(YELLOW);
	LedOff(ORANGE);
	LedOff(RED);
	
	/* Buzzer */
	PWMAudioOff(BUZZER1);
	PWMAudioSetFrequency(BUZZER1,1536);
	
	/* Lcd */
	LCDCommand(LCD_CLEAR_CMD);
	LCDMessage(LINE1_START_ADDR+5,"Having Fun !");
	LCDMessage(LINE2_START_ADDR,"DMO");
	LCDMessage(LINE2_START_ADDR+6,"USR");
	LCDMessage(LINE2_START_ADDR+18,"||");
	
	/* Debug */
	
	/* DemoArrary */
	u8LengthCount_1=sizeof(asDemoArray_1)/sizeof(LedCommandType);
	u8LengthCount_2=sizeof(asDemoArray_2)/sizeof(LedCommandType);
	
	for(u8 i=1;i<u8LengthCount_1;i++)
	{
		asDemoArray_1[i-1].pNext=&asDemoArray_1[i];
	}
	
	for(u8 i=0;i<u8LengthCount_1;i++)
	{
		asDemoArray_1[i].pGradient=aeGradient;
		asDemoArray_1[i].bOn=FALSE;
		asDemoArray_1[i].bUp=FALSE;
		asDemoArray_1[i].u8Gradient_Time=u8Gradient_Set;
	}
	
	for(u8 i=1;i<u8LengthCount_2;i++)
	{
		asDemoArray_2[i-1].pNext=&asDemoArray_2[i];
	}
	
	for(u8 i=0;i<u8LengthCount_2;i++)
	{
		asDemoArray_2[i].pGradient=aeGradient;
		asDemoArray_2[i].bOn=FALSE;
		asDemoArray_2[i].bUp=FALSE;
		asDemoArray_2[i].u8Gradient_Time=u8Gradient_Set;
	}
	/**********************************    END    *********************************/
	
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
	/*******************************    Variables    ******************************/
	/* In demo lists */
	static u8 u8DemoChoose=0;
	/***********************************   end   **********************************/
	
	/********************************    Buttons    *******************************/
	if(WasButtonPressed(BUTTON0))	// Different in different user_app
	{
		ButtonAcknowledge(BUTTON0);
		bButtonPressed=TRUE;
		u8Button_Choose=1;
		//PWMAudioOn(BUZZER1);
	}
	
	if(WasButtonPressed(BUTTON1))	// Different in different user_app
	{
		ButtonAcknowledge(BUTTON1);
		bButtonPressed=TRUE;
		u8Button_Choose=2;
		//PWMAudioOn(BUZZER1);
	}
	
	if(WasButtonPressed(BUTTON2))	// Different in different user_app
	{
		ButtonAcknowledge(BUTTON2);
		bButtonPressed=TRUE;
		u8Button_Choose=3;
		//PWMAudioOn(BUZZER1);
	}
	
	if(WasButtonPressed(BUTTON3))	// A special button used to pause and go (not fit user_app3)
	{
		ButtonAcknowledge(BUTTON3);
		bButtonPressed=TRUE;
		u8Button_Choose=4;
		bPause=!bPause;
		//PWMAudioOn(BUZZER1);
	}
	/**********************************    END    *********************************/
	
	/*********************************    Debug    ********************************/
	
	/**********************************    END    *********************************/
	
	/*********************************    Demo    ********************************/
	if(!bPause)
	{
		if(u8Button_Choose==1)
		{
			u8Button_Choose=0;
			u32Time_Counter=0;
			LedOff(WHITE);
			LedOff(PURPLE);
			LedOff(BLUE);
			LedOff(CYAN);
			LedOff(GREEN);
			LedOff(YELLOW);
			LedOff(ORANGE);
			LedOff(RED);
		
			for(u8 i=0;i<u8LengthCount_1;i++)
			{
				asDemoArray_1[i].pGradient=aeGradient;
				asDemoArray_1[i].bOn=FALSE;
				asDemoArray_1[i].bUp=FALSE;
				asDemoArray_1[i].u8Gradient_Time=u8Gradient_Set;
			}
			
				for(u8 i=0;i<u8LengthCount_2;i++)
			{
				asDemoArray_2[i].pGradient=aeGradient;
				asDemoArray_2[i].bOn=FALSE;
				asDemoArray_2[i].bUp=FALSE;
				asDemoArray_2[i].u8Gradient_Time=u8Gradient_Set;
			}
				
			if(++u8DemoChoose==3)
			{
				u8DemoChoose=0;
			}
			
			if(u8DemoChoose==0)
			{
				LCDClearChars(LINE2_START_ADDR+3,2);
			}
			
			if(u8DemoChoose==1)
			{
				LCDMessage(LINE2_START_ADDR+3,">1");
			}
			
			if(u8DemoChoose==2)
			{
				LCDMessage(LINE2_START_ADDR+3,">2");
			}
		}
		
		if(u8DemoChoose==1)
		{
			RunLedCommand(asDemoArray_1);
		}
		
		if(u8DemoChoose==2)
		{
			RunLedCommand(asDemoArray_2);
		}
	}
	/**********************************    END    *********************************/
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
