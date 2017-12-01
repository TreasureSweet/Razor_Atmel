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

extern u32 G_u32AntApiCurrentMessageTimeStamp;                    /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;                /* From ant_api.c */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */

static u32 UserApp1_u32Timeout;                        /* Timeout counter used across states */

static AntAssignChannelInfoType UserApp1_sChannelInfo; /* ANT setup parameters */

static bool bHRNeedInit;
static u8 u8DefaultHR;
static bool bLedIndicate;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: HeartBeatLed

Description:
If the bool is TRUE, display led like heart beat once and set bool to FALSE.

Requires:
         - Needs a bool address.
         - Needs to be run every time.

Warning:
         - Display led once need about 250ms, so if the bpm > 200, the Dispaly may be wrong.
*/
static void HeartBeatLed(bool *pbLedIndicate)
{
	/*--------------------           Variables          -----------------*/
	static u8 u8TimeCount = 0;
	static bool* pbAddrTest = NULL;
	static u32 au32LedArray[] = {0x00090000,  //       C,G
	                             0x000F0000,  //     B,C,G,Y
	                             0x000FC000,  //   P,B,C,G,Y,O
	                             0x001FE000,  // W,P,B,C,G,Y,O,R
	                             0x000FC000,  //   P,B,C,G,Y,O
	                             0x000F0000,  //     B,C,G,Y
	                             0x00090000}; //       C,G
	static u8 u8Index = 0;
	/*-------------------------------------------------------------------*/
	
	/* If Addr change, initialize the variables */
	if(pbAddrTest != pbLedIndicate)
	{
		pbAddrTest = pbLedIndicate;
		u8Index = 0;
		u8TimeCount = 0;
	} /* End initialize */
	
	/* Dislay led if *pbLedIndicate == TRUE, and set FALSE to stop this function */
	if(*pbLedIndicate)
	{
		u8TimeCount++;
		
		if(u8TimeCount == 35)
		{
			u8TimeCount = 0;
			
			u8Index++;
		}
		
		AT91C_BASE_PIOB->PIO_SODR |= au32LedArray[u8Index];
		
		if(u8Index == 6)
		{
			u8Index = 0;
			
			*pbLedIndicate = FALSE;
		}
	} /* End display */
	
} /* End Function: HeartBeatLed */


