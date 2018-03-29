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

/*-------------------------------------------------------------------------------------------------------------------
Function: LedOffAll

Description:
turn all leds off.
*/
static void LedOffAll(void)
{
	LedOff(RED);
	LedOff(ORANGE);
	LedOff(YELLOW);
	LedOff(GREEN);
	LedOff(CYAN);
	LedOff(BLUE);
	LedOff(PURPLE);
	LedOff(WHITE);
} /* end LedOffAll() */

/*-------------------------------------------------------------------------------------------------------------------
Function: LedOnAll

Description:
turn all leds on.
*/
static void LedOnAll(void)
{
	LedOn(RED);
	LedOn(ORANGE);
	LedOn(YELLOW);
	LedOn(GREEN);
	LedOn(CYAN);
	LedOn(BLUE);
	LedOn(PURPLE);
	LedOn(WHITE);
} /* end LedOnAll() */

/*-------------------------------------------------------------------------------------------------------------------
Function: DelayU8

Description:
Do <u8> times loop to simulate delay.

Promises:
Need an u8 input 0 ~ 255.
*/
static void DelayU8(u8 u8Time)
{
	for(u8 i = u8Time; i; i--)
	{
	};
} /* end DelayU8 */

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
	/* Initalize Leds */
	LedOffAll();
	LedOn(PURPLE);
	
	/* Enable PIO */
	//PIOB
	AT91C_BASE_PIOB->PIO_PER |= 0x00000018;           // Enable pio control of AN1 and AN0
	AT91C_BASE_PIOB->PIO_OER |= 0x00000018;           // Enable pins output control
	
	//PIOA
	AT91C_BASE_PIOA->PIO_PER |= 0x0001F800;           // Enable pio control of SCK, MISO, MOSI, TX, RX
	AT91C_BASE_PIOA->PIO_OER |= 0x0001F800;           // Enable pins output control
	
	/* Initialize pins */
	AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;       // AD to low
	AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;      // RE to low
	AT91C_BASE_PIOA->PIO_SODR = PA_13_BLADE_MISO;     // CS to high
	AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;     // UD to low
	AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;    // INC to low
	AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;    // 4053C to CX (A_OUT)
	AT91C_BASE_PIOB->PIO_SODR = PB_04_BLADE_AN1;      // 4053B to BY (GND)
	AT91C_BASE_PIOB->PIO_CODR = PB_03_BLADE_AN0;      // 4053A to AX (AUD2)
	
	/* Initialize LCD */
	LCDCommand(LCD_CLEAR_CMD);  /*01234567890123456789*/
	LCDMessage(LINE1_START_ADDR, "Channel: MUTE       ");
	LCDMessage(LINE2_START_ADDR, "Level  :   00       ");
	
	/* Turn wiper destination to the lowest (Volume level 0)*/
	for(u8 i = 99; i; i--)
	{
		AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;  // CS to low (Enable wiper change)
		AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;  // UD to low (Wiper down)
		DelayU8(2);
		
		/* INC change from high to low (change wiper once) */
		AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI; // INC to low
		DelayU8(2);
		
		/* CS change from low to high (store wiper position)*/
		AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_SODR = PA_13_BLADE_MISO;  // CS to high
	}
	
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
	
	Contrast: | Control board | Controlled board |               Address              |
	          |     +5V       |       5V         |                                    |
	          |     GND       |       GND        |                                    |
	          |     ICS       |       AD         | PA_16_BLADE_CS     (u32)0x00010000 |
	          |     SCK       |       RE         | PA_15_BLADE_SCK    (u32)0x00008000 |
	          |     MISO      |       CS         | PA_13_BLADE_MISO   (u32)0x00002000 |
	          |     MOSI      |       UD         | PA_14_BLADE_MOSI   (u32)0x00004000 |
	          |     TX        |       INC        | PA_12_BLADE_UPOMI  (u32)0x00001000 |
	          |     RX        |       4053C      | PA_11_BLADE_UPIMO  (u32)0x00000800 |
	          |     AN1       |       4053B      | PB_04_BLADE_AN1    (u32)0x00000010 |
	          |     AN0       |       4053A      | PB_03_BLADE_AN0    (u32)0x00000008 |
	*/
	
	static bool bPress = FALSE;       // Check button press
	static bool bLevelChange = FALSE; // Check if volume level change
	static u8 u8BlinkTime = 255;      // Red led blink time count
	static u8 u8ModeId = 2;           // Used to change mode
	static u8 u8VolumeLevel = 0;      // Volume level count
	static u8 u8LevelOnLcd = 0;       // Show u8VolumeLevel on lcd
	
	/* BUTTON0 */
	if(WasButtonPressed(BUTTON0))
	{
		ButtonAcknowledge(BUTTON0);
		
		bPress = TRUE;
		
		/* Enable wiper control */
		AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;  // CS to low (Enable wiper change)
		AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;  // UD to high (Wiper up)
		DelayU8(2);
		
		/* INC change from high to low (change wiper once) */
		AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI; // INC to low
		DelayU8(2);
		
		/* CS change from low to high (store wiper position)*/
		AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_SODR = PA_13_BLADE_MISO;  // CS to high
		
		/* Volume level count */
		if(u8VolumeLevel < 99)
		{
			bLevelChange = TRUE;
			u8VolumeLevel++;
		}
		
	}/* end BUTTON0 */
	
	/* BUTTON1 */
	if(WasButtonPressed(BUTTON1))
	{
		ButtonAcknowledge(BUTTON1);
		
		bPress = TRUE;
		
		/* Enable wiper control */
		AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;  // CS to low (Enable wiper change)
		AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;  // UD to low (Wiper down)
		DelayU8(2);
		
		/* INC change from high to low (change wiper once) */
		AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI; // INC to low
		DelayU8(2);
		
		/* CS change from low to high (store wiper position)*/
		AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI; // INC to high
		DelayU8(2);
		AT91C_BASE_PIOA->PIO_SODR = PA_13_BLADE_MISO;  // CS to high
		
		/* Volume level count */
		if(u8VolumeLevel > 0)
		{
			bLevelChange = TRUE;
			u8VolumeLevel--;
		}
		
	}/* end BUTTON1 */
	
	/* BUTTON2 */
	if(WasButtonPressed(BUTTON2))
	{
		ButtonAcknowledge(BUTTON2);
		
		bPress = TRUE;
		
		if((AT91C_BASE_PIOB->PIO_ODSR & PB_13_LED_WHT) == PB_13_LED_WHT) // Disable measure
		{
			LedOff(WHITE);
			
			AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;    // 4053C to CX (A_OUT)
		}
		else // Enable measure
		{
			LedOn(WHITE);
			
			AT91C_BASE_PIOA->PIO_SODR = PA_11_BLADE_UPIMO;    // 4053C to CY (3.3V)
		}
		
	}/* end BUTTON2 */
	
	/* BUTTON3 */
	if(WasButtonPressed(BUTTON3))
	{
		ButtonAcknowledge(BUTTON3);
		
		bPress = TRUE;
		
		switch(u8ModeId)
		{
			case 0: // Change to phone signal mode
			{
				LCDMessage(LINE1_START_ADDR, "Channel: PHONE      ");
				
				u8ModeId = 1;
				LedOff(BLUE);
				LedOn(GREEN);
				
				AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;      // 4053B to BX (Aout)
				AT91C_BASE_PIOB->PIO_CODR = PB_03_BLADE_AN0;      // 4053A to AX (AUD2)
				break;
			}
			
			case 1: // Change to mute mode
			{
				LCDMessage(LINE1_START_ADDR, "Channel: MUTE       ");
				
				u8ModeId = 2;
				LedOff(GREEN);
				LedOn(PURPLE);
				
				AT91C_BASE_PIOB->PIO_SODR = PB_04_BLADE_AN1;      // 4053B to BY (GND)
				break;
			}
			
			case 2: // Change to MIC signal mode
			{
				LCDMessage(LINE1_START_ADDR, "Channel: MIC        ");
				
				u8ModeId = 0;
				LedOff(PURPLE);
				LedOn(BLUE);
				
				AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;      // 4053B to BX (Aout)
				AT91C_BASE_PIOB->PIO_SODR = PB_03_BLADE_AN0;      // 4053A to AY (AUD1)
				break;
			}
			
			default:
			{
				LedBlink(RED, LED_4HZ);
				UserApp1_StateMachine = UserApp1SM_Error;
				break;
			}
		}
	}/* end BUTTON3 */
	
	/*---------Led Red Blink for button-----------*/
	if(bPress)
	{
		bPress = FALSE;
		u8BlinkTime = 0;
		
		LedOn(RED);
	}
	
	if(u8BlinkTime < 200)
	{
		u8BlinkTime++;
	}
	else if(u8BlinkTime == 200)
	{
		u8BlinkTime = 255;
		
		LedOff(RED);
	}
	/*------------end Led Red Blink---------------*/
	
	/*------------Lcd Volume Level----------------*/
	if(bLevelChange)
	{
		bLevelChange = FALSE;
		
		u8LevelOnLcd = u8VolumeLevel / 10 + 48;
		LCDMessage(LINE2_START_ADDR +11, &u8LevelOnLcd);
		
		u8LevelOnLcd = u8VolumeLevel % 10 + 48;
		LCDMessage(LINE2_START_ADDR +12, &u8LevelOnLcd);
	}
	/*----------end lcd volume level--------------*/
	
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
