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

u8 u8Button_Choose=0;       // Change when button pressed
u8 u8Tera_Choose=0;         // Change when debug choose a function
u8 u8LengthCount_1;         // Length of Demo List 1
u8 u8LengthCount_2;         // Length of Demo List 2
u16 u16LcdOnTime=0;         // Used to turn lcd off after timeset in user_app3.c ( Time )
bool bLcdOn=TRUE;           // Used to turn lcd off after timeset in user_app3.c ( Check )
bool bPause=FALSE;          // Button3 press to change it and then pause the functions
bool bButtonPressed=FALSE;  // Used to turn Buzzer off after timeset in user_app3.c
bool bEmpty=FALSE;          // Used to show warning on lcd when button1 is pressed but demo list is empty
bool bUserChoose=FALSE;     // Run demo list or not
bool bDebug_Set=FALSE;      // Every time debug input a command to user list, the bool will be ture and then initialize the runcommand app
pLedCommandType psDemoHead_1;         // The head of Demo List 1
pLedCommandType psDemoHead_2;         // The head of Demo List 2
pLedCommandType psUserHead;           // The head of User List
pLedCommandType psUserTail;           // The Tail of User List

LedRateType aeGradient[]={LED_PWM_0,LED_PWM_5,LED_PWM_10,LED_PWM_15,LED_PWM_20, 
							LED_PWM_25,LED_PWM_30,LED_PWM_35,LED_PWM_40,LED_PWM_45, 
							LED_PWM_50,LED_PWM_55,LED_PWM_60,LED_PWM_65,LED_PWM_70, 
							LED_PWM_75, LED_PWM_80,LED_PWM_85, LED_PWM_90,LED_PWM_95, 
							LED_PWM_100};    // This array is used to add gradient to led

LedCommandType asDemoArray_1[]={{WHITE,0,1900,TRUE},
								{PURPLE,400,2300,TRUE},
								{BLUE,800,2700,TRUE},
								{CYAN,1200,3100,TRUE},
								{GREEN,1600,3500,TRUE},
								{YELLOW,2000,3900,TRUE},
								{ORANGE,2400,4300,TRUE},
								{RED,2800,4700,TRUE},
								{RED,5000,6900,TRUE},
								{ORANGE,5400,7300,TRUE},
								{YELLOW,5800,7700,TRUE},
								{GREEN,6200,8100,TRUE},
								{CYAN,6600,8500,TRUE},
								{BLUE,7000,8900,TRUE},
								{PURPLE,7400,9300,TRUE},
								{WHITE,7800,9700,TRUE}
								};                       // Demo List 1

LedCommandType asDemoArray_2[]={{CYAN,0,1800,TRUE},
								{GREEN,0,1800,TRUE},
								{BLUE,300,2100,TRUE},
								{YELLOW,300,2100,TRUE},
								{PURPLE,600,2400,TRUE},
								{ORANGE,600,2400,TRUE},
								{WHITE,900,2700,TRUE},
								{RED,900,2700,TRUE},
								{CYAN,3335,5135,TRUE},
								{GREEN,3335,5135,TRUE},
								{BLUE,3635,5435,TRUE},
								{YELLOW,3635,5435,TRUE},
								{PURPLE,3935,5735,TRUE},
								{ORANGE,3935,5735,TRUE},
								{WHITE,4235,6035,TRUE},
								{RED,4235,6035,TRUE},
								{CYAN,6670,8470,TRUE},
								{GREEN,6670,8470,TRUE},
								{BLUE,6970,8770,TRUE},
								{YELLOW,6970,8770,TRUE},
								{PURPLE,7270,9070,TRUE},
								{ORANGE,7270,9070,TRUE},
								{WHITE,7570,9370,TRUE},
								{RED,7570,9370,TRUE}
								};                       // Demo List 2

/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u32 u32Time_Counter;                            /* From main.c */
extern u8 u8Gradient_Set;                              /* From user_app2.c */
extern u8 G_au8DebugScanfBuffer[];                     /* From main.c */

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

