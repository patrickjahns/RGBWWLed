/**************************************************************


    @author Patrick Jahns
 **************************************************************/
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"
#include "RGBWWLedAnimation.h"
#ifndef ESP8266
    #include "compat.h"
    #include <stdlib.h>
#endif // ESP8266

/**************************************************************
                setup, init and settings
 **************************************************************/

RGBWWLed::RGBWWLed() {
    _isAnimationActive = false;
    _cancelAnimation = false;
    _clearAnimationQueue = false;
	_colormode = MODE_RGB;
	_current_color = HSVK(0, 0, 0);
    _currentAnimation = NULL;
    createHueWheel();
	correctMaxBrightness(PWMWIDTH, PWMWIDTH, PWMWIDTH, PWMWIDTH, PWMWIDTH);


}


void RGBWWLed::init(int redPIN, int greenPIN, int bluePIN, int wwPIN, int cwPIN, int pwmFrequency) {
    _redPIN = redPIN;
    _greenPIN = greenPIN;
    _bluePIN = bluePIN;
    _wwPIN = wwPIN;
    _cwPIN = cwPIN;

    //set pins to output
    pinMode(_redPIN, OUTPUT);
    pinMode(_greenPIN, OUTPUT);
    pinMode(_bluePIN, OUTPUT);
    pinMode(_wwPIN, OUTPUT);
    pinMode(_cwPIN, OUTPUT);

    //change PWM Frequency
    analogWriteFreq(pwmFrequency);
}

void RGBWWLed::setMode(int mode) {
    switch(mode) {
        case MODE_RGBWW: _colormode = MODE_RGBWW; break;
        case MODE_RGBCW: _colormode = MODE_RGBCW; break;
        case MODE_RGBWWCW: _colormode = MODE_RGBWWCW; break;
        default: _colormode = MODE_RGB; break;
    }

}

/**
    Correct the Maximum brightness of a channel

    @param r red channel (0.0 - 1.0)
    @param g green channel (0.0 - 1.0)
    @param b blue channel (0.0 - 1.0)
    @param cw cold white channel (0.0 - 1.0)
    @param wm warm white channel (0.0 - 1.0)

*/
void RGBWWLed::correctMaxBrightness(float r, float g, float b, float ww, float cw) {
    _BrightnessFactor[0] = constrain(r, 0.0, 1.0) * PWMWIDTH;
    _BrightnessFactor[1] = constrain(g, 0.0, 1.0) * PWMWIDTH;
    _BrightnessFactor[2] = constrain(b, 0.0, 1.0) * PWMWIDTH;
    _BrightnessFactor[3] = constrain(cw, 0.0, 1.0) * PWMWIDTH;
    _BrightnessFactor[4] = constrain(ww, 0.0, 1.0) * PWMWIDTH;

};




/**
*    Correct HSV
*
*    @param red
*    @param yellow
*    @param green
*    @param cyan
*    @param blue
*    @param magenta
*
*/

