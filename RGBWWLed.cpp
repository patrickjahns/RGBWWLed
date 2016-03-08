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
	_colormode = RGB;
	_current_color = HSVK(0, 0, 0);
	_hsvmode = NORMAL;
	_WarmWhiteKelvin = RGBWW_WARMWHITEKELVIN;
	_ColdWhiteKelvin = RGBWW_COLDWHITEKELVIN;
	_currentAnimation = NULL;
	_animationQ = new RGBWWLedAnimationQ(RGBWW_ANIMATIONQSIZE);
	_pwm_output = NULL;
	createHueWheel();
	setBrightnessCorrection(RGBWW_PWMWIDTH, RGBWW_PWMWIDTH, RGBWW_PWMWIDTH, RGBWW_PWMWIDTH, RGBWW_PWMWIDTH);
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

/**
 *  Change Mode for color calculations
 */

void RGBWWLed::setColorMode(RGBWW_COLORMODE m) {
	_colormode = m;
}

RGBWW_COLORMODE RGBWWLed::getColorMode() {
	return _colormode;
}
/**
 *
 *  Change HSV to RGBW calculation Model
 *
 */
void RGBWWLed::setHSVmode(RGBWW_HSVMODE m) {
	_hsvmode = m;
}

RGBWW_HSVMODE RGBWWLed::getHSVmode() {
	return _hsvmode;
}


/**
 *     Correct the Maximum brightness of a channel
 *
 *     @param r red channel (0.0 - 1.0)
 *    @param g green channel (0.0 - 1.0)
 *    @param b blue channel (0.0 - 1.0)
 *    @param cw cold white channel (0.0 - 1.0)
 *    @param wm warm white channel (0.0 - 1.0)
 *
 */
