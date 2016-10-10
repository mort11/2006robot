#ifndef _MORT_TRACKING_H
#define _MORT_TRACKING_H


// This defines the minimum t-packet confidence value needed
// before the tracking software will transition from search
// mode to tracking mode.
#define CONFIDENCE_THRESHOLD_DEFAULT 20

// These values define how quickly the camera will attempt 
// to track the object. If these are set too high, the camera 
// will take longer to settle, too low and the camera will
// overshoot the target and oscillate.
#define PAN_GAIN_DEFAULT 3
#define TILT_GAIN_DEFAULT 8

// These two values define the image pixel that we're
// going to try to keep the tracked object on. By default
// the center of the image is used.
#define PAN_TARGET_PIXEL_DEFAULT  98   	 	//79 looks right of target
#define TILT_TARGET_PIXEL_DEFAULT 119

// These values define how much error, in pixels, is 
// allowable when trying to keep the center of the tracked
// object on the center pixel of the camera's imager. Too 
// high a value and your pointing accuracy will suffer, too
// low and your camera may oscillate because the servos 
// don't have enough pointing resolution to get the center 
// of the tracked object into the square/rectangle defined 
// by these values 
#define PAN_ALLOWABLE_ERROR_DEFAULT  8
#define TILT_ALLOWABLE_ERROR_DEFAULT 14				// (3-23-06) increased by 2 to try to prevent oscillation 

// parameters for CMUcam2 with OV7620 camera module
#define IMAGE_WIDTH 159
#define IMAGE_HEIGHT 239

void Turret_Track(void);