void RGBWWLed::correctHSV(float red, float yellow, float green, float cyan, float blue, float magenta) {
    // reset color wheel before applying any changes
    // otherwise we apply changes to any previous colorwheel
    //TODO: write unit test
    createHueWheel();
    DEBUG("==  correctHSV  ==");
    DEBUG("==  before      ==");
    DEBUG("W0 ", _HueWheelSectorWidth[0]);
    DEBUG("W1 ", _HueWheelSectorWidth[1]);
    DEBUG("W2 ", _HueWheelSectorWidth[2]);
    DEBUG("W3 ", _HueWheelSectorWidth[3]);
    DEBUG("W4 ", _HueWheelSectorWidth[4]);
    DEBUG("W5 ", _HueWheelSectorWidth[5]);
    DEBUG(" ");
    DEBUG("B0 ", _HueWheelSector[0]);
    DEBUG("B1 ", _HueWheelSector[1]);
    DEBUG("B2 ", _HueWheelSector[2]);
    DEBUG("B3 ", _HueWheelSector[3]);
    DEBUG("B4 ", _HueWheelSector[4]);
    DEBUG("B5 ", _HueWheelSector[5]);
    DEBUG("B6 ", _HueWheelSector[6]);
    DEBUG(" ");

	//correct sector 1
    _HueWheelSectorWidth[0] -= parseColorCorrection(red);
    _HueWheelSectorWidth[0] += parseColorCorrection(yellow);
    _HueWheelSector[1] += parseColorCorrection(yellow);

    //correct sector 2
    _HueWheelSectorWidth[1] -= parseColorCorrection(yellow);
    _HueWheelSectorWidth[1] += parseColorCorrection(green);
    _HueWheelSector[2] += parseColorCorrection(green);

    //correct sector 3
    _HueWheelSectorWidth[2] -= parseColorCorrection(green);
    _HueWheelSectorWidth[2] += parseColorCorrection(cyan);
    _HueWheelSector[3] += parseColorCorrection(cyan);

    //correct sector 4
    _HueWheelSectorWidth[3] -= parseColorCorrection(cyan);
    _HueWheelSectorWidth[3] += parseColorCorrection(blue);
    _HueWheelSector[4] += parseColorCorrection(blue);

    //correct sector 5
    _HueWheelSectorWidth[4] -= parseColorCorrection(blue);
    _HueWheelSectorWidth[4] += parseColorCorrection(magenta);
    _HueWheelSector[5] += parseColorCorrection(magenta);

    //correct sector 6
    _HueWheelSectorWidth[5] -= parseColorCorrection(magenta);
    _HueWheelSectorWidth[5] += parseColorCorrection(red);
    _HueWheelSector[6] += parseColorCorrection(red);
    _HueWheelSector[0] += parseColorCorrection(red);


    DEBUG("==  after      ==");
    DEBUG("W0 ", _HueWheelSectorWidth[0]);
    DEBUG("W1 ", _HueWheelSectorWidth[1]);
    DEBUG("W2 ", _HueWheelSectorWidth[2]);
    DEBUG("W3 ", _HueWheelSectorWidth[3]);
    DEBUG("W4 ", _HueWheelSectorWidth[4]);
    DEBUG("W5 ", _HueWheelSectorWidth[5]);
    DEBUG(" ");
    DEBUG("B0 ", _HueWheelSector[0]);
    DEBUG("B1 ", _HueWheelSector[1]);
    DEBUG("B2 ", _HueWheelSector[2]);
    DEBUG("B3 ", _HueWheelSector[3]);
    DEBUG("B4 ", _HueWheelSector[4]);
    DEBUG("B5 ", _HueWheelSector[5]);
    DEBUG("B6 ", _HueWheelSector[6]);
    DEBUG("==  //correctHSV  ==");
    DEBUG(" ");
}

/**************************************************************
                    OUTPUT
**************************************************************/

void RGBWWLed::setOutput(HSVK color) {
    RGBWK rgbw;
    _current_color = color;
    HSVtoRGB(color, rgbw);
    setOutput(rgbw);

}

void RGBWWLed::setOutput(RGBWK c) {
    int color[5];
    //TODO white correction
    color[0] = c.r;
    color[1] = c.g;
    color[2] = c.b;
    color[3] = c.w;
    color[4] = c.w;


    analogWrite(_redPIN, (color[0] * _BrightnessFactor[0]) >> PWMDEPTH);
    analogWrite(_greenPIN, (color[1] * _BrightnessFactor[1]) >> PWMDEPTH);
    analogWrite(_bluePIN, (color[2] * _BrightnessFactor[2]) >> PWMDEPTH);
    analogWrite(_wwPIN, (color[3] * _BrightnessFactor[3]) >> PWMDEPTH);
    analogWrite(_cwPIN, (color[4] * _BrightnessFactor[4]) >> PWMDEPTH);

}

/**
    Directly set the PWM values without color correction or white balance

*/
void RGBWWLed::setOutputRaw(int red, int green, int blue, int wwhite, int cwhite) {

    analogWrite(_redPIN, red);
    analogWrite(_greenPIN, green);
    analogWrite(_bluePIN, blue);
    analogWrite(_wwPIN, wwhite);
    analogWrite(_cwPIN, cwhite);

}


/**************************************************************
                ANIMATION/TRANSITION
**************************************************************/


/**
    Main function responsible for handling animations
*/
bool RGBWWLed::show() {
    long now = millis();


    // check if we need to cancel effect
    if (_cancelAnimation || _clearAnimationQueue) {
        //TODO empty the whole queue here
        if (_currentAnimation != NULL) {
            delete _currentAnimation;
            _currentAnimation = NULL;
        }
        _isAnimationActive = false;
        _cancelAnimation = false;
        if (_clearAnimationQueue) {
            //TODO: clear Animation Queue
            _clearAnimationQueue = false;
        }
    }

    #ifdef ESP8266
    // Interval hasn't passed yet
    if (now - last_active < MINTIMEDIFF) {
        return true;
    }
    #endif // ESP8266
    last_active = now;

    // check if we need to animate or there is any new animation
    if (!_isAnimationActive) {
        //check if animation otherwise return true
        return true;
    }
    // Interval has passed - run animation

    if (_currentAnimation->run()) {
        DEBUG("finished animation");
        if (_currentAnimation != NULL) {
                delete _currentAnimation;
                _currentAnimation = NULL;
            }
        _isAnimationActive = false;
    }

    return false;

}

