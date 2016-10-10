/*******************************************************************************
* FILE NAME: user_routines.c <FRC VERSION>
*
* DESCRIPTION:
*  This file contains the default mappings of inputs  
*  (like switches, joysticks, and buttons) to outputs on the RC.  
*
* USAGE:
*  You can either modify this file to fit your needs, or remove it from your 
*  project and replace it with a modified copy. 
*
*******************************************************************************/

#include <stdio.h>

#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"
#include "user_routines.h"
#include "serial_ports.h"
#include "camera.h"
#include "terminal.h"
#include "MORT_DEFINES.h"
#include "MORT_TIMERS.h"
#include "MORT_TRACKING.h"
#include "math.h"
#include "adc.h"

/*************************************************************************************
                                   MORT VARIABLES                                  
*************************************************************************************/
//DO NOT CHANGE		(Due to joystick configuration this year, you can ONLY use dual joysticks)
unsigned char 			dual_joysticks = 1;					// Set to 1 for dual joystick control

unsigned char 			Accelleration  = 3;					// Set to non ZERO to enable accell limiting, smaller value will slow accell/decell.

int						right_wheel_save = 127;				// Used in SoftStartWheels
int						left_wheel_save = 127;				// Used in SoftStartWheels

int						right_wheel_save_dir = 127;			// Used in Direction_Change
int						left_wheel_save_dir = 127;			// Used in Direction_Change

static unsigned char 	count = 0;							// Used to output displays at different intervals

extern unsigned char	tracking_lock;				// 1 = Locked on target , 0 = Not locked on target (declared in tracking.c)
extern unsigned char	camera_failed;				// Do we have packets? (declared in camera.c)
int 					camera_failed_count = 0;	// Count how many time the camera fails to return a packet

unsigned char	Manual_Toggle_State = 0;			// Toggle manual override
unsigned char	Manual_Toggle_Debounce = 0;			// Debounce so you have to hold the button to override
unsigned char	Manual_Toggle_State_Change = 1;		// Are we allowed to change the state of the toggle?

unsigned char	Collector_Debounce = 0;				// Debounce to make sure our collector is full, and we did not just see a ball
unsigned char	Gravity_Debounce = 0;				// Debounce to make sure the gravity is full, and we did not just see a ball
unsigned char	Feed_Debounce = 0;					// 
unsigned char	Shooter_Debounce = 0;				// Debounce to turn the wheel on first, then turn the belt to feed it

extern long Timer1_Count;


/*** DEFINE USER VARIABLES AND INITIALIZE THEM HERE ***/
/* EXAMPLES: (see MPLAB C18 User's Guide, p.9 for all types)
unsigned char wheel_revolutions = 0; (can vary from 0 to 255)
unsigned int  delay_count = 7;       (can vary from 0 to 65,535)
int           angle_deviation = 142; (can vary from -32,768 to 32,767)
unsigned long very_big_counter = 0;  (can vary from 0 to 4,294,967,295)
*/


/*******************************************************************************
* FUNCTION NAME: Limit_Switch_Max
* PURPOSE:       Sets a PWM value to neutral (127) if it exceeds 127 and the
*                limit switch is on.
* CALLED FROM:   this file
* ARGUMENTS:     
*     Argument       Type             IO   Description
*     --------       -------------    --   -----------
*     switch_state   unsigned char    I    limit switch state
*     *input_value   pointer           O   points to PWM byte value to be limited
* RETURNS:       void
*******************************************************************************/
void Limit_Switch_Max(unsigned char switch_state, unsigned char *input_value)
{
  if (switch_state == CLOSED)
  { 
    if(*input_value > 127)
      *input_value = 127;
  }
}


/*******************************************************************************
* FUNCTION NAME: Limit_Switch_Min
* PURPOSE:       Sets a PWM value to neutral (127) if it's less than 127 and the
*                limit switch is on.
* CALLED FROM:   this file
* ARGUMENTS:     
*     Argument       Type             IO   Description
*     --------       -------------    --   -----------
*     switch_state   unsigned char    I    limit switch state
*     *input_value   pointer           O   points to PWM byte value to be limited
* RETURNS:       void
*******************************************************************************/
void Limit_Switch_Min(unsigned char switch_state, unsigned char *input_value)
{
  if (switch_state == CLOSED)
  { 
    if(*input_value < 127)
      *input_value = 127;
  }
}


