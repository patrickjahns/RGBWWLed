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

#include "RGBWWLedAnimation.h"
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"

/**************************************************************
 *               HSVSetOutput
 **************************************************************/

/**
 * Simple Animation Object to set the output to a certain color
 * without effects/transition
 *
 * @param color New color to show
 * @param ctrl	Pointer to RGBWWLed controller objekt
 */
HSVSetOutput::HSVSetOutput(const HSVK& color, RGBWWLed* ctrl){
	outputcolor = color;
	rgbwwctrl = ctrl;
}

bool HSVSetOutput::run() {
	rgbwwctrl->setOutput(outputcolor);
	return true;
}



/**************************************************************
 *               HSV Transition
 **************************************************************/

/**
 * Simple Anination to fade from the current color to another color (colorFinish)
 * There are two options for the direction of the fade (short way/ long way)
 *
 * @param colorFinish	color where the animation should end
 * @param time			the amount of time the animation takes in ms
 * @param direction 	shortest (direction == 0)/longest (direction == 1) way for transition
 * @param ctrl			main rgbww objekt
 */
HSVTransition::HSVTransition(const HSVK& color, const int& time, const int& direction, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = color;
	_hasbasecolor = false;
	_steps = time / RGBWW_MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;
}

/**
 * Simple Anination to fade from one color (colorFrom) to another color (colorFinish)
 * There are two options for the direction of the fade (short way/ long way)
 *
 * @param colorFrom		color from which the animation should start
 * @param colorFinish	color where the animation should end
 * @param time			the amount of time the animation takes in ms
 * @param direction 	shortest (direction == 0)/longest (direction == 1) way for transition
 * @param ctrl			main rgbww objekt
 */
HSVTransition::HSVTransition(const HSVK& colorFrom, const HSVK& colorFinish, const int& tm, const int& direction, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = colorFinish;
	_basecolor = colorFrom;
	_hasbasecolor = true;
	_steps = tm / RGBWW_MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;

}

void HSVTransition::init() {
	int l, r, d;
	debugRGBW("==   HSVT INIT   =====");
	if (!_hasbasecolor) {
		_basecolor = rgbwwctrl->getCurrentColor();
	}
	_currentcolor = _basecolor;

	// calculate hue direction
	l = (_basecolor.h + RGBWW_PWMHUEWHEELMAX - _finalcolor.h) % RGBWW_PWMHUEWHEELMAX;
	r = (_finalcolor.h + RGBWW_PWMHUEWHEELMAX - _basecolor.h) % RGBWW_PWMHUEWHEELMAX;

	// decide on direction of turn depending on size
	d = (l < r)? -1 : 1;

	// turn direction if user wishes for long transition
	d = (_huedirection == 1) ? d : d *= -1;

	//calculate steps per time
	_steps = (_steps > 0) ? _steps : int(1); //avoid 0 division


	//HUE
	_dhue = (d == -1) ? l : r;
	_huestep = 1;
	_huestep = (_dhue < _steps) ? (_huestep <<8) : (_dhue << 8)/_steps;
	_huestep *= d;
	_hueerror =  -1 * _steps;
	_huecount = 0;

	//SATURATION
	_dsat = abs(_basecolor.s - _finalcolor.s);
	_satstep = 1;
	_satstep = (_dsat < _steps) ? (_satstep << 8) : (_dsat << 8)/_steps;
	_satstep = (_basecolor.s > _finalcolor.s) ? _satstep*=-1 : _satstep;
	_saterror = -1 * _steps;
	_satcount = 0;

	//VALUE
	_dval = abs(_basecolor.v - _finalcolor.v);
	_valstep = 1;
	_valstep = (_dval < _steps) ? (_valstep << 8) : (_dval << 8)/_steps;
	_valstep = (_basecolor.v > _finalcolor.v) ? _valstep*=-1 : _valstep;
	_valerror = -1 * _steps;
	_valcount = 0;

	//KELVIN
	_dkelvin = abs(_basecolor.k - _finalcolor.k);
	_kelvinstep = 1;
	_kelvinstep = (_dkelvin < _steps) ? (_kelvinstep << 8 ): (_dkelvin << 8)/_steps;
	_kelvinstep = (_basecolor.k > _finalcolor.k) ? _kelvinstep*=-1 : _kelvinstep;
	_kelvinerror = -1 * _steps;
	_kelvincount = 0;

	debugRGBW("steps %i", _steps);
	debugRGBW("dhue %i", _dhue);
	debugRGBW("dsat %i", _dsat);
	debugRGBW("dval %i", _dval);
	debugRGBW("dkelvin %i", _dkelvin);
	debugRGBW("== //HSVT INIT   =====");
}