void    RGBWWLed::skipAnimation(){
    if (_isAnimationActive) {
        _cancelAnimation = true;
    }
}

void    RGBWWLed::clearAnimationQueue() {
    if (_isAnimationActive) {
        _clearAnimationQueue = true;
    }
}


void RGBWWLed::setHSV(HSVK& color) {
    setHSV( color, 0);
}


void RGBWWLed::setHSV(HSVK& color, int tm, bool shortDirection) {
    // get current value and then move forward
    setHSV( _current_color, color, tm, shortDirection);
}


void RGBWWLed::setHSV(HSVK& colorFrom, HSVK& color, int tm, bool shortDirection ) {
    // only change color if it is different
    if (colorFrom.h != color.h || colorFrom.s != color.s || colorFrom.v != color.v  ) {
        if (tm == 0 || tm < MINTIMEDIFF) {
            // no transition time - directly set the color
            setOutput(color);
        } else {
            // Fix fading from off to on with different HUE
            //colorFrom.h = (colorFrom.v == 0 && color.h != colorFrom.h) ? color.h : colorFrom.h;

            //TODO: check if there has been another animation
            //TODO: animation Q - how to implement
            if (_currentAnimation != NULL) {
                _isAnimationActive = false;
                delete _currentAnimation;
                _currentAnimation = NULL;
            }
            _currentAnimation = new HSVTransition( colorFrom, color, tm, shortDirection, this);
            //_currentAnimation->run();
            _isAnimationActive = true;
        }
    }

}

/**************************************************************
                COLORUTILS
**************************************************************/
/**
    Converts HSV values to RGB colorspace

*/