/*******************************************************************************
* FUNCTION NAME: Limit_Mix
* PURPOSE:       Limits the mixed value for one joystick drive.
* CALLED FROM:   Default_Routine, this file
* ARGUMENTS:     
*     Argument             Type    IO   Description
*     --------             ----    --   -----------
*     intermediate_value    int    I    
* RETURNS:       unsigned char
*******************************************************************************/
unsigned char Limit_Mix (int intermediate_value)
{
  static int limited_value;
  
  if (intermediate_value < 2000)
  {
    limited_value = 2000;
  }
  else if (intermediate_value > 2254)
  {
    limited_value = 2254;
  }
  else
  {
    limited_value = intermediate_value;
  }
  return (unsigned char) (limited_value - 2000);
}


/*******************************************************************************
* FUNCTION NAME: User_Initialization
* PURPOSE:       This routine is called first (and only once) in the Main function.  
*                You may modify and add to this function.
* CALLED FROM:   main.c
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void User_Initialization (void)
{
  Set_Number_of_Analog_Channels(SIXTEEN_ANALOG);    /* DO NOT CHANGE! */

/* FIRST: Set up the I/O pins you want to use as digital INPUTS. */
  digital_io_01 = digital_io_02 = digital_io_03 = digital_io_04 = INPUT;
  digital_io_05 = digital_io_06 = digital_io_07 = digital_io_08 = INPUT;
  digital_io_09 = digital_io_10 = digital_io_11 = digital_io_12 = INPUT;
  digital_io_13 = digital_io_14 = digital_io_15 = digital_io_16 = INPUT;
  digital_io_18 = INPUT;  /* Used for pneumatic pressure switch. */
    /* 
     Note: digital_io_01 = digital_io_02 = ... digital_io_04 = INPUT; 
           is the same as the following:

           digital_io_01 = INPUT;
           digital_io_02 = INPUT;
           ...
           digital_io_04 = INPUT;
    */

/* SECOND: Set up the I/O pins you want to use as digital OUTPUTS. */
  digital_io_17 = OUTPUT;    /* Example - Not used in Default Code. */

/* THIRD: Initialize the values on the digital outputs. */
  rc_dig_out17 = 0;

/* FOURTH: Set your initial PWM values.  Neutral is 127. */
  pwm01 = pwm02 = pwm03 = pwm04 = pwm05 = pwm06 = pwm07 = pwm08 = 127;
  pwm09 = pwm10 = pwm11 = pwm12 = pwm13 = pwm14 = pwm15 = pwm16 = 127;

/* FIFTH: Set your PWM output types for PWM OUTPUTS 13-16.
  /*   Choose from these parameters for PWM 13-16 respectively:               */
  /*     IFI_PWM  - Standard IFI PWM output generated with Generate_Pwms(...) */
  /*     USER_CCP - User can use PWM pin as digital I/O or CCP pin.           */
  Setup_PWM_Output_Type(IFI_PWM,IFI_PWM,IFI_PWM,IFI_PWM);

  /* 
     Example: The following would generate a 40KHz PWM with a 50% duty cycle on the CCP2 pin:

         CCP2CON = 0x3C;
         PR2 = 0xF9;
         CCPR2L = 0x7F;
         T2CON = 0;
         T2CONbits.TMR2ON = 1;

         Setup_PWM_Output_Type(USER_CCP,IFI_PWM,IFI_PWM,IFI_PWM);
  */

  /* Add any other initialization code here. */
 
	Init_Timers();


  	Init_Serial_Port_One();
  	Init_Serial_Port_Two();

  	Initialize_ADC();
  	Restart_Camera();

#ifdef TERMINAL_SERIAL_PORT_1    
  stdout_serial_port = SERIAL_PORT_ONE;
#endif

#ifdef TERMINAL_SERIAL_PORT_2    
  stdout_serial_port = SERIAL_PORT_TWO;
#endif

  Putdata(&txdata);            /* DO NOT CHANGE! */

//  ***  IFI Code Starts Here***
//
//  Serial_Driver_Initialize();
//
//  printf("IFI 2006 User Processor Initialized ...\r");  /* Optional - Print initialization message. */

  User_Proc_Is_Ready();         /* DO NOT CHANGE! - last line of User_Initialization */
}

