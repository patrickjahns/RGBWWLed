/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 * @version 0.6
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
 *
 *
 */

#ifndef RGBWWLed_h
#define RGBWWLed_h


#define RGBWW_VERSION "0.6"
#define RGBWW_PWMDEPTH 10
#define RGBWW_PWMWIDTH int(pow(2, RGBWW_PWMDEPTH)) //1024
#define	RGBWW_PWMMAXVAL int(RGBWW_PWMWIDTH - 1) //1023
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

class RGBWWLed
{
public:
	//init & settings
	RGBWWLed();
	virtual	~RGBWWLed();

	/**
	 * Initialize the the LED Controller
	 *
	 * @param redPIN
	 * @param greenPIN
	 * @param bluePIN
	 * @param wwPIN
	 * @param cwPIN
	 * @param pwmFrequency (default 200)
	 */
	void init(int redPIN, int greenPIN, int bluePIN, int wwPIN, int cwPIN, int pwmFrequency=200);


	//output related
	/**
	 * Main function for processing animations/color output
	 * Use this in your loop()
	 *
	 *
	 * @return BOOL
	 * @retval TRUE 	not updating
	 * @retval FALSE 	updates applied
	 */
	bool show();


	/**
	 * Refreshs the current output
	 * Usefull when changing brightness, white or color correction
	 *
	 */
	void refresh();


	/**
	 * Set Output to given HSVK color
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
	 * @param RGBWK&	outputcolor
	 */
	void setOutput(RGBWK& color);


	/**
	 * Directly set the PWM values without color correction or white balance
	 * Assumes the values are in the range of [0, 1023] or äquivalent if you change
	 * the value range
	 *
	 * @param int&	red
	 * @param int&	green
	 * @param int&	blue
	 * @param int&	wwhite
	 * @param int&	cwhite
	 */
	void setOutputRaw(int& red, int& green, int& blue, int& cwhite, int& wwhite);


	/**
	 * Returns the current color as HSVK
	 *
	 * @return HSVK
	 */
	HSVK getCurrentColor();

	//animation related
	void setHSV(HSVK& color);
	void setHSV(HSVK& color, int time, int direction);
	void setHSV(HSVK& color, int time, bool q);
	void setHSV(HSVK& color, int time, int direction=1, bool q=false);
	void setHSV(HSVK& colorFrom, HSVK& color, int time, int direction=1, bool q=false);


	/**
	 * Function to call after an animation has finished
	 *
	 * @param func
	 */
	void setAnimationCallback( void (*func)(RGBWWLed* led) );


	/**
	 * Check if an animation is currently active
	 * @return
	 * @retval true if active
	 */
	bool isAnimationActive();


	/**
	 * Check if the AnimationQ is full
	 * @return
	 */
	bool isAnimationQFull();

	/**
	 * skip the current Animation
	 */
	void skipAnimation();


	/**
	 * Cancel all following Animations
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
	 * @param brightness
	 */
	void setAnimationBrightness(int brightness);

	//colorutils
	RGBWWColorUtils* colorutils;


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