bool HSVTransition::run () {
	debugRGBW("== HSV RUN =====");
	if (_currentstep == 0) {
		init();
	}
	_currentstep++;
	if (_currentstep >= _steps) {
		// ensure that the with the last step
		// we arrive at the destination color
		rgbwwctrl->setOutput(_finalcolor);
		return true;
	}
	
	
	//	improvement idea:
	//	set new value at the beginning and then calculate next step

	//calculate new colors with bresenham
	_currentcolor.h = bresenham(_hueerror, _huecount, _steps, _dhue, _huestep, _basecolor.h, _currentcolor.h);
	//fix hue
	RGBWWColorUtils::circleHue(_currentcolor.h);
	_currentcolor.s = bresenham(_saterror, _satcount, _steps, _dsat, _satstep, _basecolor.s, _currentcolor.s);
	_currentcolor.v = bresenham(_valerror, _valcount, _steps, _dval, _valstep, _basecolor.v, _currentcolor.v);
	_currentcolor.k = bresenham(_kelvinerror, _kelvincount, _steps, _dkelvin, _kelvinstep, _basecolor.k, _currentcolor.k);



	debugRGBW("H", _currentcolor.h);
	debugRGBW("S", _currentcolor.s);
	debugRGBW("V", _currentcolor.v);
	debugRGBW("K", _currentcolor.k);
	debugRGBW("== //HSV RUN =====");

	rgbwwctrl->setOutput(_currentcolor);
	return false;
}

/**
 * Bresenham line algorithm modified for calculating dy with dx
 * Information on Algorithm see:
 * https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 *
 * @param error
 * @param ctr
 * @param dx
 * @param dy
 * @param incr
 * @param base
 * @param current
 * @return
 */
int HSVTransition::bresenham(int& error, int& ctr, int& dx, int& dy, int& incr, int& base, int& current) {
	error = error + 2 * dy;
	if (error > 0) {
		ctr++;
		error = error - 2*dx;
		return base + ((ctr * incr) >> 8);
	}
	return current;
}

/**************************************************************
                Animation Queue
 **************************************************************/

/**
 * Queue constructor
 *
 * @param qsize elements the queue can hold
 */
RGBWWLedAnimationQ::RGBWWLedAnimationQ(int qsize) {
	_size = qsize;
	_count = 0;
	_front = 0;
	_back = 0;
    q = new RGBWWLedAnimation*[qsize];
}

RGBWWLedAnimationQ::~RGBWWLedAnimationQ(){
	clear();
	delete q;
}

/**
 * Check if the queue is empty or not
 *
 * @return	bool
 */
bool RGBWWLedAnimationQ::isEmpty() {
	return _count == 0;
}

/**
 * Check if the queue is full
 *
 * @return	BOOL
 */
bool RGBWWLedAnimationQ::isFull() {
	return _count == _size;
}



/**
 * Add an animation to the queue
 *
 * @param RGBWWLedAnimation* 	animation	pointer to Animation object
 * @return	bool
 * @retval 	true 	successfully inserted object queue
 * @retval	false	did not insert object into queue
 */
bool RGBWWLedAnimationQ::push(RGBWWLedAnimation* animation) {
	if (!isFull()){
		_count++;
		q[_front] = animation;
		_front = (_front+1) % _size;
		return true;
	}
	return false;
}

/**
 * Empty Queue and delete all objects stored
 */
void RGBWWLedAnimationQ::clear() {
	while(!isEmpty()) {
		RGBWWLedAnimation* animation = pop();
		if (animation != NULL) {
			delete animation;
		}
	}
}

/**
 * Returns first Animation object pointer but keeps it in the queue
 *
 * @return RGBWWLedAnimation*
 */
RGBWWLedAnimation* RGBWWLedAnimationQ::peek() {
	if (!isEmpty()) {
        return q[_back];
	}
	return NULL;
}

/**
 *	Returns first Animation object pointer and removes it from queue
 *
 * @return RGBWWLedAnimation*
 */
RGBWWLedAnimation* RGBWWLedAnimationQ::pop() {
	RGBWWLedAnimation* tmpptr;
	if (!isEmpty()) {
		_count--;
        tmpptr = q[_back];
        q[_back] = NULL;
        _back = (_back+1) %_size;
		return tmpptr;
	}
	return NULL;
}