/*******************************************************************************
* FUNCTION NAME: Process_Data_From_Master_uP
* PURPOSE:       Executes every 26.2ms when it gets new data from the master 
*                microprocessor.
* CALLED FROM:   main.c
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void Process_Data_From_Master_uP(void)
{

	Getdata(&rxdata);

	// This function is responsable for camera initialization 
	// and camera serial data interpretation. Once the camera
	// is initialized and starts sending tracking data, this 
	// function will continuously update the global T_Packet_Data 
	// structure with the received tracking information.
	Camera_Handler();

	
	Check_Manual_Toggle();				// Checks the toggle button, and sets Manual_Toggle_State to ON or OFF

	SHOOTER_BELT = 127;					// The shooter belt is normally off
	
	Run_Ball_System();					// Check sensors and take care of auto loading and checking joystick to collect balls

	if (Manual_Toggle_State)			// If we're in manual override..
	{
		Manual_Aim();					// Allow user to move the turret using POTBOX
	}
	else
	{
		Turret_Track();					// Aim the turret through code
	}


	if (POTBOX_SHOOTER_WHEEL)			// If we want to ramp up the wheel
	{
		Run_Shooter_Wheel();			// Start running the wheel when the POTBOX_SHOOTER_WHEEL is held

		if (POTBOX_SHOOTER_BELT)		// If we want to shoot balls
		{
			Run_Shooter_Belt(15);		// Pulse the shooter belt
		}
	}
	else								// Else, if we do not want to ramp up wheel...
	{
		Shooter_Debounce = 0;			// Reset the debounce if we're not shooting
		SHOOTER_WHEEL = 127;			// Turn the wheel off
	}
	
	if (POTBOX_REVERSE)					// If we need to run the ball system in reverse
		Run_Systems_Reverse();


	if (dual_joysticks == 1)										// If using dual joysticks
	{
		LEFT_WHEEL = ramp_joystick(255 - LEFT_DRIVE_JOYSTICK);		// Ramp the joystick and output to LEFT wheel
		RIGHT_WHEEL = ramp_joystick(255 - RIGHT_DRIVE_JOYSTICK);	// Ramp the joystick and output to RIGHT wheel
	}
	else
	{
		LEFT_WHEEL = 255 - (Limit_Mix(2000 + p1_y + p1_x - 127));
		RIGHT_WHEEL = Limit_Mix(2000 + p1_y - p1_x + 127);	
	}

	if (LEFT_DRIVE_JOYSTICK < 132 && LEFT_DRIVE_JOYSTICK > 122)
		LEFT_DRIVE_JOYSTICK = 127;
	if (RIGHT_DRIVE_JOYSTICK < 132 && RIGHT_DRIVE_JOYSTICK > 122)
		RIGHT_DRIVE_JOYSTICK = 127;

	if (Accelleration != 0)					// Only limit accell if were changing direction 
		SoftStartWheels( );
	
	if (RIGHT_DRIVE_JOYSTICK == 127)
		RIGHT_WHEEL = 127;
	if (LEFT_DRIVE_JOYSTICK == 127)
		LEFT_WHEEL = 127;


//	printf("TURRET_TILT: %d \r\n", TURRET_TILT);
//	printf("TURRET_PAN: %d \r\n", TURRET_PAN);

//	printf("Confidence: %d \r\n", T_Packet_Data.confidence);
//	printf("PAN: %d ", POTBOX_PAN);
//	printf("TILT: %d \r\n", POTBOX_TILT);
//	printf("TILT: %d \r\n", Get_Analog_Value(TURRET_TILT_POT));
//  printf("joystick: %d \r\n", POTBOX_TILT);
//	printf("offset : %d\r\n", turret_offset);
//	printf("inputs: %d %d %d %d %d %d %d %d \r\n", rc_dig_in01, rc_dig_in02, rc_dig_in03, rc_dig_in04, rc_dig_in05, rc_dig_in06, rc_dig_in10, rc_dig_in11);
//	printf("sensor: %d \r\n", Get_Auto_Mode_DEBUG());
//	if (Timer1_Count >= 40)
//	{
  //		printf("timer: %d r\n", Timer1_Count);
	//	Timer1_Count = 0;
//	}

//	printf("inputs: %d \r\n", TURRET_PAN_RIGHT_SWITCH);
//	printf("limits: %d \r\n", Get_Analog_Value(rc_ana_in03));
//	printf("turret pan: %d \r\n", TURRET_PAN);


	Update_OI();

    RIGHT_WHEEL = 254 - RIGHT_WHEEL;
	LEFT_WHEEL_2 = LEFT_WHEEL;
	RIGHT_WHEEL_2 = RIGHT_WHEEL;

//	printf("MOTOR: %d \r\n", TURRET_PAN);

	Limit_Turret();
	//TURRET_PAN = 254-TURRET_PAN;

	Putdata(&txdata);
}

/*******************************************************************************
* FUNCTION NAME: Update_OI
* PURPOSE:       Updates the Operator Interface LEDS and displays Robot status
* ARGUMENTS:     none
* RETURNS:       void
*******************************************************************************/
void Update_OI( void )
{
	count++;							// Increment loop counter

	if (count >= 15)					// Every 15 loops update state of lights
	{
		if (! tracking_lock )			// If were not locked onto the target..
		{
			if (Camera_Status_LED)		// Change light to opposite state
				Camera_Status_LED = 0;
			else
				Camera_Status_LED = 1;
		}
		else
		{	
			Camera_Status_LED = 1;		// If we are tracking keep light solid
		}

    	count = 0;						// reset the loop counter
	}


	if (camera_failed)					// If the camera got no packets ( camera.c )
	{
		camera_failed_count += 1;		// Increase the camera failed count
	}
	else
	{
		camera_failed_count = 0;		// If the camera did get a packet, that means were still getting data so reset the failed count
	}

	if (camera_failed_count >= 15)		// If camera failed 15 times in a row
	{
		Camera_Status_LED = 0;			// Turn off  red light indicating the camera has failed
		camera_failed_count = 15;		// Dont let it overflow
	}
		

	if (Manual_Toggle_State)			// Display the state of the manual toggle switch
		Manual_Toggle_LED = 1;
	else
		Manual_Toggle_LED = 0;

	if (Collector_Debounce >= 15)		// If the collector has been full for more then 15 loops, its full...
		Gravity_Full_LED = 1;			// Turn the light on
	else								// If it't not full...
		Gravity_Full_LED = 0;			// Turn the light off
}


