#include "RGBWWLedAnimation.h"
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"

/**************************************************************
                HSV Transition
 **************************************************************/

HSVTransition::HSVTransition(const HSVK& color, const int& time, const int& direction, RGBWWLed* led ) {

	//debugRGBW("== HSVT Constructor =======");
	rgbled = led;
	_finalcolor = color;
	_hasbasecolor = false;
	_steps = time / MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;
	//debugRGBW("== //HSVT Constructor =====");
}


HSVTransition::HSVTransition(const HSVK& colorFrom, const HSVK& color, const int& tm, const int& direction, RGBWWLed* led ) {

	//debugRGBW("== HSVT Constructor =======");
	rgbled = led;
	_finalcolor = color;
	_basecolor = colorFrom;
	_hasbasecolor = true;
	_steps = tm / MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;
	//debugRGBW("== //HSVT Constructor =====");
}

void HSVTransition::init() {
	int l, r, d;
	//debugRGBW("==   HSVT INIT   =====");
	if (!_hasbasecolor) {
		_basecolor = rgbled->getCurrenctColor();
	}
	_currentcolor = _basecolor;
	// calculate hue direction
	l = (_basecolor.h + PWMHUEWHEELMAX - _finalcolor.h) % PWMHUEWHEELMAX;
	r = (_finalcolor.h + PWMHUEWHEELMAX - _basecolor.h) % PWMHUEWHEELMAX;
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

	//debugRGBW("_steps %i", _steps);
	//debugRGBW("dhue %i", _dhue);
	//debugRGBW("dsat %i", _dsat);
	//debugRGBW("dval %i", _dval);
	//debugRGBW("_currentcolor.h  %i", _currentcolor.h);
	//debugRGBW("== //HSVT INIT   =====");
}

bool HSVTransition::run() {
	return run(1);
}

bool HSVTransition::run (int st) {
	//debugRGBW("== HSV RUN =====");
	if (_currentstep == 0) {
		init();
	}
	_currentstep++;
	if (_currentstep >= _steps) {
		// ensure that the with the last step we arrive at the destination color
		rgbled->setOutput(_finalcolor);
		return true;
	}
	
	/*
	*	improvement idea: set new value at the beginning and then calculate next step
	*
	*/
	
	//calculate new colors with bresenham
	_currentcolor.h = bresenham(_hueerror, _huecount, _steps, _dhue, _huestep, _basecolor.h, _currentcolor.h);
	//fix hue
	rgbled->circleHue(_currentcolor.h);
	_currentcolor.s = bresenham(_saterror, _satcount, _steps, _dsat, _satstep, _basecolor.s, _currentcolor.s);
	_currentcolor.v = bresenham(_valerror, _valcount, _steps, _dval, _valstep, _basecolor.v, _currentcolor.v);
	_currentcolor.k = bresenham(_kelvinerror, _kelvincount, _steps, _dkelvin, _kelvinstep, _basecolor.k, _currentcolor.k);



	//debugRGBW("H", _currentcolor.h);
	//debugRGBW("S", _currentcolor.s);
	//debugRGBW("V", _currentcolor.v);
	//debugRGBW("== //HSV RUN =====");

	rgbled->setOutput(_currentcolor);
	return false;
}
/**
    Bresenham line algorithm modified for calculating dy with dx (=time)
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

RGBWWLedAnimationQ::RGBWWLedAnimationQ(int qsize) {
	_size = qsize;
	_count = 0;
	_front = 0;
	_back = 0;
    q = new RGBWWLedAnimation*[qsize];
}

RGBWWLedAnimationQ::~RGBWWLedAnimationQ(){
	//cleanup 
	RGBWWLedAnimation* animation;
	while(!isEmpty()) {
		animation = pop();
		if (animation != NULL) {
			delete animation;
		}
	}
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
/**
    Returns animation object pointer but keeps it in Q
 */

RGBWWLedAnimation* RGBWWLedAnimationQ::peek() {
	if (!isEmpty()) {
        return q[_back];
	}
	return NULL;
}
/**
    Returns current Animation object pointer and removes it from queue

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