void RGBWWLed::HSVtoRGB(const HSVK& hsv, RGBWK& rgbw) {
    int val, hue, sat, r, g, b, fract, chroma, m;
    //TODO: write unit test

    hue = hsv.h;
    //gamma correction
    val = dim_curve[hsv.v];
    //sat = PWMMAXVAL-dim_curve[PWMMAXVAL-sat];
    sat = hsv.s;

    rgbw.k = hsv.k;
    DEBUG("==  HSV2RGB  ==");
    DEBUG("HUE ", hue);
    DEBUG("SAT ", sat);
    DEBUG("VAL ", val);
    if(sat == 0) {
        /* color is grayscale */
        if (_colormode == MODE_RGB) {
            rgbw.r = val;
            rgbw.g = val;
            rgbw.b = val;
            rgbw.w = 0; //might be undefined otherwise
        } else {
            rgbw.r = 0;
            rgbw.g = 0;
            rgbw.b = 0;
            rgbw.w = val;
        }

    } else {
        /*
        We have 6 sectors
        We need 7 "borders" to incorporate the shift oft sectors

        Example:
        Sector 1 is from 0 - 255. We need border 0 as lower border, 255 as upper border.
        Sector 6 is from 1275 - 1530. Lower border 1275, upper border 1530

        Apply a color correct for red with +10deg (value 25) results in
        Sector 1 from 0 - 255 & 1505 - 1530
        Sector 6 from 1275 - 1505

        Apply a color correct for red with -10deg (value 25) results in
        Sector 1 from 25 - 255
        Sector 6 from 1275 - 1530 && 0 - 25
        */
        chroma = (sat * val)/PWMMAXVAL;
        m = val - chroma;
        DEBUG("CHR ",chroma);
        DEBUG("M   ", m);
        DEBUG("===============");
        if ( hue < _HueWheelSector[0] || (hue > _HueWheelSector[5] && hue <= _HueWheelSector[6])) {
            DEBUG("Sector 6");
            if (hue < _HueWheelSector[0]) {
                fract = PWMMAXVAL + hue ;
            } else {
                fract = hue - _HueWheelSector[5];
            }

            r = chroma;
            g = 0;
            DEBUG(fract);
            DEBUG(_HueWheelSectorWidth[5]);
            b = ( chroma * (PWMMAXVAL - (PWMMAXVAL * fract) / _HueWheelSectorWidth[5])) >> PWMDEPTH;

        } else if (  hue <= _HueWheelSector[1]  || hue > _HueWheelSector[6]) {
            // Sector 1
            DEBUG("Sector 1");
            if (hue > _HueWheelSector[6]) {
                fract = hue - _HueWheelSector[6];
            } else {
                fract = hue + (PWMHUEWHEELMAX - _HueWheelSector[6]);
            }
            r = chroma;
            g = (chroma * ((PWMMAXVAL * fract) / _HueWheelSectorWidth[0])) >> PWMDEPTH;
            b = 0;

        } else if (hue <= _HueWheelSector[2]) {
            // Sector 2
            DEBUG("Sector 2");
            fract = hue - _HueWheelSector[1];
            r = (chroma * (PWMMAXVAL - (PWMMAXVAL * fract) / _HueWheelSectorWidth[1])) >> PWMDEPTH;
            g = chroma;
            b = 0;

        } else if (hue <= _HueWheelSector[3]) {
            // Sector 3
            DEBUG("Sector 3");
            fract = hue - _HueWheelSector[2];
            r = 0;
            g = chroma;
            b = (chroma * ((PWMMAXVAL * fract) / _HueWheelSectorWidth[2])) >> PWMDEPTH;

        } else if (hue <= _HueWheelSector[4]) {
            // Sector 4
            DEBUG("Sector 4");
            fract = hue - _HueWheelSector[3];
            r = 0;
            g =(chroma * (PWMMAXVAL - (PWMMAXVAL * fract) / _HueWheelSectorWidth[3])) >> PWMDEPTH;
            b = chroma;

        } else  {
            // Sector 5
            DEBUG("Sector 5");
            fract = hue - _HueWheelSector[4];
            r = (chroma * ((PWMMAXVAL * fract) / _HueWheelSectorWidth[4])) >> PWMDEPTH;
            g = 0;
            b = chroma;

        }
        // m equals the white part
        // for rgbw we use it for the white channels
        if (_colormode == MODE_RGB) {
            rgbw.r = r + m;
            rgbw.g = g + m;
            rgbw.b = b + m;
            rgbw.w = 0; // otherwise might be undefined!

            DEBUG("===============");
            DEBUG("R ", rgbw.r);
            DEBUG("G ", rgbw.g);
            DEBUG("B ", rgbw.b);
            DEBUG("===============");
        } else {
            rgbw.r = r;
            rgbw.g = g;
            rgbw.b = b;
            rgbw.w = m;

            DEBUG("===============");
            DEBUG("R ", rgbw.r);
            DEBUG("G ", rgbw.g);
            DEBUG("B ", rgbw.b);
            DEBUG("W ", rgbw.w);
            DEBUG("===============");
        }
    }
    DEBUG("==  //HSV2RGB  ==");
}

void  RGBWWLed::RGBtoHSV(const RGBWK& rgbw, HSVK& hsv) {
    DEBUG("RGBtoHSV");

};

/**************************************************************
                HELPER FUNCTIONS
**************************************************************/


/*
Helper function to create the 6 sectors for the HUE wheel
*/
void RGBWWLed::createHueWheel() {
    _HueWheelSector[0] = 0;
    for (int i = 1; i <= 6; ++i) {
        _HueWheelSector[i] = i*PWMMAXVAL;
        _HueWheelSectorWidth[i-1] = PWMMAXVAL;
    }
}

/*
Helper function to keep Hue between 0 - HueWheelMax
*/
void RGBWWLed::circleHue(int& hue ) {
    while (hue >= PWMHUEWHEELMAX) hue -= PWMHUEWHEELMAX;
    while (hue < 0) hue += PWMHUEWHEELMAX;

}

/*
Helper functions to parse hue,sat,val,colorcorrection
Converts to the according pwm size (8bit/10bit)
*/
int RGBWWLed::parseColorCorrection(float val) {
    if (val >= 29.5) val = 29.5;
    if (val <= -29.5) val = -29.5;
    return int(((val / 60) * (PWMMAXVAL)) * -1);
}

int RGBWWLed::parseHue(float hue) {
    hue = constrain(hue, 0.0, 360.0);
    return int((hue / 360) * (PWMHUEWHEELMAX));

}

int RGBWWLed::parseSat(float sat) {
	sat = constrain(sat, 0.0, 100.0);
	return int((sat / 100) * (PWMMAXVAL));
}

int RGBWWLed::parseVal(float val){
	val = constrain(val, 0.0, 100.0);
	return int((val / 100) * (PWMMAXVAL));
}