int ramp_joystick(int value)
{
	if (value < 127 )
		return sqrt(127)*sqrt(value);
	
	if (value > 127 )
		return -sqrt(127)*sqrt(-value + 255) + 255;
	
	if (value == 127 )
		return 127;	
}

void SoftStartWheels( void )
{
	// Check for accell limit;
	if (LEFT_WHEEL > left_wheel_save) 						// If we are accellerating
	{
		if (LEFT_WHEEL > (left_wheel_save+Accelleration)) 	// If we must limit accell
			left_wheel_save+=Accelleration;					// Step to next higher speed
		else
			left_wheel_save = LEFT_WHEEL;
	}

	// Check for decell limit
	if (LEFT_WHEEL < left_wheel_save) 						// If we are decellerating
	{
		if (LEFT_WHEEL < (left_wheel_save-Accelleration)) 	// If we must limit decell
			left_wheel_save-=Accelleration;					// Step to next lower speed
		else
			left_wheel_save = LEFT_WHEEL;
	}

	// Check for accell limit
	if (RIGHT_WHEEL > right_wheel_save) 						// If we are accellerating
	{
		if (RIGHT_WHEEL > (right_wheel_save+Accelleration)) 	// If we must limit accell
			right_wheel_save+=Accelleration;					// Step to next higher speed
		else
			right_wheel_save = RIGHT_WHEEL;
	}

	// Check for decell limit
	if (RIGHT_WHEEL < right_wheel_save) 						// If we are decellerating
	{
		if (RIGHT_WHEEL < (right_wheel_save-Accelleration)) 	// If we must limit decell
			right_wheel_save-=Accelleration;					// Step to next lower speed
		else
			right_wheel_save = RIGHT_WHEEL;
	}

	LEFT_WHEEL  = left_wheel_save;
	RIGHT_WHEEL = right_wheel_save;
}

int Direction_Change ( void )
{
	int direction = 0;
	int direction_flag = 0;
	if ( left_wheel_save_dir > 127 && LEFT_DRIVE_JOYSTICK < 127 )
		direction_flag = 1;
	else if ( left_wheel_save_dir < 127 && LEFT_DRIVE_JOYSTICK > 127 )
		direction_flag = 1;
	if ( right_wheel_save_dir > 127 && RIGHT_DRIVE_JOYSTICK < 127 )
		direction_flag = 1;
	else if ( right_wheel_save_dir < 127 && RIGHT_DRIVE_JOYSTICK > 127 )
		direction_flag = 1;

	if (direction_flag)
		direction = 1;
	else
		direction = 0;

	if ( LEFT_DRIVE_JOYSTICK == LEFT_WHEEL && RIGHT_DRIVE_JOYSTICK == RIGHT_WHEEL)
		direction_flag = 0;
	
	left_wheel_save_dir = LEFT_WHEEL;
	right_wheel_save_dir = RIGHT_WHEEL;
	
	return direction;
}

