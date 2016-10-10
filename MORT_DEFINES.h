/*******************************************************************************
* FILE NAME: MORT_DEFINES.h
*
* DESCRIPTION: 
*  This is the include file that defines all I/O used in the MORT 2005 Robot
*  It contains aliases and function prototypes used in all MORT 'C' routines.
*
*******************************************************************************/

#ifndef __MORT_DEFINES_h_
#define __MORT_DEFINES_h_

/*************************************************************************************
			                        MORT DEFINES
*************************************************************************************/

/*******************************USER CONTROL DEFINES*********************************/
// Drive Train Controller
#define LEFT_DRIVE_JOYSTICK		 	p2_y
#define RIGHT_DRIVE_JOYSTICK		p2_x

// Pot Box Controller
#define POTBOX_PAN					p4_x
#define POTBOX_TILT					p4_y
#define POTBOX_COLLECTOR			p4_sw_aux1
#define POTBOX_SHOOTER_WHEEL		p4_sw_trig
#define POTBOX_SHOOTER_BELT			p4_sw_top
#define POTBOX_WHEEL				p4_wheel
#define POTBOX_REVERSE				p4_sw_aux2
#define MANUAL_TOGGLE_SWITCH		p3_sw_aux1	// Toggle switch on pot box, PORT 1

/************************************PWM DEFINES*************************************/
// Drive PWM allocations
#define	LEFT_WHEEL					pwm01			// PWM for the left drive motor
#define LEFT_WHEEL_2				pwm02

#define	RIGHT_WHEEL					pwm03			// PWM for the right drive motor
#define RIGHT_WHEEL_2				pwm04

// End effector PWM allocations 
#define TURRET_PAN	 				pwm05			// PWM for turret pan
#define TURRET_TILT	 				pwm06			// PWM for turret tilt
#define COLLECTOR					pwm07			// PWM for collection motor
#define SHOOTER_BELT				pwm08			// PWM for shooter conveyor
#define SHOOTER_WHEEL				pwm09			// PWM for shooter wheel


/*******************************CONSTANT DEFINES*************************************/
// Speeds
#define TURRET_LEFT_MAX			127 - 71					// If the turret spins to fast, the camera can't keep up
#define TURRET_LEFT_MIN			127 - 7

#define TURRET_RIGHT_MAX		127 + 63					// Motor spins faster one way, so the MAX and MIN values are different
#define TURRET_RIGHT_MIN		127 + 14

#define TURRET_FORWARD_MAX		127 - 15
#define TURRET_FORWARD_MIN		127 - 11

#define TURRET_BACKWARD_MAX		127 + 25
#define TURRET_BACKWARD_MIN		127 + 20

//POT Values
#define PAN_POT_CENTER		516
#define PAN_POT_LEFT 		(PAN_POT_CENTER + 157)		// center + 174
#define PAN_POT_RIGHT 		(PAN_POT_CENTER - 163)		// center - 160
#define TILT_POT_45			870							// hood 60... ball 45
#define TILT_POT_0          (TILT_POT_45 - 162)			// hood 15... ball 0
//#define TILT_POT_FORWARD 	(TILT_POT_45 - 200)			// hood level, limit
//#define TILT_POT_BACKWARD	(TILT_POT_45 - 25)					// hood back, limit

#define TILT_POT_BACKWARD 620
#define TILT_POT_FORWARD  540			

/*******************************SENSOR DEFINES****************************************/
#define OP_SENSOR_1					rc_dig_in07		// system 2 sensor top
#define OP_SENSOR_2					rc_dig_in08		// system 2 sensor bottom
#define OP_SENSOR_3					rc_dig_in09		// system 3 sensor

#define TURRET_PAN_LEFT_SWITCH		rc_ana_in04
#define TURRET_PAN_RIGHT_SWITCH		rc_ana_in05

#define TURRET_PAN_POT				rc_ana_in01
#define TURRET_TILT_POT				rc_ana_in02

/********************************OI FEEDBACK******************************************/
#define Camera_Status_LED			Relay1_red
#define Gravity_Full_LED			Relay1_green
#define Manual_Toggle_LED			Relay2_green


/*****************************FUNCTION PROTOTYPES*************************************/
void Update_OI( void );
int ramp_joystick( int value );
void SoftStartWheels( void );
int Turret_Pan_Position(int servo_position);
int Direction_Change ( void );
void Check_Manual_Toggle ( void );
void Run_Ball_System ( void ); 
void Run_Shooter_Wheel ( void );
void Run_Shooter_Belt ( int feed_pause );
void Run_Systems_Reverse ( void );
void Manual_Aim ( void );
void Limit_Turret ( void );
int Tilt_Servo_Angle ( int Servo_Position );
int Get_Auto_Mode_DEBUG( void );