/*--------------------------------------------------------------------------------------------------------------------
Function: HeartBeatLed

Description:
Print '*' to display rate on debug

Requires:
         - Needs a HR.

Warning:
         -
*/
static void HRDebugDisplay(u8 u8HeartRate)
{
	if(u8HeartRate < 50)
	{
		DebugPrintf("*****\r\n");
	}
	else if(u8HeartRate < 60)
	{
		DebugPrintf("******\r\n");
	}
	else if(u8HeartRate < 70)
	{
		DebugPrintf("*******\r\n");
	}
	else if(u8HeartRate < 80)
	{
		DebugPrintf("********\r\n");
	}
	else if(u8HeartRate < 90)
	{
		DebugPrintf("*********\r\n");
	}
	else if(u8HeartRate < 110)
	{
		DebugPrintf("***********\r\n");
	}
	else if(u8HeartRate < 120)
	{
		DebugPrintf("************\r\n");
	}
	else if(u8HeartRate < 130)
	{
		DebugPrintf("*************\r\n");
	}
	else if(u8HeartRate < 140)
	{
		DebugPrintf("**************\r\n");
	}
	else if(u8HeartRate < 150)
	{
		DebugPrintf("***************\r\n");
	}
	else if(u8HeartRate < 160)
	{
		DebugPrintf("****************\r\n");
	}
	else if(u8HeartRate < 170)
	{
		DebugPrintf("*****************\r\n");
	}
	else if(u8HeartRate < 180)
	{
		DebugPrintf("******************\r\n");
	}
	else if(u8HeartRate < 190)
	{
		DebugPrintf("*******************\r\n");
	}
	else if(u8HeartRate < 200)
	{
		DebugPrintf("********************\r\n");
	}
	
} /* End Function: HeartBeatLed */
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
	PWMAudioOff(BUZZER1);
	PWMAudioOff(BUZZER2);
	PWMAudioSetFrequency(BUZZER1, 1000);
	PWMAudioSetFrequency(BUZZER2, 1500);
	
	/* Set bool to FALSE in addition to Display the HeartBeat led */
	bLedIndicate = FALSE;
	
	/* Set bool to TRUE in addition to set default HR */
	bHRNeedInit = TRUE;
 
	/* Network key variable */
	static u8 au8ANT_NETWORK_KEY[] = {0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};
	
	/* Configure ANT for this application */
	UserApp1_sChannelInfo.AntChannel          = ANT_CHANNEL_USERAPP;
	UserApp1_sChannelInfo.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
	UserApp1_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
	UserApp1_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;

	UserApp1_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
	UserApp1_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
	UserApp1_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
	UserApp1_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
	UserApp1_sChannelInfo.AntFrequency        = ANT_FREQUENCY_USERAPP;
	UserApp1_sChannelInfo.AntTxPower          = ANT_TX_POWER_USERAPP;

	UserApp1_sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
	
	for( u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++ )
	{
		UserApp1_sChannelInfo.AntNetworkKey[i] = au8ANT_NETWORK_KEY[i];
	}

	/* Attempt to queue the ANT channel setup */
	if( AntAssignChannel(&UserApp1_sChannelInfo) )
	{
		UserApp1_u32Timeout = G_u32SystemTime1ms;
		UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
	}
	else
	{
		/* The task isn't properly initialized, so shut it down and don't run */
		DebugPrintf("Initialize Time Out\n\r");
		
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
/* Idle */
static void UserApp1SM_Idle(void)
{
	/* Functions need to be written in if, not else */
	if(!bHRNeedInit)
	{
		/*------- Button Control -------*/
		if(WasButtonPressed(BUTTON0))
		{
			ButtonAcknowledge(BUTTON0);
			
			AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
			UserApp1_u32Timeout = G_u32SystemTime1ms;
			UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
		}
		
		if(WasButtonPressed(BUTTON1))
		{
			ButtonAcknowledge(BUTTON1);
		}
		
		if(WasButtonPressed(BUTTON2))
		{
			ButtonAcknowledge(BUTTON2);
		}
		
		if(WasButtonPressed(BUTTON3))
		{
			ButtonAcknowledge(BUTTON3);
		}
		/*----- End Button Control -----*/
	}
	else // Do once to set default HR
	{	
		AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
		UserApp1_u32Timeout = G_u32SystemTime1ms;
		UserApp1_StateMachine = UserApp1SM_WaitChannelOpen;
	}
	
} /* end UserApp1SM_Idle() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* channel opened */
static void UserApp1SM_ChannelOpen(void)
{
	static u8 u8TestHR = 0;
	static u8 u8TestHB = 0;
	static u8 u8TestDefaultHR = 0;
	static u8 au8TestDefaultHR[3];
	static u8 au8TestHR[3];
	static u8 au8OverTimesCount[] = {'0', '0', '0'};
	static u8 u8State = 0;
	static u16 u16DefaultHRRefreshSum = 0;
	static u8 u8HRChangeCount = 0;
	static u16 u16DisplayTime = 0;
	static u16 u16BuzzerTime = 0;
	static bool bDisplay = FALSE;
	static bool bLedEnabled = TRUE;
	static bool bWarnOn = TRUE;
	u8 u8DValue = 0;
	
	/*------------------- Start AntReadAppMessageBuffer() --------------------*/
	if(AntReadAppMessageBuffer())
	{
		if(G_eAntApiCurrentMessageClass == ANT_DATA)
		{
			if(u8TestHR != G_au8AntApiCurrentMessageBytes[7])
			{
				u8TestHR = G_au8AntApiCurrentMessageBytes[7];
				u8HRChangeCount++;
				u16DefaultHRRefreshSum += u8TestHR;
				u8DValue = abs(u8TestHR - u8DefaultHR);
				
				au8TestHR[0] = u8TestHR / 100 +48;
				au8TestHR[1] = (u8TestHR / 10) % 10 +48;
				au8TestHR[2] = u8TestHR % 10 +48;
				
				LCDMessage(LINE2_START_ADDR + 16,au8TestHR);
				
				/* Warning states */
				if(bWarnOn)
				{
					if(u8DValue < 20)      // WHITE
					{
						if(u8State != 0)
						{
							u8State = 0;
							
							LedPWM(LCD_RED, LED_PWM_100);
							LedPWM(LCD_GREEN, LED_PWM_100);
							LedPWM(LCD_BLUE, LED_PWM_100);
						}
					}
					else if(u8DValue < 30) // BLUE
					{
						if(u8State != 1)
						{
							u8State = 1;
							
							LedPWM(LCD_RED, LED_PWM_10);
							LedPWM(LCD_GREEN, LED_PWM_10);
							LedPWM(LCD_BLUE, LED_PWM_100);
						}
					}
					else if(u8DValue < 40) // YELLOW
					{
						if(u8State != 2)
						{
							u8State = 2;
							
							LedPWM(LCD_RED, LED_PWM_100);
							LedPWM(LCD_GREEN, LED_PWM_90);
							LedPWM(LCD_BLUE, LED_PWM_0);
						}
					}
					else if(u8DValue < 50) // ORANGE
					{
						if(u8State != 3)
						{
							u8State = 3;
							u16BuzzerTime = 0;
							
							LedPWM(LCD_RED, LED_PWM_100);
							LedPWM(LCD_GREEN, LED_PWM_20);
							LedPWM(LCD_BLUE, LED_PWM_10);
						}
					}
					else                   // RED
					{
						if(u8State != 4)
						{
							u8State = 4;
							u16BuzzerTime = 0;
							
							LedPWM(LCD_RED, LED_PWM_100);
							LedPWM(LCD_GREEN, LED_PWM_0);
							LedPWM(LCD_BLUE, LED_PWM_0);
							
							/* Over Times Count */
							if(++au8OverTimesCount[2] == '9'+1)
							{
								au8OverTimesCount[2] = '0';
								
								if(++au8OverTimesCount[1] == '9'+1)
								{
									au8OverTimesCount[1] = '0';
									
									if(++au8OverTimesCount[0] == '9'+1)
									{
										au8OverTimesCount[0] = '0';
									}
								}
							} /* End Over Times Count */
						}
					}
				} /* End warning states */
			}
			
			if(u8TestHB !=  G_au8AntApiCurrentMessageBytes[6])
			{
				u8TestHB = G_au8AntApiCurrentMessageBytes[6];
				
				bLedIndicate = TRUE;
				
				HRDebugDisplay(u8TestHR);
			}
		}
		
		if(G_eAntApiCurrentMessageClass == ANT_TICK)
		{
		}
	}
	/*----------------- End AntReadAppMessageBuffer() -----------------------*/
	
	/*---------------------- Button Function --------------------------------*/
	
	/* Display count over times */
	if(WasButtonPressed(BUTTON1))
	{
		ButtonAcknowledge(BUTTON1);
//		PWMAudioOnk(BUZZER1);
		bDisplay = TRUE;
		u16DisplayTime = 0;
		
		LCDClearChars(LINE1_START_ADDR, 20);
								   /*01234567890123456789*/
		LCDMessage(LINE1_START_ADDR,"HR Over Times  :    ");
		LCDMessage(LINE1_START_ADDR + 16, au8OverTimesCount);
	} /* END */
	
	/* Turn off warning */
	if(WasButtonPressed(BUTTON2))
	{
		ButtonAcknowledge(BUTTON2);
//		PWMAudioOn(BUZZER1);
		bDisplay = TRUE;
		u16DisplayTime = 0;
		
		LCDClearChars(LINE1_START_ADDR, 20);
		
		if(bWarnOn)
		{
			bWarnOn = FALSE;
			u8State = 0;
			u16BuzzerTime = 0;
			PWMAudioOff(BUZZER2);
			
								       /*01234567890123456789*/
			LCDMessage(LINE1_START_ADDR,"Warn Display   :OFF ");
			
			LedPWM(LCD_RED, LED_PWM_100);
			LedPWM(LCD_GREEN, LED_PWM_100);
			LedPWM(LCD_BLUE, LED_PWM_100);
		}
		else
		{
			bWarnOn = TRUE;
								       /*01234567890123456789*/
			LCDMessage(LINE1_START_ADDR,"Warn Display   : ON ");
		}
	}/* END */
	
	/* Turn off Led display */
	if(WasButtonPressed(BUTTON3))
	{
		ButtonAcknowledge(BUTTON3);
//		PWMAudioOn(BUZZER1);
		bDisplay = TRUE;
		u16DisplayTime = 0;
		
		bLedEnabled = !bLedEnabled;
		
		LCDClearChars(LINE1_START_ADDR, 20);
		
		if(bLedEnabled)
		{
								       /*01234567890123456789*/
			LCDMessage(LINE1_START_ADDR,"HB Led Display : ON ");
		}
		else
		{
								       /*01234567890123456789*/
			LCDMessage(LINE1_START_ADDR,"HB Led Display :OFF ");
		}
	} /* END */
	
	/* Clear count over times */
	if(IsButtonHeld(BUTTON1,1500))
	{
		au8OverTimesCount[0] = '0';
		au8OverTimesCount[1] = '0';
		au8OverTimesCount[2] = '0';
		
		if(u16DisplayTime == 2000)
		{
			LCDMessage(LINE1_START_ADDR + 16, au8OverTimesCount);
		}
	} /* END */
	
	if(bDisplay)
	{
		u16DisplayTime++;
		
		if(u16DisplayTime == 200)
		{
			PWMAudioOff(BUZZER1);
		}
		
		if(u16DisplayTime == 3000)
		{
			LCDClearChars(LINE1_START_ADDR, 20);
									   /*01234567890123456789*/
			LCDMessage(LINE1_START_ADDR,"Equipment state: ON ");
		}
	}
	/*------------------------ End Button -----------------------------------*/
	
	/*------------------------ DefaultHR Change ------------------------------*/
	if(u8TestDefaultHR != u8DefaultHR)
	{
		u8TestDefaultHR = u8DefaultHR;
		
		au8TestDefaultHR[0] = u8TestDefaultHR / 100 +48;
		au8TestDefaultHR[1] = (u8TestDefaultHR / 10) % 10 +48;
		au8TestDefaultHR[2] = u8TestDefaultHR % 10 +48;
		
		LCDMessage(LINE2_START_ADDR + 6,au8TestDefaultHR);
	}
	
	/* Heart Default Rate refresh */
	if(u8HRChangeCount == 200)
	{
		u8DefaultHR = u16DefaultHRRefreshSum / u8HRChangeCount;
		u16DefaultHRRefreshSum = 0;
		u8HRChangeCount = 0;
	}
	/*--------------------------- End Change ---------------------------------*/
	
	/* Heart Beat Led Display */
	if(bLedEnabled)
	{
		HeartBeatLed(&bLedIndicate);
	}
	
	/* Buzzer */
	if( (u8State >= 3) && bWarnOn )
	{
		if(u16BuzzerTime++ == 0)
		{
			PWMAudioOn(BUZZER2);
		}
		else
		{
			if(u8State == 3)
			{
				if(u16BuzzerTime == 1000)
				{
					PWMAudioOff(BUZZER2);
				}
				
				if(u16BuzzerTime == 2000)
				{
					u16BuzzerTime = 0;
				}
			}
			
			if(u8State == 4)
			{
				if(u16BuzzerTime == 500)
				{
					PWMAudioOff(BUZZER2);
				}
				
				if(u16BuzzerTime == 1000)
				{
					u16BuzzerTime = 0;
				}
			}
		}
	} /* End Buzzer */
	
	/* Button0 quit */
	if(WasButtonPressed(BUTTON0))
	{
		ButtonAcknowledge(BUTTON0);
		
		PWMAudioOff(BUZZER2);
		LedPWM(LCD_RED, LED_PWM_100);
		LedPWM(LCD_GREEN, LED_PWM_100);
		LedPWM(LCD_BLUE, LED_PWM_100);
		
		u8TestHR = 0;
		u8TestHB = 0;
		u8TestDefaultHR = 0;
		u8State = 0;
		u16BuzzerTime = 0;
		bDisplay = FALSE;
		
		AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
		UserApp1_u32Timeout = G_u32SystemTime1ms;
		UserApp1_StateMachine = UserApp1SM_WaitChannelClose;
	}
	
} /* end UserApp1SM_ChannelOpen */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Set default HR */
static void UserApp1SM_SetDefaultHR(void)
{
	static u8 u8TestHR = 0;
	static u8 u8TestHB = 0;
	static u8 u8TimesCount = 0;
	static u8 au8TimesCountDown[2];
	static u16 u16HRCount = 0;
	
	/* Count <DefaultTimes> times, add to sum: u16HRCount */
	if(u8TimesCount < DefaultTimes)
	{
		/* Start AntReadAppMessageBuffer() */
		if(AntReadAppMessageBuffer())
		{
			/* Display new message on lcd */
			if(bHRNeedInit)
			{
				bHRNeedInit = FALSE;
				LCDCommand(LCD_CLEAR_CMD);
										   /*01234567890123456789*/
				LCDMessage(LINE1_START_ADDR,"Device initializing.");
				LCDMessage(LINE2_START_ADDR,"Please wait.        ");
			} /* End display */
				
			if(G_eAntApiCurrentMessageClass == ANT_DATA)
			{
				if(u8TestHR != G_au8AntApiCurrentMessageBytes[7])
				{
					u8TimesCount++;
					
					u16HRCount += G_au8AntApiCurrentMessageBytes[7];
					u8TestHR = G_au8AntApiCurrentMessageBytes[7];
					
					au8TimesCountDown[0] = (DefaultTimes - u8TimesCount) / 10 +48;
					au8TimesCountDown[1] = (DefaultTimes - u8TimesCount) % 10 +48;
					LCDMessage(LINE2_START_ADDR+18,au8TimesCountDown);
				}
				
				if(u8TestHB !=  G_au8AntApiCurrentMessageBytes[6])
				{
					u8TestHB = G_au8AntApiCurrentMessageBytes[6];
					
					bLedIndicate = TRUE;
				}
			}
			
			if(G_eAntApiCurrentMessageClass == ANT_TICK)
			{	
				if(G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX] == EVENT_RX_FAIL)
				{
				}
				
				if(G_au8AntApiCurrentMessageBytes[ANT_TICK_MSG_EVENT_CODE_INDEX] == EVENT_RX_FAIL_GO_TO_SEARCH)
				{
				}
			}
		} /* End AntReadAppMessageBuffer() */
	}
	else // Calculate mean value and set it as default HR: u8DefaultHR
	{
		LCDCommand(LCD_CLEAR_CMD);
						           /*01234567890123456789*/
		LCDMessage(LINE1_START_ADDR,"Equipment state:OFF ");
		LCDMessage(LINE2_START_ADDR,"BUT0:Control ON/OFF ");
		
		u8DefaultHR = u16HRCount / DefaultTimes;
		
		AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
		UserApp1_u32Timeout = G_u32SystemTime1ms;
		UserApp1_StateMachine = UserApp1SM_WaitChannelClose;
	}
	
	/* Hear Beat Led Display */
	HeartBeatLed(&bLedIndicate);
	
} /* end UserApp1SM_SetDefaultHR */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel assign */
static void UserApp1SM_WaitChannelAssign(void)
{
	if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
	{
		UserApp1_StateMachine = UserApp1SM_Idle;
	}
	
	/* Watch for timeout */
	if(IsTimeUp(&UserApp1_u32Timeout, ANT_CHANNEL_TIMEOUT_UERAPP))
	{
		/* Assign failed */
		DebugPrintf("Assign Time Out\n\r");

		UserApp1_StateMachine = UserApp1SM_Error;
	}
	
} /* end UserApp1SM_WaitChannelAssign()*/


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel open */
static void UserApp1SM_WaitChannelOpen(void)
{
	if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_OPEN)
	{
		if(!bHRNeedInit)
		{
			LCDCommand(LCD_CLEAR_CMD);
									   /*01234567890123456789*/
			LCDMessage(LINE1_START_ADDR,"Equipment state: ON ");
			LCDMessage(LINE2_START_ADDR,"HR(D):    HR(N):    ");
			
			UserApp1_StateMachine = UserApp1SM_ChannelOpen;
		}
		else
		{
			LCDCommand(LCD_CLEAR_CMD);
								       /*01234567890123456789*/
			LCDMessage(LINE1_START_ADDR,"Open your HRM, wait ");
			LCDMessage(LINE2_START_ADDR,"for initialization. ");
		
			UserApp1_StateMachine = UserApp1SM_SetDefaultHR;
		}
	}
	
	/* Watch for timeout */
	if(IsTimeUp(&UserApp1_u32Timeout, ANT_CHANNEL_TIMEOUT_UERAPP))
	{
		/* Open channel failed */
		DebugPrintf("Open Time Out\n\r");
		
		UserApp1_StateMachine = UserApp1SM_Error;
	}
	
} /* end UserApp1SM_WaitChannelOpen */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for channel close */
static void UserApp1SM_WaitChannelClose(void)
{
	if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
	{
		LCDCommand(LCD_CLEAR_CMD);
						           /*01234567890123456789*/
		LCDMessage(LINE1_START_ADDR,"Equipment state:OFF ");
		LCDMessage(LINE2_START_ADDR,"BUT0:Control ON/OFF ");
		
		UserApp1_StateMachine = UserApp1SM_Idle;
	}
	
	/* Watch for timeout */
	if(IsTimeUp(&UserApp1_u32Timeout, ANT_CHANNEL_TIMEOUT_UERAPP))
	{
		/* Close channel failed */
		DebugPrintf("Close Time Out\n\r");
		
		UserApp1_StateMachine = UserApp1SM_Error;
	}
	
} /* end UserApp1SM_WaitChannelClose */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
	LedOn(RED);
	
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