/* Run led commands ( Format in (): The head of a list, gradient time) */
void RunLedCommand(LedCommandType *psOutput,u8 u8Time)
{
	static LedCommandType *psPoint;

	if(psOutput->pNext!=NULL)
	{
		for(psPoint=psOutput->pNext;;psPoint=psPoint->pNext)
		{
			if(u32Time_Counter==psPoint->u32LedOn)
			{
				psPoint->bOn=TRUE;
				psPoint->bUp=TRUE;
			}
			
			if(u32Time_Counter==psPoint->u32LedOff)
			{
				psPoint->bOn=TRUE;
				psPoint->bUp=FALSE;
			}
			
			if(psPoint->pNext==NULL)
			{
				break;
			}
		}
		
		for(psPoint=psOutput->pNext;;psPoint=psPoint->pNext)
		{
			if(psPoint->bOn)
			{
				if(psPoint->bGradient)	// Gradient
				{
					if(psPoint->u8Gradient_Time++==u8Time)
					{
						psPoint->u8Gradient_Time=0;
						 
						if(psPoint->bUp)
						{
							LedPWM(psPoint->eLedNum,*(++psPoint->peGradient));
						}
						else
						{
							LedPWM(psPoint->eLedNum,*(--psPoint->peGradient));
						}
						
						if(*(psPoint->peGradient)==LED_PWM_0||*(psPoint->peGradient)==LED_PWM_100)
						{
							psPoint->bOn=FALSE;
							psPoint->u8Gradient_Time=u8Time;
						}
					}
				}

				if(!psPoint->bGradient)	// No gradient
				{
					psPoint->bOn=FALSE;
					
					if(psPoint->bUp)
					{
						LedPWM(psPoint->eLedNum,LED_PWM_100);
					}
					else
					{
						LedPWM(psPoint->eLedNum,LED_PWM_0);
					}
				}
			}
			
			if(psPoint->pNext==NULL)
			{
				break;
			}
		}
	}
	else	// The command list is empty
	{
		if(!bEmpty)
		{
			LCDClearChars(LINE1_START_ADDR,20);
			LCDMessage(LINE1_START_ADDR+3,"Command empty !");
		}
		
		bEmpty=TRUE;
	}
}/* Finish */

/* LedCommand initialize ( Format in (): The head of a list, gradient time) */
void LedCommandInitialize(LedCommandType *psOutput,u8 u8Time)
{
	static LedCommandType *psPoint;
	u32Time_Counter=0;
	LedOff(WHITE);
	LedOff(PURPLE);
	LedOff(BLUE);
	LedOff(CYAN);
	LedOff(GREEN);
	LedOff(YELLOW);
	LedOff(ORANGE);
	LedOff(RED);
	
	if(psOutput->pNext!=NULL)
	{
		for(psPoint=psOutput->pNext;;psPoint=psPoint->pNext)
		{
			psPoint->peGradient=aeGradient;
			psPoint->bOn=FALSE;
			psPoint->bUp=FALSE;
			psPoint->u8Gradient_Time=u8Time;
			
			if(psPoint->pNext==NULL)
			{
				break;
			}
		}
	}
}/* Finish */

