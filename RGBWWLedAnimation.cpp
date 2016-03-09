/**
 * RGBWWLed - simple Library for controlling RGB WarmWhite ColdWhite strips
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * All files of this project are provided under the LGPL v3 license.
 */

#include "RGBWWLedAnimation.h"
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"

/**************************************************************
 *               HSVSetOutput
 **************************************************************/


HSVSetOutput::HSVSetOutput(const HSVK& color, RGBWWLed* ctrl, int time /* = 0 */){
	outputcolor = color;
	rgbwwctrl = ctrl;
	steps = 0;
	if (time > 0) {
		steps = time / RGBWW_MINTIMEDIFF;
	}
	count = 0;
}

bool HSVSetOutput::run() {
	if (count == 0) {
		rgbwwctrl->setOutput(outputcolor);
	}
	if (steps != 0) {
		if (count < steps) {
			return false;
		}
	}
	count += 1;
	return true;
}


/**************************************************************
 *               HSV Transition
 **************************************************************/


HSVTransition::HSVTransition(const HSVK& colorEnd, const int& time, const int& direction, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = colorEnd;
	_hasbasecolor = false;
	_steps = time / RGBWW_MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;
}


HSVTransition::HSVTransition(const HSVK& colorFrom, const HSVK& colorEnd, const int& tm, const int& direction, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = colorEnd;
	_basecolor = colorFrom;
	_hasbasecolor = true;
	_steps = tm / RGBWW_MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;

}

bool HSVTransition::init() {
	int l, r, d;
	if (!_hasbasecolor) {
		_basecolor = rgbwwctrl->getCurrentColor();
	}
	//don`t animate if the color is already the same
	if (_basecolor.h == _finalcolor.h && _basecolor.s == _finalcolor.s && _basecolor.v == _finalcolor.v && _basecolor.k == _finalcolor.k) {
		return false;
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
	hue.delta = (d == -1) ? l : r;
	hue.step = 1;
	hue.step = (hue.delta < _steps) ? (hue.step <<8) : (hue.delta << 8)/_steps;
	hue.step *= d;
	hue.error = -1 * _steps;
	hue.count = 0;

	//SATURATION
	sat.delta = abs(_basecolor.s - _finalcolor.s);
	sat.step = 1;
	sat.step = (sat.delta < _steps) ? (sat.step << 8) : (sat.delta << 8)/_steps;
	sat.step = (_basecolor.s > _finalcolor.s) ? sat.step*=-1 : sat.step;
	sat.error = -1* _steps;
	sat.count = 0;

	//VALUE
	val.delta = abs(_basecolor.v - _finalcolor.v);
	val.step = 1;
	val.step = (val.delta < _steps) ? (val.step << 8) : (val.delta << 8)/_steps;
	val.step = (_basecolor.v > _finalcolor.v) ? val.step*=-1 : val.step;
	val.error = -1*_steps;
	val.count = 0;

	//KELVIN
	kelvin.delta = abs(_basecolor.k - _finalcolor.k);
	kelvin.step = 1;
	kelvin.step = (kelvin.delta < _steps) ? (kelvin.step << 8 ): (kelvin.delta << 8)/_steps;
	kelvin.step = (_basecolor.k > _finalcolor.k) ? kelvin.step*=-1 : kelvin.step;
	kelvin.error = -1 * _steps;
	kelvin.count = 0;
	return true;
}



bool HSVTransition::run () {
	debugRGBW("== HSV RUN =====");
	if (_currentstep == 0) {
		if (!init()) {
			return true;
		}
	}
	debugRGBW("CURRENT  H %i | S %i | V %i | K %i", _currentcolor.h, _currentcolor.s, _currentcolor.v, _currentcolor.k);
	debugRGBW("FINAL    H %i | S %i | V %i | K %i", _finalcolor.h, _finalcolor.s, _finalcolor.v, _finalcolor.k);
	rgbwwctrl->setOutput(_currentcolor);

	_currentstep++;
	if (_currentstep >= _steps) {
		// ensure that the with the last step
		// we arrive at the destination color
		rgbwwctrl->setOutput(_finalcolor);
		return true;
	}
	
	//calculate new colors with bresenham
	_currentcolor.h = bresenham(hue, _steps, _basecolor.h, _currentcolor.h);
	_currentcolor.s = bresenham(sat, _steps, _basecolor.s, _currentcolor.s);
	_currentcolor.v = bresenham(val, _steps,_basecolor.v, _currentcolor.v);
	_currentcolor.k = bresenham(kelvin, _steps, _basecolor.k, _currentcolor.k);

	//fix hue
	RGBWWColorUtils::circleHue(_currentcolor.h);

	debugRGBW("== //HSV RUN =====");

	return false;
}


int HSVTransition::bresenham(BresenhamValues& values, int& dx, int& base, int& current) {
	//more information on bresenham:
	//https://www.cs.helsinki.fi/group/goa/mallinnus/lines/bresenh.html
	values.error = values.error + 2 * values.delta;
	if (values.error > 0) {
		values.count += 1;
		values.error = values.error - 2*dx;
		return base + ((values.count * values.step) >> 8);
	}
	return current;
}

/**************************************************************
                Animation Queue
 **************************************************************/


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


bool RGBWWLedAnimationQ::isEmpty() {
	return _count == 0;
}


bool RGBWWLedAnimationQ::isFull() {
	return _count == _size;
}


bool RGBWWLedAnimationQ::push(RGBWWLedAnimation* animation) {
	if (!isFull()){
		_count++;
		q[_front] = animation;
		_front = (_front+1) % _size;
		return true;
	}
	return false;
}


void RGBWWLedAnimationQ::clear() {
	while(!isEmpty()) {
		RGBWWLedAnimation* animation = pop();
		if (animation != NULL) {
			delete animation;
		}
	}
}


RGBWWLedAnimation* RGBWWLedAnimationQ::peek() {
	if (!isEmpty()) {
        return q[_back];
	}
	return NULL;
}


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
