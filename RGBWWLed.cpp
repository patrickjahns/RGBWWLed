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
	_current_color = HSVCT(0, 0, 0);
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


HSVCT RGBWWLed::getCurrentColor() {
	return _current_color;
}



void RGBWWLed::setOutput(HSVCT& outputcolor) {
	RGBWCT rgbwk;
	_current_color = outputcolor;
	colorutils.HSVtoRGB(outputcolor, rgbwk);
	setOutput(rgbwk);

}


void RGBWWLed::setOutput(RGBWCT& outputcolor) {
	ChannelOutput output;
	colorutils.whiteBalance(outputcolor, output);
	setOutput(output);
}


void RGBWWLed::setOutput(ChannelOutput& output) {
	if(_pwm_output != NULL) {
		colorutils.correctBrightness(output);
		_current_output = output;
		debugRGBW("R:%i | G:%i | B:%i | WW:%i | CW:%i", output.r, output.g, output.b, output.ww, output.cw);
		_pwm_output->setOutput(RGBWW_dim_curve[output.r],
							   RGBWW_dim_curve[output.g],
							   RGBWW_dim_curve[output.b],
							   RGBWW_dim_curve[output.ww],
							   RGBWW_dim_curve[output.cw]);
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

void RGBWWLed::setHSV(HSVCT& color, bool queue /*= false */) {
	if (!queue) {
		//not using queue
		cleanupAnimationQ();
		cleanupCurrentAnimation();
	}
	_animationQ->push(new HSVSetOutput(color, this));
}



void RGBWWLed::setHSV(HSVCT& color, int time, bool  queue /*= false*/) {
	if (!queue) {
		//not using queue
		cleanupAnimationQ();
		cleanupCurrentAnimation();
	}
	_animationQ->push(new HSVSetOutput(color, this, time));
}


void RGBWWLed::fadeHSV(HSVCT& color, int time, bool queue) {
	fadeHSV( color, time, 1, queue);
}


void RGBWWLed::fadeHSV(HSVCT& color, int time, int direction) {
	fadeHSV( color, time, direction, false);
}


void RGBWWLed::fadeHSV(HSVCT& color, int time, int direction /* = 1 */, bool queue /* = false */) {

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


void RGBWWLed::fadeHSV(HSVCT& colorFrom, HSVCT& color, int time, int direction /* = 1 */, bool queue /* = false */) {

	if (colorFrom.h != color.h || colorFrom.s != color.s || colorFrom.v != color.v  || colorFrom.ct != color.ct  ) {
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


void RGBWWLed::setRAW(ChannelOutput output, bool queue /* = false */) {
	if (!queue) {
		//not using queue
		cleanupAnimationQ();
		cleanupCurrentAnimation();
	}
	_animationQ->push(new RAWSetOutput(output, this));

}

void RGBWWLed::setRAW(ChannelOutput output, int time, bool queue /* = false */) {
	if (!queue) {
		//not using queue
		cleanupAnimationQ();
		cleanupCurrentAnimation();
	}
	_animationQ->push(new RAWSetOutput(output, this, time));
}


void RGBWWLed::fadeRAW(ChannelOutput output, int time, bool queue /* = false */) {
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


void RGBWWLed::fadeRAW(ChannelOutput output_from, ChannelOutput output, int time, bool queue /* = false */) {
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