void Check_Manual_Toggle ( void )
{
	if (MANUAL_TOGGLE_SWITCH)						// If we hit the toggle switch
	{
		if (Manual_Toggle_State_Change == 1)		// If we can toggle the ON / OFF state...
		{
			if (Manual_Toggle_State == 0)			// If we were previously OFF
			{
				Manual_Toggle_Debounce += 1;		// Increase the debounce so you have to hold it
		
				if (Manual_Toggle_Debounce >= 5)	// If we have held the button for 10 loops
				{
					Manual_Toggle_State = 1;		// Turn the toggle state to on
					Manual_Toggle_State_Change = 0;	// Dont allow state change until the button is released
				}
				else
				{
					Manual_Toggle_State = 0;		// If we haven't held the button long enough, manual is still off
				}
			}
			else									// If we were previously ON
			{
				Manual_Toggle_State = 0;			// We are now OFF
				Manual_Toggle_State_Change = 0;		// Don't allow change until you release the button
			}
		}
	}
	else
	{
		Manual_Toggle_Debounce = 0;					// If the switch is not pressed, we reset the debounce
		Manual_Toggle_State_Change = 1;				// Now that the button is released, we can toggle the state
	}

}

void Run_Ball_System( void )
{
	if (OP_SENSOR_1)					// If the top of the gravity sensor goes off (COLLECTOR SENSOR)
		Collector_Debounce++;			// Incease the debounce
	else								// If the top of the gravity sensor is idle
		Collector_Debounce = 0;			// Reset the debounce because there are no balls there

	if (Collector_Debounce >= 15)		// If a ball has been sitting there for more then 15 program loops
	{	
		COLLECTOR = 127;				// Turn the collector off
		Collector_Debounce = 15;		// Keep debounce from overflowing
	}
	else								// If the collector debounce is less then five...
	{									// Allow user to control collector
		if(POTBOX_COLLECTOR)			// If the colllector button was pressed
		{
			if (COLLECTOR > 10)			// Ramp up collector until its full speed ("0")
				COLLECTOR -= 10;
			else
				COLLECTOR = 0;			// Turn the collector on
	
		}
		else							// If the button is not pressed
			COLLECTOR = 127;			// Turn the collector off
	}
	

	if (OP_SENSOR_2)					// If the lower gravity feed sensor goes off
		Gravity_Debounce++;				// Increase the debounce to make sure a ball is sitting there

	if (Gravity_Debounce >= 9)			// If a ball is sitting there for more then 9 program loops
	{
		Gravity_Debounce = 9;			// Keep debounce from overflowing
		if (OP_SENSOR_3)				// If the third sensor is off (meaning balls ARE there)
		{
			SHOOTER_BELT = 127;			// Stop the belt
			Gravity_Debounce = 0;		// Reset the debounce
		}
		else							// If there is room to move balls up...
			SHOOTER_BELT = 200;			// Turn to belt on
	}
	else								// If there is no ball there for more then 9 loops
	{
		if (OP_SENSOR_3)				// If balls are queued up			
			Gravity_Debounce = 0;		// Keep debounce from going up
	}

}
void Run_Shooter_Wheel ( void )
{
	if (SHOOTER_WHEEL < 235)
		SHOOTER_WHEEL += 20;
	else
		SHOOTER_WHEEL = 255;
}

void Run_Shooter_Belt ( int feed_pause )
{
	if (SHOOTER_WHEEL >= 190)
	{
		Shooter_Debounce++;
		Feed_Debounce++;
		if (Shooter_Debounce >= 80)
		{
			Shooter_Debounce = 80;
	
			if (Feed_Debounce <= 10)
				SHOOTER_BELT = 255;
			else if (Feed_Debounce > 10 && Feed_Debounce <= (feed_pause + 10))
				SHOOTER_BELT = 127;
			else if (Feed_Debounce > (feed_pause + 10))
				Feed_Debounce = 0;
		}
	}
}


