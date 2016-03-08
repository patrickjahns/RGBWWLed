/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
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
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"
#include "RGBWWLedAnimation.h"
#include "RGBWWLedOutput.h"



/**************************************************************
 *                setup, init and settings
 **************************************************************/

RGBWWLed::RGBWWLed() {
	_isAnimationActive = false;
	_cancelAnimation = false;
	_clearAnimationQueue = false;
	_current_color = HSVK(0, 0, 0);
	_currentAnimation = NULL;
	_animationQ = new RGBWWLedAnimationQ(RGBWW_ANIMATIONQSIZE);
	colorutils = new RGBWWColorUtils();
	_pwm_output = NULL;

}

RGBWWLed::~RGBWWLed() {
	delete colorutils;
	delete _animationQ;
	if (_currentAnimation != NULL) {
		delete _currentAnimation;
	}
	if (_pwm_output != NULL) {
		delete _pwm_output;
	}

}

void RGBWWLed::init(int redPIN, int greenPIN, int bluePIN, int wwPIN, int cwPIN, int pwmFrequency /* =200 */) {

	_pwm_output = new PWMOutput(redPIN, greenPIN, bluePIN, wwPIN, cwPIN, pwmFrequency);

}



/**************************************************************
 *                     OUTPUT
 **************************************************************/

void RGBWWLed::refresh() {
	setOutput(getCurrentColor());
}
 
 
HSVK RGBWWLed::getCurrentColor() {
	return _current_color;
}

void RGBWWLed::setOutput(HSVK color) {
	RGBWK rgbw;
	_current_color = color;
	colorutils->HSVtoRGB(color, rgbw);
	setOutput(rgbw);

}

void RGBWWLed::setOutput(RGBWK c) {
	int color[5];
	int ww, cw;
	colorutils->whiteBalance(c, ww, cw);
	color[RGBWW_COLORS::RED] = c.r;
	color[RGBWW_COLORS::GREEN] = c.g;
	color[RGBWW_COLORS::BLUE] = c.b;
	color[RGBWW_COLORS::WW] = ww;
	color[RGBWW_COLORS::CW] = cw;
	setOutputRaw(color[RGBWW_COLORS::RED],
			color[RGBWW_COLORS::GREEN],
			color[RGBWW_COLORS::BLUE],
			color[RGBWW_COLORS::WW],
			color[RGBWW_COLORS::CW]);
}



/**
    Directly set the PWM values without color correction or white balance

 */
void RGBWWLed::setOutputRaw(int red, int green, int blue, int wwhite, int cwhite) {
	if(_pwm_output != NULL) {
		_pwm_output->setOutput(red, green, blue, wwhite, cwhite);
	}
}


/**************************************************************
 *                 ANIMATION/TRANSITION
 **************************************************************/


/**
    Main function responsible for handling animations
 */
bool RGBWWLed::show() {


	// check if we need to cancel effect
	if (_cancelAnimation || _clearAnimationQueue) {

		if (_cancelAnimation ) {
			cleanupCurrentAnimation();
		}

		// cleanup Q if we cancel all effects
		if (_clearAnimationQueue) {
			cleanupAnimationQ();
		}
	}

	#ifdef ARDUINO
		//only need this part when using arduino
		long now = millis();
		if (now - last_active < RGBWW_MINTIMEDIFF) {
			// Interval hasn't passed yet
			return true;
		}
		last_active = now;
	#endif // ARDUINO

	// Interval has passed
	// check if we need to animate or there is any new animation
	if (!_isAnimationActive) {
		//check if animation otherwise return true
		if (_animationQ->isEmpty()) {
			return true;
		}
		_currentAnimation = _animationQ->pop();
		_isAnimationActive = true;
	}

	if (_currentAnimation->run()) {
		cleanupCurrentAnimation();
		//callback animation finished
		if(_animationcallback != NULL ){
			_animationcallback(this);
		}
	}

	return false;

}


void RGBWWLed::skipAnimation(){
	if (_isAnimationActive) {
		_cancelAnimation = true;
	}
}

void RGBWWLed::clearAnimationQueue() {
	_clearAnimationQueue = true;
}

void RGBWWLed::setAnimationCallback( void (*func)(RGBWWLed* led) ) {
  _animationcallback = func;
}

void RGBWWLed::setAnimationSpeed(int speed) {
	if(_currentAnimation != NULL) {
		_currentAnimation->setSpeed(speed);
	}
}
void RGBWWLed::setAnimationBrightness(int brightness){
	if(_currentAnimation != NULL) {
			_currentAnimation->setBrightness(brightness);
		}
}

void RGBWWLed::setHSV(HSVK& color) {
	setHSV( color, 0, 1, false);
}

void RGBWWLed::setHSV(HSVK& color, int time, bool q) {
	setHSV( color, time, 1, q);
}


void RGBWWLed::setHSV(HSVK& color, int time, int direction) {
	setHSV( color, time, direction, false);
}

void RGBWWLed::setHSV(HSVK& color, int time, int direction /* = 1 */, bool q /* = false */) {
	HSVK colorFrom = getCurrentColor();
	if (colorFrom.h != color.h || colorFrom.s != color.s || colorFrom.v != color.v  || colorFrom.k != color.k  ) {
		//only set color if it differs from current color
		if (time == 0 || time < RGBWW_MINTIMEDIFF) {
			// no animation - setting color directly
			// TODO: only animate if color is different from current color
			if (!q) {
				cleanupAnimationQ();
				cleanupCurrentAnimation();
			}
			_animationQ->push(new HSVSetOutput(color, this));

		} else {
			if (!q) {
				cleanupAnimationQ();
				cleanupCurrentAnimation();
			}
			_animationQ->push(new HSVTransition(color, time, direction, this));
		}
	}

}

void RGBWWLed::setHSV(HSVK& colorFrom, HSVK& color, int time, int direction /* = 1 */, bool q /* = false */) {

	if (time == 0 || time < RGBWW_MINTIMEDIFF) {
		// no animation - setting color directly
		if (!q) {
			cleanupAnimationQ();
			cleanupCurrentAnimation();
		}
		_animationQ->push(new HSVSetOutput(color, this));

	} else {
		if (!q) {
			cleanupAnimationQ();
			cleanupCurrentAnimation();
		}
		_animationQ->push(new HSVTransition(colorFrom, color, time, direction, this));

	}
}

void RGBWWLed::cleanupCurrentAnimation() {
	if (_currentAnimation != NULL) {
		_isAnimationActive = false;
		delete _currentAnimation;
		_currentAnimation = NULL;
	}
}

void RGBWWLed::cleanupAnimationQ() {
	_animationQ->clear();
	_clearAnimationQueue = false;
}

bool RGBWWLed::isAnimationQFull() {
	return _animationQ->isFull();
}

bool RGBWWLed::isAnimationActive() {
	return _isAnimationActive;
}





