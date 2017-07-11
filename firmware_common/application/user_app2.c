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
  LedOff(WHITE);
  LedOff(PURPLE);
  LedOff(BLUE);
  LedOff(CYAN);
  LedOff(GREEN);
  LedOff(YELLOW);
  LedOff(ORANGE);
  LedOff(RED);
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
  static u16 u16Counter=0;
  static u16 u16Counter_1=0;
  static u8 u8Choose=0;
  static LedRateType aeLedHz[]={LED_PWM_0, LED_PWM_5, LED_PWM_10, LED_PWM_15, LED_PWM_20, 
                                LED_PWM_25, LED_PWM_30, LED_PWM_35, LED_PWM_40, LED_PWM_45, 
                                LED_PWM_50, LED_PWM_55 , LED_PWM_60, LED_PWM_65, LED_PWM_70, 
                                LED_PWM_75, LED_PWM_80, LED_PWM_85, LED_PWM_90, LED_PWM_95, 
                                LED_PWM_100,LED_PWM_100,LED_PWM_95,LED_PWM_90,LED_PWM_85,
                                LED_PWM_80,LED_PWM_75,LED_PWM_70,LED_PWM_65, LED_PWM_60,
                                LED_PWM_55,LED_PWM_50,LED_PWM_45,LED_PWM_40, LED_PWM_35,
                                LED_PWM_30,LED_PWM_25,LED_PWM_20,LED_PWM_15,LED_PWM_10,
                                LED_PWM_5,LED_PWM_0};
  static LedRateType* peLed1=aeLedHz;
  static LedRateType* peLed2=aeLedHz;
  static LedRateType* peLed3=aeLedHz;
  static LedRateType* peLed4=aeLedHz;
  static LedRateType* peLed5=aeLedHz;
  static LedRateType* peLed6=aeLedHz;
  static LedRateType* peLed7=aeLedHz;
  static LedRateType* peLed8=aeLedHz;
  
  u16Counter++;
  u16Counter_1++;
/*1   ************************************/  
  if(peLed1-&aeLedHz[0]==40)
  {
    peLed1=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=0)
  {
    LedPWM(WHITE,*(peLed1++));
  }

  /*2   **********************************/
  if(peLed2-&aeLedHz[0]==40)
  {
    peLed2=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=1)
  {
    LedPWM(PURPLE,*(peLed2++));
  }

  /*3   **********************************/
  if(peLed3-&aeLedHz[0]==40)
  {
    peLed3=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=2)
  {
    LedPWM(BLUE,*(peLed3++));
  }
  
  /*4   **********************************/
  if(peLed4-&aeLedHz[0]==40)
  {
    peLed4=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=3)
  {
    LedPWM(CYAN,*(peLed4++));
  }

  /*5   **********************************/
  if(peLed5-&aeLedHz[0]==40)
  {
    peLed5=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=4)
  {
    LedPWM(GREEN,*(peLed5++));
  }

  /*6   **********************************/
  if(peLed6-&aeLedHz[0]==40)
  {
    peLed6=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=5)
  {
    LedPWM(YELLOW,*(peLed6++));
  }

  /*7   **********************************/
  if(peLed7-&aeLedHz[0]==40)
  {
    peLed7=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=6)
  {
    LedPWM(ORANGE,*(peLed7++));
  }

  /*8   **********************************/
  if(peLed8-&aeLedHz[0]==40)
  {
    peLed8=aeLedHz;
  }
  
  if(u16Counter==100&&u8Choose>=7)
  {
    LedPWM(RED,*(peLed8++));
  }
  
  /*The adjacent time difference*/  
  if(u16Counter_1==400)
  {
    if(u8Choose<=8)
    {
      u8Choose++;
    }
    u16Counter_1=0;
  }
  
  if(u16Counter==100)
  {
    u16Counter=0;
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