void Run_Systems_Reverse ( void )
{
	SHOOTER_BELT = 40;
	
	if (SHOOTER_WHEEL >= 110)
		SHOOTER_WHEEL -= 10;
	else
		SHOOTER_WHEEL = 100;

	if (COLLECTOR <= 245)			// Ramp up collector until its full speed ("0")
		COLLECTOR += 10;
	else
		COLLECTOR = 255;			// Turn the collector on	
}

void Manual_Aim ( void )
{
		TURRET_PAN = POTBOX_PAN;						// Output the joystick right out to the turret pan
		TURRET_TILT = 254 - POTBOX_TILT;				// Reverse the joystick so you pull back to tilt up
		
		if (POTBOX_SHOOTER_WHEEL && POTBOX_SHOOTER_BELT)								// Kills joystick while shooting in manual
		{
			if (POTBOX_PAN <= 10)
				TURRET_PAN = TURRET_RIGHT_MAX;
			else if (POTBOX_PAN >= 245)
				TURRET_PAN = TURRET_LEFT_MAX;
			else
				TURRET_PAN = 127;

			if (POTBOX_TILT >= 245)
				TURRET_TILT = TURRET_FORWARD_MAX;
			else if (POTBOX_TILT <= 10)
				TURRET_TILT = TURRET_BACKWARD_MAX;
			else
				TURRET_TILT = 127;
		}

/******** testing code ****************************/

		if (POTBOX_PAN < 132 && POTBOX_PAN > 122)
			TURRET_PAN = 127;

		if (POTBOX_TILT < 130 && POTBOX_TILT > 124)
			TURRET_TILT = 127;

		TURRET_PAN = 254 - TURRET_PAN;
}

void Limit_Turret (void)
{

	// Limit the tilt
	if (Get_Analog_Value(TURRET_TILT_POT) >= TILT_POT_BACKWARD)
	{
			if (TURRET_TILT > 127)
				TURRET_TILT = 127;
	}		

	if (Get_Analog_Value(TURRET_TILT_POT) <= TILT_POT_FORWARD)
	{
			if (TURRET_TILT < 127)
				TURRET_TILT = 127;
	}


	// Limit the pan
	if (Get_Analog_Value(TURRET_PAN_LEFT_SWITCH) > 1022)
	{
		if (TURRET_PAN < 127)
			TURRET_PAN = 127;
	}

	if (Get_Analog_Value(TURRET_PAN_RIGHT_SWITCH) > 1022)
	{
		if (TURRET_PAN > 127)
			TURRET_PAN = 127;
	}


	// Limit tilt speed
	if (TURRET_TILT >= TURRET_BACKWARD_MAX)
		TURRET_TILT = TURRET_BACKWARD_MAX;
	else if (TURRET_TILT <= TURRET_FORWARD_MAX)
		TURRET_TILT = TURRET_FORWARD_MAX;
	
	if (TURRET_PAN <= TURRET_LEFT_MAX)
		TURRET_PAN = TURRET_LEFT_MAX;
	else if (TURRET_PAN >= TURRET_RIGHT_MAX)
		TURRET_PAN = TURRET_RIGHT_MAX;

/*	if (TURRET_TILT <= TURRET_BACKWARD_MIN && TURRET_TILT > 127)
		TURRET_TILT = TURRET_BACKWARD_MIN;
	else if (TURRET_TILT >= TURRET_FORWARD_MIN && TURRET_TILT < 127)
		TURRET_TILT = TURRET_FORWARD_MIN;
	
	if (TURRET_PAN >= TURRET_LEFT_MIN && TURRET_PAN < 127)
		TURRET_PAN = TURRET_LEFT_MIN;
	else if (TURRET_PAN <= TURRET_RIGHT_MIN && TURRET_PAN > 127)
		TURRET_PAN = TURRET_RIGHT_MIN;
*/


	//printf("RIGHT: %d LEFT: %d \r\n", Get_Analog_Value(TURRET_PAN_RIGHT_SWITCH), Get_Analog_Value(TURRET_PAN_LEFT_SWITCH));

}

int Get_Auto_Mode_DEBUG(void)
{
	if (rc_dig_in01 == 0)
		return 1;
	else if (rc_dig_in02 == 0)
		return 2;
	else if (rc_dig_in03 == 0)
		return 3;
	else if (rc_dig_in04 == 0)
		return 4;
	else if (rc_dig_in05 == 0)
		return 5;
	else if (rc_dig_in06 == 0)
		return 6;
	else
		return 0;
}


/* END Default_Routine(); */


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