/* LedCommand print. If print finish, return 1. If print not finish, return 0. ( Format in (): The head of a list) */
u8 PrintLedCommand(LedCommandType *psPrint)
{
	static bool bOn=TRUE;
	static u8 u8Count=0;
	static u8 u8CopyCount=0;
	static u8 au8Print[256]=NULL;
	static LedCommandType *psPoint=NULL;
	
	if(bOn)
	{
		bOn=FALSE;
		psPoint=psPrint;
		DebugPrintf("\n\r\n\r============     Command List     ============\n\r| Num |  LED   | TimeON | TimeOFF | Gradient |\n\r");
	}
	
	if(psPoint->pNext!=NULL)
	{
		u8 au8PrintArray[]="|     |        |        |         |          |\n\r";
		psPoint=psPoint->pNext;
		u8Count++;
		u8CopyCount++;
		
		for(u8 i=0,u8Count_1=u8Count;;i++)
		{
			au8PrintArray[4-i]=u8Count_1%10+48;
			u8Count_1/=10;
			
			if(u8Count_1==0)
			{
				break;
			}
		}
		
		switch(psPoint->eLedNum)
		{
			case WHITE:
				strcpy(&au8PrintArray[8],"WHITE");
				break;
				
			case PURPLE:
				strcpy(&au8PrintArray[8],"PURPLE");
				break;
				
			case BLUE:
				strcpy(&au8PrintArray[8],"BLUE");
				break;
				
			case CYAN:
				strcpy(&au8PrintArray[8],"CYAN");
				break;
				
			case GREEN:
				strcpy(&au8PrintArray[8],"GREEN");
				break;
				
			case YELLOW:
				strcpy(&au8PrintArray[8],"YELLOW");
				break;
				
			case ORANGE:
				strcpy(&au8PrintArray[8],"ORANGE");
				break;
				
			case RED:
				strcpy(&au8PrintArray[8],"RED");
				break;
		}
		
		for(u32 i=0,u32Count=psPoint->u32LedOn;;i++)
		{
			au8PrintArray[21-i]=u32Count%10+48;
			u32Count/=10;
			
			if(u32Count==0)
			{
				break;
			}
		}
		
		for(u32 i=0,u32Count=psPoint->u32LedOff;;i++)
		{
			au8PrintArray[30-i]=u32Count%10+48;
			u32Count/=10;
			
			if(u32Count==0)
			{
				break;
			}
		}
		
		switch(psPoint->bGradient)
		{
			case TRUE:
				strcpy(&au8PrintArray[39],"YES");
				break;
				
			case FALSE:
				strcpy(&au8PrintArray[39],"NO");
				break;
		}
		
		for(u8 i=0;i<46;i++)
		{
			if(au8PrintArray[i]=='\0')
			{
				au8PrintArray[i]=' ';
			}
		}
		
		strcat(au8Print,au8PrintArray);
		
		if(u8CopyCount==15)
		{
			u8CopyCount=0;
			DebugPrintf(au8Print);
			
			for(u8 i=0;au8Print[i]!='\0';i++)
			{
				au8Print[i]=NULL;
			}
		}
		
		if(psPoint->pNext==NULL)
		{
			bOn=TRUE;
			DebugPrintf(au8Print);
			DebugPrintf("=================    END    ==================\n\r");
			
			bOn=TRUE;
			u8Count=0;
			u8CopyCount=0;
			LedCommandType *psPoint=NULL;

			for(u8 i=0;au8Print[i]!='\0';i++)
			{
				au8Print[i]=NULL;
			}
			
			return 1;
		}
		
		if(!bOn)
		{
			return 0;
		}
	}
	else
	{
		bOn=TRUE;
		u8Count=0;
		u8CopyCount=0;
		LedCommandType *psPoint=NULL;

		for(u8 i=0;au8Print[i]!='\0';i++)
		{
			au8Print[i]=NULL;
		}
		
		DebugPrintf("            The Led command is empty !         \n\r=================    END    ==================\n\r");
		return 1;
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
	DebugSetPassthrough();
	
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
	LCDMessage(LINE2_START_ADDR,"DMO   USR    LOC  || ");
	
	/* DemoArrary */
	u8LengthCount_1=sizeof(asDemoArray_1)/sizeof(LedCommandType);
	u8LengthCount_2=sizeof(asDemoArray_2)/sizeof(LedCommandType);
	
	for(u8 i=1;i<u8LengthCount_1;i++)
	{
		asDemoArray_1[i-1].pNext=&asDemoArray_1[i];
	}
	
	for(u8 i=0;i<u8LengthCount_1;i++)
	{
		asDemoArray_1[i].peGradient=aeGradient;
		asDemoArray_1[i].bOn=FALSE;
		asDemoArray_1[i].bUp=FALSE;
		asDemoArray_1[i].u8Gradient_Time=DemoGradientTime;
	}
	
	for(u8 i=1;i<u8LengthCount_2;i++)
	{
		asDemoArray_2[i-1].pNext=&asDemoArray_2[i];
	}
	
	for(u8 i=0;i<u8LengthCount_2;i++)
	{
		asDemoArray_2[i].peGradient=aeGradient;
		asDemoArray_2[i].bOn=FALSE;
		asDemoArray_2[i].bUp=FALSE;
		asDemoArray_2[i].u8Gradient_Time=DemoGradientTime;
	}
	
	psDemoHead_1=(pLedCommandType)malloc(sizeof(LedCommandType));
	psDemoHead_2=(pLedCommandType)malloc(sizeof(LedCommandType));
	psDemoHead_1->pNext=asDemoArray_1;
	psDemoHead_2->pNext=asDemoArray_2;
	
	/* UserArray */
	psUserHead=(pLedCommandType)malloc(sizeof(LedCommandType));
	psUserTail=psUserHead;
	psUserTail->pNext=NULL;

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
	/* In Buttons */
	static bool bLock=FALSE;
	
	/* In demo lists */
	static u8 u8DemoChoose=0;
	/***********************************   end   **********************************/
	
	/********************************    Buttons    *******************************/
	if(WasButtonPressed(BUTTON2))  // Function: If bLock == TRUE, the function in bLock will be invalid.
	{
		ButtonAcknowledge(BUTTON0);
		ButtonAcknowledge(BUTTON1);
		ButtonAcknowledge(BUTTON2);
		ButtonAcknowledge(BUTTON3);
		
		u16LcdOnTime=0;
		bLcdOn=TRUE;
		bLock=!bLock;
		LedOn(LCD_RED);
		LedOn(LCD_GREEN);
		LedOn(LCD_BLUE);
		
		bButtonPressed=TRUE;
		PWMAudioOn(BUZZER1);

		if(bLock)
		{
			LCDMessage(LINE2_START_ADDR+16,"<");
		}
		else
		{
			LCDClearChars(LINE2_START_ADDR+16,1);
		}
	}
	
	if(!bLock)  // Every need to be locked should be written in this if.
	{
		if(WasButtonPressed(BUTTON0))  // Function: Show Demo Lists.
		{
			ButtonAcknowledge(BUTTON0);
			u8Button_Choose=1;
			u16LcdOnTime=0;
			bLcdOn=TRUE;
			
			LedOn(LCD_RED);
			LedOn(LCD_GREEN);
			LedOn(LCD_BLUE);
			
			bButtonPressed=TRUE;
			PWMAudioOn(BUZZER1);
		}
		
		if(WasButtonPressed(BUTTON1))  // Function: Show User List.
		{
			ButtonAcknowledge(BUTTON1);
			u8Button_Choose=2;
			u16LcdOnTime=0;
			bLcdOn=TRUE;
			
			LedOn(LCD_RED);
			LedOn(LCD_GREEN);
			LedOn(LCD_BLUE);
			
			bButtonPressed=TRUE;
			PWMAudioOn(BUZZER1);
		}
		
		if(WasButtonPressed(BUTTON3)) // Function: If bPause == TRUE, the function in bPause will be paused.
		{
			ButtonAcknowledge(BUTTON3);
			u8Button_Choose=4;
			u16LcdOnTime=0;
			bLcdOn=TRUE;
			bPause=!bPause;
			
			LedOn(LCD_RED);
			LedOn(LCD_GREEN);
			LedOn(LCD_BLUE);
			
			bButtonPressed=TRUE;
			PWMAudioOn(BUZZER1);
			
			if(bPause)
			{
				LCDMessage(LINE2_START_ADDR+18,"> ");
			}
			else
			{
				LCDMessage(LINE2_START_ADDR+18,"||");
			}
		}
	}
	/**********************************    END    *********************************/
	
	/**********************************    Pause    *********************************/
	if(!bPause)// Every need to be paused should be written in this if.
	{
		/*----------------------------------    Demo    ----------------------------------*/
		if(u8Button_Choose==1)
		{
			u8Button_Choose=0;
			bUserChoose=FALSE;
			LCDClearChars(LINE2_START_ADDR+9,1);
			
			if(++u8DemoChoose==3)
			{
				u8DemoChoose=0;
			}
			
			if(u8DemoChoose==0)
			{
				LCDClearChars(LINE2_START_ADDR+3,2);
				u32Time_Counter=0;
				LedOff(WHITE);
				LedOff(PURPLE);
				LedOff(BLUE);
				LedOff(CYAN);
				LedOff(GREEN);
				LedOff(YELLOW);
				LedOff(ORANGE);
				LedOff(RED);
			}
			
			if(u8DemoChoose==1)
			{
				LedCommandInitialize(psDemoHead_1,DemoGradientTime);
				LCDMessage(LINE2_START_ADDR+3,"<1");
			}
			
			if(u8DemoChoose==2)
			{
				LedCommandInitialize(psDemoHead_2,DemoGradientTime);
				LCDMessage(LINE2_START_ADDR+3,"<2");
			}
		}
		
		if(u8DemoChoose==1)
		{
			RunLedCommand(psDemoHead_1,DemoGradientTime);
		}
		
		if(u8DemoChoose==2)
		{
			RunLedCommand(psDemoHead_2,DemoGradientTime);
		}
		/*----------------------------------    END    ----------------------------------*/
		
		/*----------------------------------    User    ----------------------------------*/
		if(u8Button_Choose==2)
		{
			u8Button_Choose=0;
			u8DemoChoose=0;
			bUserChoose=!bUserChoose;
			LCDClearChars(LINE2_START_ADDR+3,2);
			
			if(bUserChoose)
			{
				LedCommandInitialize(psUserHead,u8Gradient_Set);
				LCDMessage(LINE2_START_ADDR+9,"<");
			}
			else
			{
				u32Time_Counter=0;
				LedOff(WHITE);
				LedOff(PURPLE);
				LedOff(BLUE);
				LedOff(CYAN);
				LedOff(GREEN);
				LedOff(YELLOW);
				LedOff(ORANGE);
				LedOff(RED);
				LCDClearChars(LINE2_START_ADDR+9,1);
			}
		}
		
		if(bUserChoose)
		{
			if(bDebug_Set)
			{
				bDebug_Set=FALSE;
				LedCommandInitialize(psUserHead,u8Gradient_Set);
			}
			
			RunLedCommand(psUserHead,u8Gradient_Set);
		}
		/*----------------------------------    END    ----------------------------------*/
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
