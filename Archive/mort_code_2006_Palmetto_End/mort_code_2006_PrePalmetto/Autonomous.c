/*******************************************************************************
* FILE NAME: autonomous.c
*
* DESCRIPTION:
* This file contains autonomous mode functions
*
*******************************************************************************/
#include <stdio.h>
#include "ifi_aliases.h"
#include "ifi_default.h"
#include "ifi_utilities.h"
#include "user_routines.h"
#include "camera.h"
#include "MORT_TRACKING.h"
#include "MORT_AUTONOMOUS.h"
#include "MORT_TIMERS.h"

/*************************************************************************************
                                   MORT VARIABLES                                  
*************************************************************************************/
int auton_state;
unsigned char first_run = 1;
extern unsigned char tracking_lock;
extern unsigned char tracking_lock_pan;
extern unsigned char Collector_Debounce;
extern unsigned char Gravity_Debounce;
extern unsigned char Shooter_Debounce;
extern unsigned char Manual_Toggle_State;
extern unsigned char Feed_Debounce;

int counter = 0;


int Get_Auto_Mode(void)
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

void MORT_autonomous_mode( int mode )
{

	switch (mode)
	{
		case CLOSE_STRAIGHT:
			if (first_run)
			{
				counter = 0;
				first_run = 0;
			}
			close_straight();
			break;

		case CLOSE_SHOOT:
			close_shoot();
			break;

		case CLOSE_DELAY:
			if (first_run)
			{
				counter = 0;
				first_run = 0;
				auton_state = CLOSE_DELAY_WAIT;
			}
			close_delay();
			break;

		case MIDDLE_RIGHT:
			if (first_run)
			{
				counter = 0;
				first_run = 0;
				auton_state = MIDDLE_RIGHT_OUT;
			}
			middle_right();
			break;
		
		case DEFENSE:
			if (first_run)
			{
				counter = 0;
				first_run = 0;
			}
			defense();
			break;

		default:
			turn_off_everything();
			break;
	}

	Limit_Turret();

}


//********************************************************************************************************
//							AUTONOMOUS MODE STATE MACHINE	
//********************************************************************************************************

void close_straight(void)
{
	Camera_Handler();	// We always want to get new data from camera
	Turret_Track();
	Run_Ball_System();
	Run_Shooter_Wheel();

	p1_y = 200;
	p1_x = 127;
	counter++;
	
	if (counter >= 15 && counter < 40)
		Run_Shooter_Belt(15);
	else if (counter >= 40)
	{
		counter = 40;
		COLLECTOR = 0;
		p1_y = 127;
		p1_x = 127;
		Run_Shooter_Belt(15);
	}
}

void close_delay(void)
{
	Camera_Handler();
	Turret_Track();
	Run_Ball_System();
	Run_Shooter_Wheel();

	switch (auton_state)
	{
		case CLOSE_DELAY_WAIT:
			p1_y = 127;
			p1_x = 127;
			
			counter++;
			if (counter == 50)
			{
				counter = 0;
				auton_state = CLOSE_DELAY_SHOOT;
			}
			
			break;


		case CLOSE_DELAY_SHOOT:
			p1_y = 200;
			p1_x = 127;

			counter++;			
			if (counter >= 15 && counter < 40)
				Run_Shooter_Belt(15);
			else if (counter >= 40)
			{
				counter = 40;
				COLLECTOR = 0;
				p1_y = 127;
				p1_x = 127;
				Run_Shooter_Belt(15);
			}

			break;
	}
}

void close_shoot( void )
{
	Run_Ball_System();
	Run_Shooter_Wheel();
	Camera_Handler();
	Turret_Track();
				
	// The following code will over-write the turret_tilt set in turret_track()
	// Pull the tilt all the way back
	if (Get_Analog_Value(TURRET_TILT_POT) < TILT_POT_BACKWARD)
	{
		TURRET_TILT = TURRET_BACKWARD_MAX;			// Turret Forward
	}
	else
	{
		TURRET_TILT = 127;
		
		if (tracking_lock_pan)
		{
			Run_Shooter_Belt(20);
		}
	}	
}

			
void middle_right( void )
{

	Camera_Handler();				// We always want the camera to have data
	Turret_Track();
	Run_Ball_System();
	Run_Shooter_Wheel();

	switch (auton_state)
	{
		
		case MIDDLE_RIGHT_OUT:

			p1_x = 127;
			p1_y = 200;
			counter++;

			if (counter >= 35)
			{
				counter = 0;
				auton_state = MIDDLE_RIGHT_DELAY;
			}

			break;

		case MIDDLE_RIGHT_DELAY:

			p1_x = 127;
			p1_y = 127;
			counter++;

			if (counter >= 5)
			{	
				counter = 0;
				auton_state = MIDDLE_RIGHT_TURN;
			}

			break;

		case MIDDLE_RIGHT_TURN:

			p1_x = 200;
			p1_y = 127;
			counter++;

			if (counter >= 25)
			{
				counter = 0;
				auton_state = MIDDLE_RIGHT_DELAY_TWO;
			}

			break;

		case MIDDLE_RIGHT_DELAY_TWO:

			p1_x = 127;
			p1_y = 127;

			counter++;
			if (counter >= 5)
			{	
				counter = 0;
				auton_state = MIDDLE_RIGHT_APPROACH;
			}

			break;

		case MIDDLE_RIGHT_APPROACH:

			p1_x = 127;
			p1_y = 200;
			counter++;

			if (counter >= 15 && counter < 35)
				Run_Shooter_Belt(15);
			else if (counter >= 35)
			{
				counter = 0;
				Run_Shooter_Belt(15);
				auton_state = MIDDLE_RIGHT_SHOOT;
			}
			
			break;
	
		case MIDDLE_RIGHT_SHOOT:
			
			p1_x = 127;
			p1_y = 127;
			Run_Shooter_Belt(15);
			COLLECTOR = 0;
		
			break;
	}
}

void defense(void)
{
	p1_x = 127;
	p1_y = 200;

	counter++;
	if (counter >= 55)
	{
		counter = 55;
		p1_x = 127;
		p1_y = 127;
	}
}

void turn_off_everything( void )
{
	p1_x = 127;
	p1_y = 127;

	LEFT_WHEEL = 127;
	LEFT_WHEEL_2 = 127;

	RIGHT_WHEEL = 127;
	RIGHT_WHEEL_2 = 127;

	TURRET_PAN = 127;
	TURRET_TILT = 127;

	COLLECTOR = 127;

	SHOOTER_BELT = 127;
	SHOOTER_WHEEL = 127;
}	
