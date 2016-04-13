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
	_current_output = ChannelOutput(0, 0, 0, 0, 0);
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

ChannelOutput RGBWWLed::getCurrentOutput() {
	return _current_output;
}


HSVK RGBWWLed::getCurrentColor() {
	return _current_color;
}


void RGBWWLed::setOutput(HSVK& outputcolor) {
	RGBWK rgbwk;
	_current_color = outputcolor;
	colorutils.HSVtoRGB(outputcolor, rgbwk);
	setOutput(rgbwk);

}


void RGBWWLed::setOutput(RGBWK& outputcolor) {
	ChannelOutput output;
	colorutils.whiteBalance(outputcolor, output);
	setOutput(output);
}

void RGBWWLed::setOutput(ChannelOutput& output) {
	if(_pwm_output != NULL) {
		colorutils.correctBrightness(output);
		_current_output = output;
		_pwm_output->setOutput(output.r, output.g, output.b, output.ww, output.cw);
	}
};

void RGBWWLed::setOutputRaw(int& red, int& green, int& blue, int& wwhite, int& cwhite) {
	if(_pwm_output != NULL) {
		_current_output = ChannelOutput(red, green, blue, wwhite, cwhite);
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
		//callback animation finished
		if(_animationcallback != NULL ){
			_animationcallback(this);
		}
		cleanupCurrentAnimation();
	}

	return false;

}


bool RGBWWLed::addToQueue(RGBWWLedAnimation* animation) {
	return _animationQ->push(animation);
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


void RGBWWLed::setRAW(ChannelOutput output) {
	setRAW(output, 0, false);
}


void RGBWWLed::setRAW(ChannelOutput output, int time, bool queue /* = false */) {
	if (time == 0 || time < RGBWW_MINTIMEDIFF) {
		// no animation - setting color directly
		if (!queue) {
			//not using queue
			cleanupAnimationQ();
			cleanupCurrentAnimation();
		}
		_animationQ->push(new RAWSetOutput(output, this));
	} else {
		if (!queue) {
			//not using queue
			cleanupAnimationQ();
			cleanupCurrentAnimation();
		}
		_animationQ->push(new RAWTransition(output, time, this));
	}
}


void RGBWWLed::setRAW(ChannelOutput output_from, ChannelOutput output, int time, bool queue /* = false */) {
	if (output_from.r != output.r || output_from.g != output.g || output_from.b != output.b  ||
				output_from.ww != output.ww || output_from.cw != output.cw ) {
		if (time == 0 || time < RGBWW_MINTIMEDIFF) {
			// no animation - setting color directly
			if (!queue) {
				//not using queue
				cleanupAnimationQ();
				cleanupCurrentAnimation();
			}
			_animationQ->push(new RAWSetOutput(output, this));

		} else {
			if (!queue) {
				//not using queue
				cleanupAnimationQ();
				cleanupCurrentAnimation();
			}
			_animationQ->push(new RAWTransition(output_from, output, time, this));

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