void RGBWWLed::setBrightnessCorrection(int r, int g, int b, int ww, int cw) {
	_BrightnessFactor[0] = (constrain(r, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[1] = (constrain(g, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[2] = (constrain(b, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[3] = (constrain(cw, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[4] = (constrain(ww, 0, 100)/100) * RGBWW_PWMWIDTH;

};


void RGBWWLed::getBrightnessCorrection(int& r, int& g, int& b, int& ww, int& cw) {
	r = _BrightnessFactor[0]/RGBWW_PWMWIDTH;
	g = _BrightnessFactor[1]/RGBWW_PWMWIDTH;
	b = _BrightnessFactor[2]/RGBWW_PWMWIDTH;
	ww = _BrightnessFactor[3]/RGBWW_PWMWIDTH;
	cw = _BrightnessFactor[4]/RGBWW_PWMWIDTH;
}


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

void RGBWWLed::setHSVcorrection(float red, float yellow, float green, float cyan, float blue, float magenta) {
	// reset color wheel before applying any changes
	// otherwise we apply changes to any previous colorwheel
	createHueWheel();
	/*
	debugRGBW("==  correctHSV  ==");
	debugRGBW("--  before      --");
	debugRGBW("W0 %i", _HueWheelSectorWidth[0]);
	debugRGBW("W1 %i", _HueWheelSectorWidth[1]);
	debugRGBW("W2 %i", _HueWheelSectorWidth[2]);
	debugRGBW("W3 %i", _HueWheelSectorWidth[3]);
	debugRGBW("W4 %i", _HueWheelSectorWidth[4]);
	debugRGBW("W5 %i", _HueWheelSectorWidth[5]);
	debugRGBW(" ");
	debugRGBW("B0 %i", _HueWheelSector[0]);
	debugRGBW("B1 %i", _HueWheelSector[1]);
	debugRGBW("B2 %i", _HueWheelSector[2]);
	debugRGBW("B3 %i", _HueWheelSector[3]);
	debugRGBW("B4 %i", _HueWheelSector[4]);
	debugRGBW("B5 %i", _HueWheelSector[5]);
	debugRGBW("B6 %i", _HueWheelSector[6]);
	debugRGBW(" ");
	*/
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

	/*
	debugRGBW("--  after      --");
	debugRGBW("W0 %i", _HueWheelSectorWidth[0]);
	debugRGBW("W1 %i", _HueWheelSectorWidth[1]);
	debugRGBW("W2 %i", _HueWheelSectorWidth[2]);
	debugRGBW("W3 %i", _HueWheelSectorWidth[3]);
	debugRGBW("W4 %i", _HueWheelSectorWidth[4]);
	debugRGBW("W5 %i", _HueWheelSectorWidth[5]);
	debugRGBW(" ");
	debugRGBW("B0 %i", _HueWheelSector[0]);
	debugRGBW("B1 %i", _HueWheelSector[1]);
	debugRGBW("B2 %i", _HueWheelSector[2]);
	debugRGBW("B3 %i", _HueWheelSector[3]);
	debugRGBW("B4 %i", _HueWheelSector[4]);
	debugRGBW("B5 %i", _HueWheelSector[5]);
	debugRGBW("B6 %i", _HueWheelSector[6]);
	debugRGBW("==  //correctHSV  ==");
	debugRGBW(" ");
	*/
}

void RGBWWLed::getHSVcorrection(float& red, float& yellow, float& green, float& cyan, float& blue, float& magenta) {
	red = -1 * (float(_HueWheelSector[6] - 6*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	yellow = -1 * (float(_HueWheelSector[1] - 1*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	green = -1 * (float(_HueWheelSector[2] - 2*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	cyan = -1 * (float(_HueWheelSector[3] - 3*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	blue = -1 * (float(_HueWheelSector[4] - 4*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	magenta = -1 * (float(_HueWheelSector[5] - 5*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;

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
	HSVtoRGB(color, rgbw);
	setOutput(rgbw);

}

void RGBWWLed::setOutput(RGBWK c) {
	int color[5];
	int ww, cw;
	whiteBalance(c, ww, cw);
	color[0] = (c.r * _BrightnessFactor[0]) >> RGBWW_PWMDEPTH;
	color[1] = (c.g * _BrightnessFactor[1]) >> RGBWW_PWMDEPTH;
	color[2] = (c.b * _BrightnessFactor[2]) >> RGBWW_PWMDEPTH;
	color[3] = (ww * _BrightnessFactor[3]) >> RGBWW_PWMDEPTH;
	color[4] = (cw * _BrightnessFactor[4]) >> RGBWW_PWMDEPTH;
	setOutputRaw(color[0], color[1], color[2], color[3], color[4]);
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

/**************************************************************
 *                     COLORUTILS
 **************************************************************/

 void RGBWWLed::whiteBalance(RGBWK& rgbw, int& ww, int& cw) {
	switch(_colormode) {
	case RGBWWCW:
		if (_WarmWhiteKelvin <= rgbw.k && _ColdWhiteKelvin >= rgbw.k) {
			int wwfactor = ((_ColdWhiteKelvin - rgbw.k) << RGBWW_PWMDEPTH) /  (_ColdWhiteKelvin - _WarmWhiteKelvin);
			//balance between CW and WW Leds
			ww = (rgbw.w * wwfactor) >> RGBWW_PWMDEPTH;
			cw = (rgbw.w * (1 - wwfactor)) >> RGBWW_PWMDEPTH;
		} else {
			// if kelvin outside range - different calculation algorithm
			// for now we asume a neutral white (i.e 0.5 CW, 0.5 WW)
			ww = rgbw.w/2;
			cw = rgbw.w/2;
		}
		break;
	case RGBCW:
		//TODO implement valid algorithm
		cw = rgbw.w;
		ww = 0;
		break;
		
	case RGBWW:
		//TODO implement valid algorithm
		cw = 0;
		ww = rgbw.w;
		break;
		
	case RGB:
		//TODO implement valid algorithm
		cw = 0;
		ww = 0;
		break;
	}
 }


void RGBWWLed::HSVtoRGB(const HSVK& hsv, RGBWK& rgbw) {
	HSVtoRGB(hsv, rgbw, _hsvmode);
}

void RGBWWLed::HSVtoRGB(const HSVK& hsv, RGBWK& rgbw, RGBWW_HSVMODE m) {
	switch(m) {
	case SPEKTRUM: HSVtoRGBspektrum(hsv, rgbw); break;
	case RAINBOW: HSVtoRGBrainbow(hsv, rgbw); break;
	default: HSVtoRGBn(hsv, rgbw); break;
	}

}

/**
    Convert HSV values to RGB colorspace by keeping
    the max total color output equal.
    Information see:
    https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors

 */

void RGBWWLed::HSVtoRGBspektrum(const HSVK& hsv, RGBWK& rgbw) {
	//TODO: implement linear spectrum
	HSVtoRGBn(hsv, rgbw);
}

/**
    Convert HSV values to RGB colorspace with rainbow color table
    Information see:
    https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors

 */
void RGBWWLed::HSVtoRGBrainbow(const HSVK& hsv, RGBWK& rgbw) {
	//TODO: implement rainbow spectrum
	HSVtoRGBn(hsv, rgbw);
}

/**
    Convert HSV values to RGB colorspace standard algorithm
    Mor Information on the "standard" conversion:
    https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV

 */
void RGBWWLed::HSVtoRGBn(const HSVK& hsv, RGBWK& rgbw) {
	int val, hue, sat, r, g, b, fract, chroma, m;

	hue = hsv.h;
	//gamma correction
	val = dim_curve[hsv.v];
	sat = hsv.s;
	sat = RGBWW_PWMMAXVAL-dim_curve[RGBWW_PWMMAXVAL-sat];
	//sat = hsv.s;

	rgbw.k = hsv.k;
	debugRGBW("==  HSV2RGB  ======");

	if(sat == 0) {
		/* color is grayscale */
		if (_colormode == RGB) {
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
		chroma = (sat * val)/RGBWW_PWMMAXVAL;
		m = val - chroma;
		if ( hue < _HueWheelSector[0] || (hue > _HueWheelSector[5] && hue <= _HueWheelSector[6])) {
			debugRGBW("Sector 6");
			if (hue < _HueWheelSector[0]) {
				fract = RGBWW_PWMMAXVAL + hue ;
			} else {
				fract = hue - _HueWheelSector[5];
			}

			r = chroma;
			g = 0;
			b = ( chroma * (RGBWW_PWMMAXVAL - (RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[5])) >> RGBWW_PWMDEPTH;

		} else if (  hue <= _HueWheelSector[1]  || hue > _HueWheelSector[6]) {
			// Sector 1
			debugRGBW("Sector 1");
			if (hue > _HueWheelSector[6]) {
				fract = hue - _HueWheelSector[6];
			} else {
				fract = hue + (RGBWW_PWMHUEWHEELMAX - _HueWheelSector[6]);
			}
			r = chroma;
			g = (chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[0])) >> RGBWW_PWMDEPTH;
			b = 0;

		} else if (hue <= _HueWheelSector[2]) {
			// Sector 2
			debugRGBW("Sector 2");
			fract = hue - _HueWheelSector[1];
			r = (chroma * (RGBWW_PWMMAXVAL - (RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[1])) >> RGBWW_PWMDEPTH;
			g = chroma;
			b = 0;

		} else if (hue <= _HueWheelSector[3]) {
			// Sector 3
			debugRGBW("Sector 3");
			fract = hue - _HueWheelSector[2];
			r = 0;
			g = chroma;
			b = (chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[2])) >> RGBWW_PWMDEPTH;

		} else if (hue <= _HueWheelSector[4]) {
			// Sector 4
			debugRGBW("Sector 4");
			fract = hue - _HueWheelSector[3];
			r = 0;
			g =(chroma * (RGBWW_PWMMAXVAL - (RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[3])) >> RGBWW_PWMDEPTH;
			b = chroma;

		} else  {
			// Sector 5
			debugRGBW("Sector 5");
			fract = hue - _HueWheelSector[4];
			r = (chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[4])) >> RGBWW_PWMDEPTH;
			g = 0;
			b = chroma;

		}
		// m equals the white part
		// for rgbw we use it for the white channels
		if (_colormode == RGB) {
			rgbw.r = r + m;
			rgbw.g = g + m;
			rgbw.b = b + m;
			rgbw.w = 0; // otherwise might be undefined!

			debugRGBW("----------------");
			debugRGBW("R i%", rgbw.r);
			debugRGBW("G i%", rgbw.g);
			debugRGBW("B i%", rgbw.b);
			debugRGBW("----------------");
		} else {
			rgbw.r = r;
			rgbw.g = g;
			rgbw.b = b;
			rgbw.w = m;

			debugRGBW("----------------");
			debugRGBW("R i%", rgbw.r);
			debugRGBW("G i%", rgbw.g);
			debugRGBW("B i%", rgbw.b);
			debugRGBW("W i%", rgbw.w);
			debugRGBW("----------------");
		}
	}
	debugRGBW("==  //HSV2RGB  ==");
}

void  RGBWWLed::RGBtoHSV(const RGBWK& rgbw, HSVK& hsv) {
	debugRGBW("RGBtoHSV");

};

/**************************************************************
 *                 HELPER FUNCTIONS
 **************************************************************/


/*
Helper function to create the 6 sectors for the HUE wheel
 */
void RGBWWLed::createHueWheel() {
	_HueWheelSector[0] = 0;
	for (int i = 1; i <= 6; ++i) {
		_HueWheelSector[i] = i*RGBWW_PWMMAXVAL;
		_HueWheelSectorWidth[i-1] = RGBWW_PWMMAXVAL;
	}
}

/*
Helper function to keep Hue between 0 - HueWheelMax
 */
void RGBWWLed::circleHue(int& hue ) {
	while (hue >= RGBWW_PWMHUEWHEELMAX) hue -= RGBWW_PWMHUEWHEELMAX;
	while (hue < 0) hue += RGBWW_PWMHUEWHEELMAX;

}

/*
Helper functions to parse hue,sat,val,colorcorrection
Converts to the according pwm size (8bit/10bit)
 */
int RGBWWLed::parseColorCorrection(float val) {
	if (val >= 29.5) val = 29.5;
	if (val <= -29.5) val = -29.5;
	return int(((val / 60) * (RGBWW_PWMMAXVAL)) * -1);
}

int RGBWWLed::parseHue(float hue) {
	hue = constrain(hue, 0.0, 360.0);
	return int((hue / 360) * (RGBWW_PWMHUEWHEELMAX));

}

int RGBWWLed::parseSat(float sat) {
	sat = constrain(sat, 0.0, 100.0);
	return int((sat / 100) * (RGBWW_PWMMAXVAL));
}

int RGBWWLed::parseVal(float val){
	val = constrain(val, 0.0, 100.0);
	return int((val / 100) * (RGBWW_PWMMAXVAL));
}



