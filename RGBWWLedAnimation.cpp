#include "RGBWWLedAnimation.h"
#include "RGBWWLedColor.h"
#include "RGBWWLed.h"



#ifndef ESP8266
    #include "compat.h"
    #include <stdlib.h>
#endif // ESP8266

RGBWWLedAnimationQ::RGBWWLedAnimationQ(int qsize) {
    _size = qsize;
    _count = 0;

}

bool RGBWWLedAnimationQ::isEmpty() {
    return _count == 0;
}

bool RGBWWLedAnimationQ::isFull() {
    return _count == _size;
}

bool RGBWWLedAnimationQ::push(RGBWWLedAnimation* animation) {
    if (!isFull()){
        //
        _count++;
        q.push(animation);
        return true;
    }
    return false;
}

RGBWWLedAnimation* RGBWWLedAnimationQ::peek() {
    if (!isEmpty()) {
        return q.front();
    }
    return NULL;
}

RGBWWLedAnimation* RGBWWLedAnimationQ::pop() {
    RGBWWLedAnimation* tmpptr;
    if (!isEmpty()) {
        _count--;
        tmpptr = q.front();
        q.pop();
        return tmpptr;
    }
    return NULL;
}


HSVTransition::HSVTransition(HSVK colorFrom, HSVK color, const int& tm, const int& direction, RGBWWLed* led ) {
    int l, r, d;

    rgbled = led;
    DEBUG("== HSVT Constructor =======");
    DEBUG("hue", color.h);
    DEBUG("huefrom", colorFrom.h);

    // calculate hue direction
    l = (colorFrom.h + PWMHUEWHEELMAX - color.h) % PWMHUEWHEELMAX;
    r = (color.h + PWMHUEWHEELMAX - colorFrom.h) % PWMHUEWHEELMAX;
    // decide on direction of turn depending on size
    d = (l < r)? -1 : 1;

    // turn direction if user wishes for long transition
    d = (direction == 1) ? d : d *= -1;

    //calculate steps per time
    _steps = tm / MINTIMEDIFF;
    _steps = (_steps > 0) ? _steps : int(1); //avoid 0 division
    _currentstep = 0;

    //HUE
    _dhue = (d == -1) ? l : r;
    _huestep = 1;
    _huestep = (_dhue < _steps) ? (_huestep <<8) : (_dhue << 8)/_steps;
    _huestep *= direction;
    _hueerror =  -1 * _steps;
    _huecount = 0;

    //SATURATION
    _dsat = abs(colorFrom.s - color.s);
    _satstep = 1;
    _satstep = (_dsat < _steps) ? (_satstep << 8) : (_dsat << 8)/_steps;
    _satstep = (colorFrom.s > color.s) ? _satstep*=-1 : _satstep;
    _saterror = -1 * _steps;
    _satcount = 0;

    //VALUE
    _dval = abs(colorFrom.v - color.v);
    _valstep = 1;
    _valstep = (_dval < _steps) ? (_valstep << 8) : (_dval << 8)/_steps;
    _valstep = (colorFrom.v > color.v) ? _valstep*=-1 : _valstep;
    _valerror = -1 * _steps;
    _valcount = 0;

    //KELVIN
    _dkelvin = abs(colorFrom.k - color.k);
    _kelvinstep = 1;
    _kelvinstep = (_dkelvin < _steps) ? (_kelvinstep << 8 ): (_dkelvin << 8)/_steps;
    _kelvinstep = (colorFrom.k > color.k) ? _kelvinstep*=-1 : _kelvinstep;
    _kelvinerror = -1 * _steps;
    _kelvincount = 0;

    // setup colors
    _basecolor = colorFrom;
    _currentcolor = colorFrom;
    _finalcolor = color;

    DEBUG("_steps", _steps);
    DEBUG("dhue", _dhue);
    DEBUG("dsat", _dsat);
    DEBUG("dval", _dval);
    DEBUG("_currentcolor", _currentcolor.h);
    DEBUG("== //HSVT Constructor =====");
}

bool HSVTransition::run(int st) {
    return run();
}
//TODO: skip steps if we missed them because of time delays
bool HSVTransition::run () {
    DEBUG("== HSV RUN =====");

    _currentstep++;

    /*
    _hueerror = _hueerror + 2*_dhue;
    if (_hueerror > 0) {
        _huecount++;
        int tmphue = _basecolor.h + ((_huecount * _huestep) >> 8);
        rgbled->circleHue(tmphue);
        _currentcolor.h = tmphue;
        _hueerror = _hueerror - 2*_steps;
    }
    //SAT
    _saterror = _saterror + 2*_dsat;
    if (_saterror > 0) {
        _satcount++;
        _currentcolor.s = _basecolor.s + ((_satcount * _satstep) >> 8);
        _saterror = _saterror - 2*_steps;
    }
    //VAL

    _valerror = _valerror + 2*_dval;
    if (_valerror > 0) {
        _valcount++;
        _currentcolor.v = _basecolor.v + ((_valcount * _valstep) >> 8);
        _valerror = _valerror - 2*_steps;
    }
    */
    //calculate new colors with bresenham
    _currentcolor.h = bresenham(_hueerror, _huecount, _steps, _dhue, _huestep, _basecolor.h, _currentcolor.h);
    //fix hue
    rgbled->circleHue(_currentcolor.h);
    _currentcolor.s = bresenham(_saterror, _satcount, _steps, _dsat, _satstep, _basecolor.s, _currentcolor.s);
    _currentcolor.v = bresenham(_valerror, _valcount, _steps, _dval, _valstep, _basecolor.v, _currentcolor.v);
    _currentcolor.k = bresenham(_kelvinerror, _kelvincount, _steps, _dkelvin, _kelvinstep, _basecolor.k, _currentcolor.k);



    DEBUG("H", _currentcolor.h);
    DEBUG("S", _currentcolor.s);
    DEBUG("V", _currentcolor.v);
    DEBUG("== //HSV RUN =====");
    if (_currentstep >= _steps) {
        // ensure that the with the last step we arrive at the destination color
        rgbled->setOutput(_finalcolor);
        return true;
    }
    rgbled->setOutput(_currentcolor);
    return false;
}
/**
    Bresenham line algorithm modified for calculating dy with dx(=time)

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
