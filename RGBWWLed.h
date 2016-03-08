/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 * @version 0.7
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 * This Library provides Methods for controlling LED RGB, WarmWhite and
 * ColdWhite Led(stripes) via PWM. The Library can either use the standard
 * ARDUINO PWM Implementation or if available the espressif ESP8266 SDK
 * Pwm.
 *
 * The Library can handle different color modes (RGB, RGB+WarmWhite,
 * RGB+ColdWhite, RGB+WarmWhite+ColdWhite) and offeres different calculation
 * models for outputting HSV colors
 *
 * The Library offers various adjustments (i.e. maximum brightness of each
 * channel, adjusting HSV basecolors etc..)
 *
 * It also provides a simple interface for creating Animations and
 * queuing several animations/colors to a complex animation
 */

#ifndef RGBWWLed_h
#define RGBWWLed_h


#define RGBWW_VERSION "0.7"
#define RGBWW_PWMDEPTH 10
#define RGBWW_PWMWIDTH int(pow(2, RGBWW_PWMDEPTH))
#define	RGBWW_PWMMAXVAL int(RGBWW_PWMWIDTH - 1)
#define	RGBWW_PWMHUEWHEELMAX int(RGBWW_PWMMAXVAL * 6)

#define RGBWW_UPDATEFREQUENCY 50
#define RGBWW_MINTIMEDIFF  int(1000 / RGBWW_UPDATEFREQUENCY)
#define RGBWW_ANIMATIONQSIZE 50
#define	RGBWW_WARMWHITEKELVIN 2700
#define RGBWW_COLDWHITEKELVIN 6000

#include <ARDUINO.h>
#ifdef SMING_VERSION
	#define RGBWW_USE_ESP_HWPWM
	#include "../../SmingCore/SmingCore.h"
#endif

#ifndef DEBUG_RGBWW
	#define DEBUG_RGBWW TRUE
#endif

#include "debugUtils.h"
#include "RGBWWLedColor.h"
#include "RGBWWLedAnimation.h"
#include "RGBWWLedOutput.h"



class RGBWWLedAnimation;
class RGBWWLedAnimationQ;
class RGBWWColorUtils;
class PWMOutput;

/**
 *
 */
class RGBWWLed
{
public:

	RGBWWLed();
	~RGBWWLed();

	/**
	 * Initialize the the LED Controller
	 *
	 * @param redPIN	int representing the MC pin for the red channel
	 * @param greenPIN  int representing the MC pin for the green channel
	 * @param bluePIN   int representing the MC pin for the blue channel
	 * @param wwPIN     int representing the MC pin for the warm white channel
	 * @param cwPIN		int representing the MC pin for the cold white channel
	 * @param pwmFrequency (default 200)
	 */
	void init(int redPIN, int greenPIN, int bluePIN, int wwPIN, int cwPIN, int pwmFrequency=200);


	/**
	 * Main function for processing animations/color output
	 * Use this in your loop()
	 *
	 *
	 * @retval true 	not updating
	 * @retval false 	updates applied
	 */
	bool show();


	/**
	 * Refreshs the current output.
	 * Usefull when changing brightness, white or color correction
	 *
	 */
	void refresh();


	/**
	 * Set Output to given HSVK color.
	 * Converts HSVK into seperate color channels (r,g,b,w)
	 * and applies brightness and white correction
	 *
	 * @param HSVK&	outputcolor
	 */
	void setOutput(HSVK& color);


	/**
	 * Sets the output of the Controller to the given RGBWK
	 * while applying brightness and white correction
	 *
	 * @param RGBWK& outputcolor
	 */
	void setOutput(RGBWK& color);


	/**
	 * Directly set the PWM values without color correction or white balance.
	 * Assumes the values are in the range of [0, PWMMAXVAL].
	 *
	 * @param int&	red
	 * @param int&	green
	 * @param int&	blue
	 * @param int&	wwhite
	 * @param int&	cwhite
	 */
	void setOutputRaw(int& red, int& green, int& blue, int& cwhite, int& wwhite);


	/**
	 * Returns an HSVK object representing the current color
	 *
	 * @return HSVK current color
	 */
	HSVK getCurrentColor();


	/**
	 * Output specified HSV color
	 *
	 * @param color HSVK object with color values
	 */
	void setHSV(HSVK& color);


	/**
	 * Fade to specified HSVK color
	 *
	 * @param color 	new color
	 * @param time		duration of transition in ms
	 * @param direction direction of transition (0= long/ 1=short)
	 */
	void setHSV(HSVK& color, int time, int direction);


	/**
	 * Fade to specified HSVK color
	 *
	 * @param color 	new color
	 * @param time		duration of transition in ms
	 * @param queue		directly execute fade or queue it
	 */
	void setHSV(HSVK& color, int time, bool queue);


	/**
	 * Fade to specified HSVK color
	 *
	 * @param color 	new color
	 * @param time		duration of transition in ms
	 * @param direction direction of transition (0= long/ 1=short)
	 * @param queue		directly execute fade or queue it
	 */
	void setHSV(HSVK& color, int time, int direction = 1, bool queue = false);


	/**
	 * Fade from one color (colorFrom) to another color
	 *
	 * @param colorFrom starting color of transition
	 * @param color 	ending color of transition
	 * @param time		duration of transition in ms
	 * @param direction direction of transition (0= long/ 1=short)
	 * @param queue		directly execute fade or queue it
	 */
	void setHSV(HSVK& colorFrom, HSVK& color, int time, int direction = 1, bool q = false);


	/**
	 * Set a function as callback when an animation has finished.
	 *
	 * Example use case would be, to save the current color to flash
	 * after an animation has finished to preserve it after a powerloss
	 *
	 * @param func
	 */
	void setAnimationCallback( void (*func)(RGBWWLed* led) );


	/**
	 * Check if an animation is currently active
	 *
	 * @retval true if an animation is currently active
	 * @retval false if no animation is active
	 */
	bool isAnimationActive();


	/**
	 * Check if the AnimationQueue is full
	 *
	 * @retval true queue is full
	 * @retval false queue is not full
	 */
	bool isAnimationQFull();

	/**
	 * skip the current animation
	 *
	 */
	void skipAnimation();


	/**
	 * Cancel all animations in the queue
	 *
	 */
	void clearAnimationQueue();


	/**
	 * Change the speed of the current running animation
	 *
	 * @param speed
	 */
	void setAnimationSpeed(int speed);


	/**
	 * Change the brightness of the current animation
	 *
	 * @param brightness
	 */
	void setAnimationBrightness(int brightness);

	//colorutils
	RGBWWColorUtils colorutils;


private:
	unsigned long last_active;
	HSVK 	_current_color;
	RGBWK   _current_output;
	bool    _cancelAnimation;
	bool    _clearAnimationQueue;
	bool    _isAnimationActive;

	RGBWWLedAnimation*  _currentAnimation;
	RGBWWLedAnimationQ* _animationQ;
	PWMOutput* _pwm_output;

	void (*_animationcallback)(RGBWWLed* led) = NULL;

	//helpers
	void cleanupCurrentAnimation();
	void cleanupAnimationQ();

};

#endif //RGBWWLed_h
