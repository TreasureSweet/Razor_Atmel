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
u8 u8CallbackTimesCount;
static u32 u32VoltageCount;
static u8 u8VolumeLevel;

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

/*-------------------------------------------------------------------------------------------------------------------*/
/* ADC callback */
void UserApp1_AdcCallback(u16 u16Result_)
{
	u32VoltageCount += u16Result_;
} /* end UserApp1_AdcCallback() */

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
	/* ADC Initialize */
	u8CallbackTimesCount = 0;
	u32VoltageCount = 0;
	
	/* Initalize Leds */
	LedOffAll();
	LedOn(PURPLE);
	
	/* Initialize pins */
	AT91C_BASE_PIOA->PIO_PER |= PIN_RE;
	AT91C_BASE_PIOA->PIO_OER |= PIN_RE;
	AT91C_BASE_PIOA->PIO_CODR = PIN_RE;      // RE to low
	
	/* Initialize LCD */
	LCDCommand(LCD_CLEAR_CMD);  /*01234567890123456789*/
	LCDMessage(LINE1_START_ADDR, "Channel: MUTE       ");
	LCDMessage(LINE2_START_ADDR, "Level  :   00    MIN");
	_74HC4053D_SWITCH_XY(BY);
	
	/* Turn wiper destination to the lowest (Volume level 0)*/
	X9C103S_RESISTANCE_UP(99);
	u8VolumeLevel = 0;
	
	/* Assign ADC callback */
	Adc12AssignCallback(ADC12_CH2, UserApp1_AdcCallback);
	
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

/*-------------------------------------------------------------------------------------------------------------------*/
/* Led Blink for button press */
static void ButtonPressLedDisplay(bool *pbPress)
{
	static u8 u8BlinkTime = 200;      // Red led blink time count
	
	if(*pbPress)                      // Check if the first press
	{
		*pbPress = FALSE;
		u8BlinkTime = 0;
		
		LedOn(ButtonPressLed);
	}
	
	if(u8BlinkTime < 200)             // Not the first, so count for led on time
	{
		u8BlinkTime++;
	}
	
	if(u8BlinkTime == 200)            // Time is up, turn off led
	{
		LedOff(ButtonPressLed);
	}
} /* end ButtonPressLedDisplay */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Turn Volume up */
static void VolumeUp(void)
{
	static u8 au8LevelOnLcd[] = "00";         // Displayed on lcd
	
	X9C103S_RESISTANCE_DOWN(1);
	
	/* Volume level count */
	if(u8VolumeLevel < 99)
	{
		if(u8VolumeLevel == 0) // Clear " MIN "
		{
			LCDClearChars(LINE2_START_ADDR + 17, 3);
		}
		
		u8VolumeLevel++;
		
		if(u8VolumeLevel == 99) // Display " MAX "
		{
			LCDMessage(LINE2_START_ADDR + 17, "MAX");
		}
	}
	
	/* Volume level display */
	au8LevelOnLcd[0] = u8VolumeLevel / 10 + 48;
	au8LevelOnLcd[1] = u8VolumeLevel % 10 + 48;
	LCDMessage(LINE2_START_ADDR +11, au8LevelOnLcd);
	
} /* end VolumeUp */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Turn Volume down */
static void VolumeDown(void)
{
	static u8 au8LevelOnLcd[] = "00";         // Displayed on lcd
	
	X9C103S_RESISTANCE_UP(1);
	
	/* Volume level count */
	if(u8VolumeLevel > 0)
	{
		if(u8VolumeLevel == 99) // Clear " MAX "
		{
			LCDClearChars(LINE2_START_ADDR + 17, 3);
		}
		
		u8VolumeLevel--;
		
		if(u8VolumeLevel == 0) // Display " MIN "
		{
			LCDMessage(LINE2_START_ADDR + 17, "MIN");
		}
	}
	
	/* Volume level display */
	au8LevelOnLcd[0] = u8VolumeLevel / 10 + 48;
	au8LevelOnLcd[1] = u8VolumeLevel % 10 + 48;
	LCDMessage(LINE2_START_ADDR +11, au8LevelOnLcd);
	
} /* end VolumeDown */

/*-------------------------------------------------------------------------------------------------------------------*/
/* Change mode */
static void ModeChange(void)
{
	static u8 u8ModeId = 2;           // Used to change mode
	
	switch(u8ModeId)
	{
		case 0: // Change to phone signal mode
		{
			LCDMessage(LINE1_START_ADDR, "Channel: PHONE      ");
			
			u8ModeId = 1;
			LedOff(BLUE);
			LedOn(GREEN);
			
			_74HC4053D_SWITCH_XY(BX);
			_74HC4053D_SWITCH_XY(AX);
			break;
		}
		
		case 1: // Change to mute mode
		{
			LCDMessage(LINE1_START_ADDR, "Channel: MUTE       ");
			
			u8ModeId = 2;
			LedOff(GREEN);
			LedOn(PURPLE);
			
			_74HC4053D_SWITCH_XY(BY);
			break;
		}
		
		case 2: // Change to MIC signal mode
		{
			LCDMessage(LINE1_START_ADDR, "Channel: MIC        ");
			
			u8ModeId = 0;
			LedOff(PURPLE);
			LedOn(BLUE);
			
			_74HC4053D_SWITCH_XY(BX);
			_74HC4053D_SWITCH_XY(AY);
			break;
		}
		
		default:
		{
			LedBlink(RED, LED_4HZ);
			UserApp1_StateMachine = UserApp1SM_Error;
			break;
		}
	}
} /* end ModeChange */

