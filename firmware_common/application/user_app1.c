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

static u8 au8Data[16][10] =
{
	{0x02,0x20,	0x00,0x00,	0x20,0x40,	0x00,0x00,	0x08,0x20},
	{0x12,0x20,	0x1F,0xF0,	0x3F,0x7E,	0x00,0x00,	0x08,0x20},
	{0x12,0x20,	0x11,0x10,	0x48,0x90,	0x00,0x00,	0xFF,0xFE},
	{0x12,0x20,	0x11,0x10,	0x85,0x08,	0x00,0x00,	0x08,0x20},
	{0x12,0x20,	0x1F,0xF0,	0x3F,0xF8,	0x00,0x00,	0x00,0x00},
	{0xFF,0xFE,	0x11,0x10,	0x01,0x08,	0x00,0x00,	0x00,0xFC},
	{0x12,0x20,	0x11,0x10,	0x01,0x08,	0x00,0x00,	0x7C,0x84},
	{0x12,0x20,	0x1F,0xF0,	0x3F,0xF8,	0xFF,0xFE,	0x44,0x84},
	{0x12,0x20,	0x02,0x80,	0x21,0x00,	0x00,0x00,	0x44,0xFC},
	{0x12,0x20,	0x0C,0x60,	0x21,0x00,	0x00,0x00,	0x7C,0x84},
	{0x13,0xE0,	0x34,0x58,	0x3F,0xFC,	0x00,0x00,	0x44,0x84},
	{0x10,0x00,	0xC4,0x46,	0x03,0x04,	0x00,0x00,	0x44,0xFC},
	{0x10,0x00,	0x04,0x40,	0x05,0x04,	0x00,0x00,	0x7C,0x84},
	{0x10,0x00,	0x08,0x40,	0x19,0x28,	0x00,0x00,	0x44,0x84},
	{0x1F,0xFC,	0x08,0x40,	0xE1,0x10,	0x00,0x00,	0x01,0x14},
	{0x00,0x00,	0x10,0x40,	0x01,0x00,	0x00,0x00,	0x02,0x08}
};

static u8 au8Dispaly[16][10];

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
	CD45_STB_TO_H;
	CD45_INH_TO_H;
	MBI_OE_TO_L;
	
	AT91C_BASE_PIOA->PIO_PER |= AT91C_PIO_PA0;
	AT91C_BASE_PIOA->PIO_OER |= AT91C_PIO_PA0;
	AT91C_BASE_PIOA->PIO_CODR = AT91C_PIO_PA0;
	
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
static void DisplayMessage(void)
{
	static u8 u8Delay = 0;
	static u8 u8OverBit_Old;
	static u8 u8OverBit_New = 0;
	static u8 u8Count = 0;
	static u8 u8Index = 0;
	static u8* pStr1 = NULL;
	static u8* pStr2 = NULL;
	static u8 u8CountTimes = 0;
	static bool bOn = TRUE;
	
	if(0 == u8Delay)
	{
		u8Delay = 60;
		
		if(80 == u8CountTimes)
		{
			u8CountTimes = 0;
			bOn = !bOn;
		}
		
		if(bOn)
		{
			if(0 == u8Count)
			{
				u8Count = 0x80;
				pStr1 = au8Data[0] + u8Index;
				
				if(9 == u8Index++)
				{
					u8Index = 0;
				}
			}
			
			pStr2 = pStr1;
			
			for(u8 y = 0; y < 16; y++)
			{
				for(u8 x = 10; x; x--)
				{
					u8OverBit_Old = u8OverBit_New;
					u8OverBit_New = au8Dispaly[y][x-1] >> 7;
					au8Dispaly[y][x-1] = au8Dispaly[y][x-1] << 1;
					
					if(10 == x)
					{
						au8Dispaly[y][x-1] |= (*pStr2 & u8Count) / u8Count;
						pStr2 += 10;
					}
					else
					{
						au8Dispaly[y][x-1] |= u8OverBit_Old;
					}
				}
				
				u8OverBit_New = 0;
			}
			
			u8Count = u8Count >> 1;
		}
		else
		{
			for(u8 y = 0; y < 16; y++)
			{
				for(u8 x = 10; x; x--)
				{
					u8OverBit_Old = u8OverBit_New;
					u8OverBit_New = au8Dispaly[y][x-1] >> 7;
					au8Dispaly[y][x-1] = au8Dispaly[y][x-1] << 1;
					
					if(10 == x)
					{
						au8Dispaly[y][x-1] |= 0;
						pStr2 += 10;
					}
					else
					{
						au8Dispaly[y][x-1] |= u8OverBit_Old;
					}
				}
				
				u8OverBit_New = 0;
			}
		}
		
		u8CountTimes++;
	}
	
	u8Delay--;
}

/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
	static u8 u8Index = 0;
	
	/* CD45 Loop Once */
	if(16 == u8Index)
	{
		u8Index = 0;
	}
	/* End loop once */
	
	/*-----------------------  Function  -------------------------*/
	DisplayMessage();
	
	for(u8 i = 10; i; i--)
	{
		MBI_SEND_U8(au8Dispaly[u8Index][i-1]);
	}
	
	/*---------------------- End Function  -----------------------*/
	
	/* CD45 Output choose */
	CD45_OUTPUT_CHOOSE(u8Index++);
	CD45_INH_TO_L;
	for(u8 j = 200; j; j--);
	CD45_INH_TO_H;
	/* End CD45 Output choose */
	
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */

/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
