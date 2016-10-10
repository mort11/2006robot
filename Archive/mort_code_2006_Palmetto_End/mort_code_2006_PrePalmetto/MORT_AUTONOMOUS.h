/*******************************************************************************
* FILE NAME: MORT_AUTONOMOUS.h
*
* DESCRIPTION: 
*  This is the include file that defines all I/O used in the MORT 2005 Robot
*  It contains aliases and function prototypes used in all MORT 'C' routines.
*
*******************************************************************************/
#include "MORT_DEFINES.h"


// Defines for different autonomous modes
#define CLOSE_SHOOT				1		// closest position DON'T move, use camera to pan, pull hood all the way back
#define CLOSE_STRAIGHT			2		// closest position move straight to goal w/camera
#define CLOSE_DELAY				3		// closest position delay then move straight to goal w/camera
#define MIDDLE_RIGHT			4		// middle position move to center field, turn right, move to target, shoot
#define DEFENSE					5		// farthest position move straight to goal to block



// Define the state for CLOSE_STRAIGHT
// No states for this mode

// Define the state for CLOSE_SHOOT
// No states for this mode

// Define the state for CLOSE_DELAY
#define CLOSE_DELAY_WAIT		1
#define CLOSE_DELAY_SHOOT		2

// Define the state for MIDDLE_RIGHT
#define MIDDLE_RIGHT_OUT 		1
#define MIDDLE_RIGHT_DELAY		2
#define MIDDLE_RIGHT_TURN		3
#define MIDDLE_RIGHT_DELAY_TWO	4
#define MIDDLE_RIGHT_APPROACH	5
#define MIDDLE_RIGHT_SHOOT		6

// Define the state for DEFENSE
// No states for this mode

/*****************************FUNCTION PROTOTYPES*************************************/
int Get_Auto_Mode( void );
void MORT_autonomous_mode(int mode);
void close_straight(void);
void close_delay(void);
void close_shoot(void);
void middle_right(void);
void defense(void);
void turn_off_everything(void);



