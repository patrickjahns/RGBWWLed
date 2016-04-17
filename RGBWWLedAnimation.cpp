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


HSVSetOutput::HSVSetOutput(const HSVCT& color, RGBWWLed* ctrl, int time /* = 0 */){
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


HSVTransition::HSVTransition(const HSVCT& colorEnd, const int& time, const int& direction, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = colorEnd;
	_hasbasecolor = false;
	_steps = time / RGBWW_MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;
}


HSVTransition::HSVTransition(const HSVCT& colorFrom, const HSVCT& colorEnd, const int& time, const int& direction, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = colorEnd;
	_basecolor = colorFrom;
	_hasbasecolor = true;
	_steps = time / RGBWW_MINTIMEDIFF;
	_huedirection = direction;
	_currentstep = 0;

}

bool HSVTransition::init() {
	int l, r, d;
	if (!_hasbasecolor) {
		_basecolor = rgbwwctrl->getCurrentColor();
	}
	//don`t animate if the color is already the same
	if (_basecolor.h == _finalcolor.h && _basecolor.s == _finalcolor.s && _basecolor.v == _finalcolor.v && _basecolor.ct == _finalcolor.ct) {
		return false;
	}
	_currentcolor = _basecolor;

	// calculate hue direction
	l = (_basecolor.h + RGBWW_CALC_HUEWHEELMAX - _finalcolor.h) % RGBWW_CALC_HUEWHEELMAX;
	r = (_finalcolor.h + RGBWW_CALC_HUEWHEELMAX - _basecolor.h) % RGBWW_CALC_HUEWHEELMAX;

	// decide on direction of turn depending on size
	d = (l < r) ? -1 : 1;

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
	ct.delta = abs(_basecolor.ct - _finalcolor.ct);
	ct.step = 1;
	ct.step = (ct.delta < _steps) ? (ct.step << 8 ): (ct.delta << 8)/_steps;
	ct.step = (_basecolor.ct > _finalcolor.ct) ? ct.step*=-1 : ct.step;
	ct.error = -1 * _steps;
	ct.count = 0;
	return true;
}



bool HSVTransition::run () {

	if (_currentstep == 0) {
		if (!init()) {
			return true;
		}
		_currentstep = 0;
	}
	debugRGBW("HSVTransition::run CURRENT  H %i | S %i | V %i | K %i", _currentcolor.h, _currentcolor.s, _currentcolor.v, _currentcolor.ct);
	debugRGBW("HSVTransition::run FINAL    H %i | S %i | V %i | K %i", _finalcolor.h, _finalcolor.s, _finalcolor.v, _finalcolor.ct);
	_currentstep++;
	if (_currentstep >= _steps) {
		// ensure that the with the last step
		// we arrive at the destination color
		rgbwwctrl->setOutput(_finalcolor);
		return true;
	}
	rgbwwctrl->setOutput(_currentcolor);

	//calculate new colors with bresenham
	_currentcolor.h = bresenham(hue, _steps, _basecolor.h, _currentcolor.h);
	_currentcolor.s = bresenham(sat, _steps, _basecolor.s, _currentcolor.s);
	_currentcolor.v = bresenham(val, _steps,_basecolor.v, _currentcolor.v);
	_currentcolor.ct = bresenham(ct, _steps, _basecolor.ct, _currentcolor.ct);

	//fix hue
	RGBWWColorUtils::circleHue(_currentcolor.h);


	return false;
}

void HSVTransition::reset() {
	_currentstep = 0;
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
 *               RAWSetOutput
 **************************************************************/


RAWSetOutput::RAWSetOutput(const ChannelOutput& output, RGBWWLed* ctrl, int time /* = 0 */){
	outputcolor = output;
	rgbwwctrl = ctrl;
	steps = 0;
	if (time > 0) {
		steps = time / RGBWW_MINTIMEDIFF;
	}
	count = 0;
}

bool RAWSetOutput::run() {
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
 *               RAW Transition
 **************************************************************/


RAWTransition::RAWTransition(const ChannelOutput& output, const int& time, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = output;
	_hasbasecolor = false;
	_steps = time / RGBWW_MINTIMEDIFF;
	_currentstep = 0;
}


RAWTransition::RAWTransition(const ChannelOutput& output_from, const ChannelOutput& output, const int& time, RGBWWLed* ctrl ) {
	rgbwwctrl = ctrl;
	_finalcolor = output;
	_basecolor = output_from;
	_hasbasecolor = true;
	_steps = time / RGBWW_MINTIMEDIFF;
	_currentstep = 0;

}

bool RAWTransition::init() {
	if (!_hasbasecolor) {
		_basecolor = rgbwwctrl->getCurrentOutput();
	}

	// don`t animate if the color is already the same
	if (_basecolor.r == _finalcolor.r && _basecolor.g == _finalcolor.g && _basecolor.b == _finalcolor.b &&
			_basecolor.ww == _finalcolor.ww && _basecolor.cw == _finalcolor.cw) {
		return false;
	}
	_currentcolor = _basecolor;

	// calculate steps per time
	_steps = (_steps > 0) ? _steps : int(1); //avoid 0 division


	// RED
	red.delta = abs(_basecolor.r - _finalcolor.r);
	red.step = 1;
	red.step = (red.delta < _steps) ? (red.step << 8) : (red.delta << 8)/_steps;
	red.step = (_basecolor.r > _finalcolor.r) ? red.step*=-1 : red.step;
	red.error = -1* _steps;
	red.count = 0;

	// GREEN
	green.delta = abs(_basecolor.g - _finalcolor.g);
	green.step = 1;
	green.step = (green.delta < _steps) ? (green.step << 8) : (green.delta << 8)/_steps;
	green.step = (_basecolor.g > _finalcolor.g) ? green.step*=-1 : green.step;
	green.error = -1* _steps;
	green.count = 0;

	// BLUE
	blue.delta = abs(_basecolor.b - _finalcolor.b);
	blue.step = 1;
	blue.step = (blue.delta < _steps) ? (blue.step << 8) : (blue.delta << 8)/_steps;
	blue.step = (_basecolor.b > _finalcolor.b) ? blue.step*=-1 : blue.step;
	blue.error = -1* _steps;
	blue.count = 0;

	// WW
	warmwhite.delta = abs(_basecolor.ww - _finalcolor.ww);
	warmwhite.step = 1;
	warmwhite.step = (warmwhite.delta < _steps) ? (warmwhite.step << 8) : (warmwhite.delta << 8)/_steps;
	warmwhite.step = (_basecolor.ww > _finalcolor.ww) ? warmwhite.step*=-1 : warmwhite.step;
	warmwhite.error = -1* _steps;
	warmwhite.count = 0;

	// CW
	coldwhite.delta = abs(_basecolor.cw - _finalcolor.cw);
	coldwhite.step = 1;
	coldwhite.step = (coldwhite.delta < _steps) ? (coldwhite.step << 8) : (coldwhite.delta << 8)/_steps;
	coldwhite.step = (_basecolor.cw > _finalcolor.cw) ? coldwhite.step*=-1 : coldwhite.step;
	coldwhite.error = -1* _steps;
	coldwhite.count = 0;
	return true;
}



bool RAWTransition::run () {

	if (_currentstep == 0) {
		if (!init()) {
			return true;
		}
		_currentstep = 1;
	}
	debugRGBW("RAWTransition::run CURRENT  R %i | G %i | B %i | WW %i | CW %i  ", _currentcolor.r, _currentcolor.g, _currentcolor.b, _currentcolor.ww, _currentcolor.cw);
	debugRGBW("RAWTransition::run FINAL    R %i | G %i | B %i | WW %i | CW %i ", _finalcolor.r, _finalcolor.g, _finalcolor.b, _finalcolor.ww, _finalcolor.cw);


	if (_currentstep >= _steps) {
		// ensure that the with the last step
		// we arrive at the destination color
		rgbwwctrl->setOutput(_finalcolor);
		return true;
	}
	rgbwwctrl->setOutput(_currentcolor);
	_currentstep++;
	//calculate new colors with bresenham
	_currentcolor.r = bresenham(red, _steps, _basecolor.r, _currentcolor.r);
	_currentcolor.g = bresenham(green, _steps, _basecolor.g, _currentcolor.g);
	_currentcolor.b = bresenham(blue, _steps,_basecolor.b, _currentcolor.b);
	_currentcolor.ww = bresenham(warmwhite, _steps, _basecolor.ww, _currentcolor.ww);
	_currentcolor.cw = bresenham(coldwhite, _steps, _basecolor.cw, _currentcolor.cw);



	return false;
}

void RAWTransition::reset() {
	_currentstep = 0;
}


int RAWTransition::bresenham(BresenhamValues& values, int& dx, int& base, int& current) {
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
                Animation Set
 **************************************************************/

RGBWWAnimationSet::RGBWWAnimationSet(RGBWWLedAnimation** animations, int count, bool loop /* =false */){
	q = animations;
	_count = count;
	_loop = loop;
}


RGBWWAnimationSet::~RGBWWAnimationSet(){
	for (int i = 0; i < _count; i++) {
		delete q[i];
	}
}

void RGBWWAnimationSet::setSpeed(int newspeed) {
	_speed = newspeed;
	q[_current]->setSpeed(_speed);
};

void RGBWWAnimationSet::setBrightness(int newbrightness){
	_brightness = newbrightness;
	q[_current]->setBrightness(_brightness);
};

bool RGBWWAnimationSet::run(){

	if (q[_current]->run()) {
		q[_current]->reset();
		_current+=1;
		if (_brightness != -1) {
			q[_current]->setBrightness(_brightness);
		}
		if (_speed != -1) {
			q[_current]->setSpeed(_speed);
		}
		if (_current >= _count) {
			if(_loop) {
				_current = 0;
			} else {
				return true; // finished set
			}
		}
	}

	return false; //continuing animation
};


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
