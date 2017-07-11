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
  LedOff(WHITE);
  LedOn(RED);
  LedOff(GREEN);
  LedOff(PURPLE);
    
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
  static u32 u32Password=1111111111;
  static u32 u32PasswordJudge=0;
  static u32 u32ChooseTime=0;
  static bool bPasswordChange=FALSE;
  static bool bLightOn=FALSE;
  static u8 u8PreeBlink=0;
  static u16 u16ChangeBlink=0;
  
  u8 u8ButtonNum=5;
  
  /*Give every button a num*/
  if(WasButtonPressed(BUTTON0))
  {
    u8ButtonNum=1;
    LedOn(WHITE);
    bLightOn=TRUE;
    ButtonAcknowledge(BUTTON0);
  }
  
  if(WasButtonPressed(BUTTON1))
  {
    u8ButtonNum=2;
    LedOn(WHITE);
    bLightOn=TRUE;
    ButtonAcknowledge(BUTTON1);
  }
  
  if(WasButtonPressed(BUTTON2))
  {
    u8ButtonNum=3;
    LedOn(WHITE);
    bLightOn=TRUE;
    ButtonAcknowledge(BUTTON2);
  }

  if(WasButtonPressed(BUTTON3))
  {
    u8ButtonNum=4;
    LedOn(WHITE);
    bLightOn=TRUE;
    ButtonAcknowledge(BUTTON3);
  }  
  
  /*If you want to change password,you must hold BUTTON3 the first 2 scends*/
  if(u32ChooseTime<2000)
  {
    u32ChooseTime++;
  
    /*Password change when hold BUTTON3 1.5s*/
    if(IsButtonHeld(BUTTON3,1500))
    {
      bPasswordChange=TRUE;
      u32Password=0;
    }
  
    if(bPasswordChange)
    {
      /*Blink the red an green light ever 1s*/
      u16ChangeBlink++;
      u32ChooseTime=0;
      
      if(u16ChangeBlink==500)
      {
        LedToggle(RED);
        LedToggle(GREEN);
        u16ChangeBlink=0;
      }
    
      /*Import password with button0~2,and use button3 to save*/
      if(u8ButtonNum<4)
      {
        u32Password=10*u32Password+u8ButtonNum;
      }
      
      /*Identify and pop-up mode*/
      if(u8ButtonNum==4)
      {
        u32ChooseTime=2000;
        LedOn(RED);
        LedOff(GREEN);
        u16ChangeBlink=0;      
      }
    }   
  }
  else
  {
    /*Inport password and distinguish true or false*/
    if(u8ButtonNum<4)
    {
      u32PasswordJudge=10*u32PasswordJudge+u8ButtonNum;
    }
  
    /*Distinguish when BUTTON4 is pressed*/
    if(u8ButtonNum==4)
    {
      if(u32PasswordJudge==u32Password)//Grenn blink when true and turn off red
      {
        LedOff(RED);
        LedBlink(GREEN,LED_4HZ);
      }
      else//Red blink when false
      {
        LedBlink(RED,LED_4HZ);
      }
    }
  }
  
  /*Push button light*/
  if(bLightOn)
  {
    if(u8PreeBlink++==200)
    {
      u8PreeBlink=0;
      LedOff(WHITE);
      bLightOn=FALSE;
    }
  }
  
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