/*-------------------------------------------------------------------------------------------------------------------*/
/* VolageCheck */
static void VolageCheck(bool *pbVCheck)
{
	static u8 u8Wait = 0;                    // Wait for volage change to 3.3
	static u16 u16DispalyTime = 0;           // Time for displaying volage
	                              /*0123*/
	static u8 au8VolageDisplay[] = "3.3V";   // Displayed on lcd line1
	
	if(u8CallbackTimesCount > 0)             // Start conversion continuously to count average
	{
		if(Adc12StartConversion(ADCUsed))
		{
			u8CallbackTimesCount--;
			
			if(u8CallbackTimesCount == 0)    // Conversion finish, count average
			{
				u32VoltageCount /= AdcCallbackTimes;
				u32VoltageCount = ((3300 * u32VoltageCount) / 4096) / 10;
				
				if(u32VoltageCount % 10 >= 5)// Come in five
				{
					u32VoltageCount /= 10;
					
					if((u32VoltageCount % 10) + 1 == 10)
					{
						au8VolageDisplay[2] = 48;
						u32VoltageCount += 10;
					}
					else
					{
						au8VolageDisplay[2] = (u32VoltageCount % 10) + 1 + 48;
					}
				}
				else
				{
					u32VoltageCount /= 10;
					au8VolageDisplay[2] = (u32VoltageCount % 10) + 48;
				}
				
				u32VoltageCount /= 10;
				au8VolageDisplay[0] = u32VoltageCount + 48;
				
				AT91C_BASE_PIOA->PIO_CODR = PIN_RE;      // RE to low
				_74HC4053D_SWITCH_XY(CX);
				LedOff(WHITE);
				u16DispalyTime = 5000;
				LCDMessage(LINE1_START_ADDR + 16, au8VolageDisplay);
		 	}
		}
	}
	
	if(u16DispalyTime > 0)                               // Clear volage on lcd 5s later
	{
		u16DispalyTime--;
		
		if(u16DispalyTime == 0)
		{
			LCDClearChars(LINE1_START_ADDR + 16, 4);
		}
	}
	
	if(*pbVCheck)                                       // Start check if bool is ture
	{
		AT91C_BASE_PIOA->PIO_SODR = PIN_RE;      // RE to high
		_74HC4053D_SWITCH_XY(CY);    // 4053C to CY (3.3V)
		
		if(u8Wait++ == 5)
		{
			u8Wait = 0;
			
			if(Adc12StartConversion(ADCUsed))
			{
				LedOn(WHITE);
				u32VoltageCount = 0;
				*pbVCheck = FALSE;
				u8CallbackTimesCount = AdcCallbackTimes;
			}
		}
	}
} /* end VolageCheck */

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
	/*
	Function:
	BUTTON0: Volume Up
	BUTTON1: Volume Down
	BUTTON2: Measure Position of X9C103
	BUTTON3: Change Mode < 1.MIC Signal (id = 0)  2.Phone Signal (id = 1)  3.Mute (id = 2) >
	
	Note: 1. Blink led red every time you press button.
	      2. When measure X9C103, turn white led on, or turn it off.
	      3. If use MIC, blue led on. If use phone, green led on. If mute, purple led on. Only one led works at the same time.
	      4. Show volume level and channel id on lcd.
	*/
	
	static bool bPress = FALSE;       // Check button press
	static bool bVCheck = FALSE;      // If start Volage check
	static u8 u8ChangeInterval = 0;   // Interval time of changing volume continuously
	
	/*---------------------      BUTTON0         ---------------------*/
	if(WasButtonPressed(BUTTON0))    // Change once < Press >
	{
		ButtonAcknowledge(BUTTON0);
		u8ChangeInterval = 0;
		bPress = TRUE;
		VolumeUp();
	}
	
	if(IsButtonHeld(BUTTON0, 1500))  // Change continuously < Hold >
	{
		if(u8ChangeInterval == 0)
		{
			VolumeUp();
		}
		
		if(++u8ChangeInterval == 150)
		{
			u8ChangeInterval = 0;
		}
	}
	/*--------------------     END BUTTON0         -------------------*/
	
	/*---------------------      BUTTON1         ---------------------*/
	if(WasButtonPressed(BUTTON1))    // Change once < Press >
	{
		ButtonAcknowledge(BUTTON1);
		u8ChangeInterval = 0;
		bPress = TRUE;
		VolumeDown();
	}
	
	if(IsButtonHeld(BUTTON1, 1500))  // Change continuously < Hold >
	{
		if(u8ChangeInterval == 0)
		{
			VolumeDown();
		}
		
		if(++u8ChangeInterval == 150)
		{
			u8ChangeInterval = 0;
		}
	}
	/*--------------------     END BUTTON1         -------------------*/
	
	/*---------------------      BUTTON2         ---------------------*/
	if(WasButtonPressed(BUTTON2))
	{
		ButtonAcknowledge(BUTTON2);
		bPress = TRUE;
		bVCheck = TRUE;
	}
	/*--------------------     END BUTTON2         -------------------*/
	
	/*---------------------      BUTTON3         ---------------------*/
	if(WasButtonPressed(BUTTON3))   // Mode change (initial is MUTE)
	{
		ButtonAcknowledge(BUTTON3);
		bPress = TRUE;
		ModeChange();
	}
	/*--------------------     END BUTTON3         -------------------*/
	
	/* Handle */
	ButtonPressLedDisplay(&bPress);
	VolageCheck(&bVCheck);
	
} /* end UserApp1SM_Idle() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
