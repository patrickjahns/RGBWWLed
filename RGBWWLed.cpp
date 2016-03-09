/**
 * RGBWWLed - simple Library for controlling RGB WarmWhite ColdWhite LEDs via PWM
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * All files of this project are provided under the LGPL v3 license.
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
	_pwm_output = NULL;
	last_active = 0;

}

RGBWWLed::~RGBWWLed() {
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
	setOutput(_current_color);
}
 
 
HSVK RGBWWLed::getCurrentColor() {
	return _current_color;
}


void RGBWWLed::setOutput(HSVK& outputcolor) {
	RGBWK rgbw;
	_current_color = outputcolor;
	colorutils.HSVtoRGB(outputcolor, rgbw);
	setOutput(rgbw);

}


void RGBWWLed::setOutput(RGBWK& outputcolor) {
	int colors[5];
	int ww, cw;
	colorutils.whiteBalance(outputcolor, ww, cw);
	colors[RGBWW_COLORS::RED] = outputcolor.r;
	colors[RGBWW_COLORS::GREEN] = outputcolor.g;
	colors[RGBWW_COLORS::BLUE] = outputcolor.b;
	colors[RGBWW_COLORS::WW] = ww;
	colors[RGBWW_COLORS::CW] = cw;
	setOutputRaw(colors[RGBWW_COLORS::RED],
			colors[RGBWW_COLORS::GREEN],
			colors[RGBWW_COLORS::BLUE],
			colors[RGBWW_COLORS::WW],
			colors[RGBWW_COLORS::CW]);
}


void RGBWWLed::setOutputRaw(int& red, int& green, int& blue, int& wwhite, int& cwhite) {
	if(_pwm_output != NULL) {
		_pwm_output->setOutput(red, green, blue, wwhite, cwhite);
	}
}


/**************************************************************
 *                 ANIMATION/TRANSITION
 **************************************************************/


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



bool RGBWWLed::isAnimationQFull() {
	return _animationQ->isFull();
}


bool RGBWWLed::isAnimationActive() {
	return _isAnimationActive;
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


void RGBWWLed::setHSV(HSVK& color, int time, bool queue) {
	setHSV( color, time, 1, queue);
}


void RGBWWLed::setHSV(HSVK& color, int time, int direction) {
	setHSV( color, time, direction, false);
}


void RGBWWLed::setHSV(HSVK& color, int time, int direction /* = 1 */, bool queue /* = false */) {

	if (time == 0 || time < RGBWW_MINTIMEDIFF) {
		// no animation - setting color directly
		if (!queue) {
			//not using queue
			cleanupAnimationQ();
			cleanupCurrentAnimation();
		}
		_animationQ->push(new HSVSetOutput(color, this));
	} else {
		if (!queue) {
			//not using queue
			cleanupAnimationQ();
			cleanupCurrentAnimation();
		}
		_animationQ->push(new HSVTransition(color, time, direction, this));
	}

}


void RGBWWLed::setHSV(HSVK& colorFrom, HSVK& color, int time, int direction /* = 1 */, bool queue /* = false */) {

	if (colorFrom.h != color.h || colorFrom.s != color.s || colorFrom.v != color.v  || colorFrom.k != color.k  ) {
		if (time == 0 || time < RGBWW_MINTIMEDIFF) {
			// no animation - setting color directly
			if (!queue) {
				//not using queue
				cleanupAnimationQ();
				cleanupCurrentAnimation();
			}
			_animationQ->push(new HSVSetOutput(color, this));

		} else {
			if (!queue) {
				//not using queue
				cleanupAnimationQ();
				cleanupCurrentAnimation();
			}
			_animationQ->push(new HSVTransition(colorFrom, color, time, direction, this));

		}
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







