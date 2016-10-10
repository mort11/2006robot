#include <stdio.h>
#include "ifi_default.h"
#include "ifi_aliases.h"
#include "camera.h"
#include "MORT_TRACKING.h"
#include "MORT_DEFINES.h"

unsigned char tracking_lock_pan = 0;
unsigned char tracking_lock_tilt = 0;
unsigned char tracking_lock = 0;

void Turret_Track(void)
{
	static unsigned int old_camera_t_packets = 0;
	static unsigned char loop_count = 0;
	int pan_error;
	int tilt_error;
	int pan_step;
	int tilt_step;

	// Has a new camera t-packet arrived since we last checked?
	if (camera_t_packets != old_camera_t_packets)
	{
		old_camera_t_packets = camera_t_packets;

		// Does the camera have a tracking solution?
		if (T_Packet_Data.confidence >= CONFIDENCE_THRESHOLD_DEFAULT)
		{

			///////////////////////////////
			//							 //
			// x-axes/tilt tracking code //
			//                           //
			///////////////////////////////

			// Calculate how many image pixels we're away from the vertical center line
			pan_error = (int)T_Packet_Data.mx - PAN_TARGET_PIXEL_DEFAULT;

			//printf("pan_error: %d \r\n", pan_error);

			// Are we too far to the left or right of the vertical center line?
			if (pan_error > PAN_ALLOWABLE_ERROR_DEFAULT )
			{

				if (pan_error > 35)
				{
					TURRET_PAN = TURRET_RIGHT_MAX;
					tracking_lock_pan = 0;	
				}
				else
				{
					TURRET_PAN = TURRET_RIGHT_MIN;
					tracking_lock_pan = 0;
				}

			}
			else if (pan_error < -1 * PAN_ALLOWABLE_ERROR_DEFAULT)
			{

				if (pan_error < -35)
				{
					TURRET_PAN = TURRET_LEFT_MAX;
					tracking_lock_pan = 0;
				}
				else
				{
					TURRET_PAN = TURRET_LEFT_MIN;
					tracking_lock_pan = 0;
				}

				
			}	
			else
			{
				// We need no more adjustment on the pan
				TURRET_PAN = 127;
				tracking_lock_pan = 1;
			}

			///////////////////////////////
			//							 //
			// y-axes/tilt tracking code //
			//                           //
			///////////////////////////////

			// Calculate how many image pixels we're away from the horizontal center line
			tilt_error = (int)T_Packet_Data.my - TILT_TARGET_PIXEL_DEFAULT;
			
//			printf("tilt error: %d \r\n", tilt_error);

			if (tilt_error > TILT_ALLOWABLE_ERROR_DEFAULT)
			{
	
				TURRET_TILT = TURRET_FORWARD_MIN;
				tracking_lock_tilt = 0;
	
			}
			else if (tilt_error < -1 * TILT_ALLOWABLE_ERROR_DEFAULT)
			{
	
				TURRET_TILT = TURRET_BACKWARD_MIN;
				tracking_lock_tilt = 0;
	
			}
			else
			{
				TURRET_TILT = 127;
				tracking_lock_tilt = 1;
			}

			Limit_Turret();

			if (tracking_lock_tilt && tracking_lock_pan)
				tracking_lock = 1;
			else
				tracking_lock = 0;
		}	
		else
		{	
			// No target in site
			tracking_lock = 0;
			TURRET_TILT = 127;
			TURRET_PAN = 127;
		}
	}
	else
	{
	//	TURRET_TILT = 127;
	//	TURRET_PAN = 127;
	}
}
