/**********************************************************************************************************************
File: user_app1.h                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
1. Follow the instructions at the top of user_app1.c
2. Use ctrl-h to find and replace all instances of "user_app1" with "yournewtaskname"
3. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
4. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
5. Add #include yournewtaskname.h" to configuration.h
6. Add/update any special configurations required in configuration.h (e.g. peripheral assignment and setup values)
7. Delete this text (between the dashed lines)
----------------------------------------------------------------------------------------------------------------------

Description:
Header file for user_app1.c

**********************************************************************************************************************/

#ifndef __USER_APP1_H
#define __USER_APP1_H

/**********************************************************************************************************************
Type Definitions
**********************************************************************************************************************/
typedef struct
{
	LedNumberType eLedNum; // Choose a led
	u32 u32LedOn;          // The time turn on led
	u32 u32LedOff;         // The time turn off led
	bool bGradient;        // If the led need gradient
	void *pNext;           // Point to the next struct
	LedRateType *peGradient;  // Point to PWM array to realize gradient
	bool bOn;                 // Turn TRUE when arrive u32LedOn or u32LedOff
	bool bUp;                 // Gradient up or down ( up: TRUE, down: FALSE )
	u8 u8Gradient_Time;       // Used to set gradient speed
}LedCommandType,*pLedCommandType;

/**********************************************************************************************************************
Constants / Definitions
**********************************************************************************************************************/
#define DemoGradientTime (u8)20 // Gradient speed of demo lists

/**********************************************************************************************************************
Function Declarations
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/
void RunLedCommand(LedCommandType *,u8);
void LedCommandInitialize(LedCommandType *,u8);
u8 PrintLedCommand(LedCommandType *);
/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/
void UserApp1Initialize(void);
void UserApp1RunActiveState(void);


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/***********************************************************************************************************************
State Machine Declarations
***********************************************************************************************************************/
static void UserApp1SM_Idle(void);    

static void UserApp1SM_Error(void);         


#endif /* __USER_APP1_H */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
