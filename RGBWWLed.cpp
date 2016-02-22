/**************************************************************


    @author Patrick Jahns
 **************************************************************/
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"
#include "RGBWWLedAnimation.h"
#include "RGBWWLedOutput.h"



/**************************************************************
                setup, init and settings
 **************************************************************/

RGBWWLed::RGBWWLed() {
	_isAnimationActive = false;
	_cancelAnimation = false;
	_clearAnimationQueue = false;
	_colormode = RGB;
	_current_color = HSVK(0, 0, 0);
	_currentAnimation = NULL;
	_hsvmode = NORMAL;
	_animationQ = new RGBWWLedAnimationQ(ANIMATIONQSIZE);
	_pwm_output = NULL;
	createHueWheel();
	setBrightnessCorrection(PWMWIDTH, PWMWIDTH, PWMWIDTH, PWMWIDTH, PWMWIDTH);
}


void RGBWWLed::init(int redPIN, int greenPIN, int bluePIN, int wwPIN, int cwPIN, int pwmFrequency) {

	_pwm_output = new PWMOutput(redPIN, greenPIN, bluePIN, wwPIN, cwPIN, pwmFrequency);

}

/**
    Change Mode for color calculations
 */

void RGBWWLed::setColorMode(COLORMODE m) {
	_colormode = m;
}

COLORMODE RGBWWLed::getColorMode() {
	return _colormode;
}
/**

    Change HSV to RGBW calculation Model

 */
void RGBWWLed::setHSVmode(HSVMODE m) {
	_hsvmode = m;
}

HSVMODE RGBWWLed::getHSVmode() {
	return _hsvmode;
}


/**
    Correct the Maximum brightness of a channel

    @param r red channel (0.0 - 1.0)
    @param g green channel (0.0 - 1.0)
    @param b blue channel (0.0 - 1.0)
    @param cw cold white channel (0.0 - 1.0)
    @param wm warm white channel (0.0 - 1.0)

 */
void RGBWWLed::setBrightnessCorrection(float r, float g, float b, float ww, float cw) {
	_BrightnessFactor[0] = constrain(r, 0.0, 1.0) * PWMWIDTH;
	_BrightnessFactor[1] = constrain(g, 0.0, 1.0) * PWMWIDTH;
	_BrightnessFactor[2] = constrain(b, 0.0, 1.0) * PWMWIDTH;
	_BrightnessFactor[3] = constrain(cw, 0.0, 1.0) * PWMWIDTH;
	_BrightnessFactor[4] = constrain(ww, 0.0, 1.0) * PWMWIDTH;

};

void RGBWWLed::getBrightnessCorrection(float& r, float& g, float& b, float& ww, float& cw) {
	r = float(_BrightnessFactor[0]/PWMWIDTH);
	g = float(_BrightnessFactor[1]/PWMWIDTH);
	b = float(_BrightnessFactor[2]/PWMWIDTH);
	ww = float(_BrightnessFactor[3]/PWMWIDTH);
	cw = float(_BrightnessFactor[4]/PWMWIDTH);
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
	//TODO: write unit test
	createHueWheel();
	/*
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
	*/
}

void RGBWWLed::getHSVcorrection(float& red, float& yellow, float& green, float& cyan, float& blue, float& magenta) {
	red = -1 * (float(_HueWheelSector[6] - 6*PWMMAXVAL)/ float(PWMMAXVAL)) * 60.0;
	yellow = -1 * (float(_HueWheelSector[1] - 1*PWMMAXVAL)/ float(PWMMAXVAL)) * 60.0;
	green = -1 * (float(_HueWheelSector[2] - 2*PWMMAXVAL)/ float(PWMMAXVAL)) * 60.0;
	cyan = -1 * (float(_HueWheelSector[3] - 3*PWMMAXVAL)/ float(PWMMAXVAL)) * 60.0;
	blue = -1 * (float(_HueWheelSector[4] - 4*PWMMAXVAL)/ float(PWMMAXVAL)) * 60.0;
	magenta = -1 * (float(_HueWheelSector[5] - 5*PWMMAXVAL)/ float(PWMMAXVAL)) * 60.0;

}

/**************************************************************
                    OUTPUT
 **************************************************************/

void RGBWWLed::refresh() {
	setOutput(getCurrenctColor());
}
 
 
HSVK RGBWWLed::getCurrenctColor() {
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
	//TODO white correction
	color[0] = (c.r * _BrightnessFactor[0]) >> PWMDEPTH;
	color[1] = (c.g * _BrightnessFactor[1]) >> PWMDEPTH;
	color[2] = (c.b * _BrightnessFactor[2]) >> PWMDEPTH;
	color[3] = (c.w * _BrightnessFactor[3]) >> PWMDEPTH;
	color[4] = (c.w * _BrightnessFactor[4]) >> PWMDEPTH;
	setOutputRaw(color[0], color[1], color[2], color[3], color[4]);
}



/**
    Directly set the PWM values without color correction or white balance

 */
void RGBWWLed::setOutputRaw(int red, int green, int blue, int wwhite, int cwhite) {
	_pwm_output->setOutput(red, green, blue, wwhite, cwhite);

}


/**************************************************************
                ANIMATION/TRANSITION
 **************************************************************/


/**
    Main function responsible for handling animations
 */
bool RGBWWLed::show() {


	// check if we need to cancel effect
	if (_cancelAnimation || _clearAnimationQueue) {

		cleanupCurrentAnimation();
		_cancelAnimation = false;

		// cleanup Q if we cancel all effects
		if (_clearAnimationQueue) {

			clearAnimationQ();
			_clearAnimationQueue = false;
		}
	}

#ifdef ESP8266
	//TODO: make this look nicer for SMING
	long now = millis();
	if (now - last_active < MINTIMEDIFF) {
		// Interval hasn't passed yet
		return true;
	}

	last_active = now;
#endif // ESP8266
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
		//DEBUG("finished animation");
		cleanupCurrentAnimation();
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
	setHSV( color, 0, 1, false);
}

void RGBWWLed::setHSV(HSVK& color, int time, bool q) {
	setHSV( color, time, 1, q);
}


void RGBWWLed::setHSV(HSVK& color, int time, int direction) {
	// get current value and then move forward
	setHSV( color, time, direction, false);
}

void RGBWWLed::setHSV(HSVK& color, int time, int direction, bool q) {
	if (time == 0 || time < MINTIMEDIFF) {
		// no transition time - directly set the color
		setOutput(color);
	} else {
		// Fix fading from off to on with different HUE
		//colorFrom.h = (colorFrom.v == 0 && color.h != colorFrom.h) ? color.h : colorFrom.h;
		if (q) {
			// using queue
			_animationQ->push(new HSVTransition(color, time, direction, this));

		} else {

			// not using queue up -> clear the queue first
			clearAnimationQ();
			// clear any running animation
			cleanupCurrentAnimation();
			_currentAnimation = new HSVTransition(color, time, direction, this);
			_isAnimationActive = true;
		}

	}
}

void RGBWWLed::setHSV(HSVK& colorFrom, HSVK& color, int time, int direction, bool q ) {
	// only change color if it is different
	if (colorFrom.h != color.h || colorFrom.s != color.s || colorFrom.v != color.v  || colorFrom.k != color.k  ) {
		if (time == 0 || time < MINTIMEDIFF) {
			// no transition time - directly set the color
			setOutput(color);
		} else {
			// Fix fading from off to on with different HUE
			//colorFrom.h = (colorFrom.v == 0 && color.h != colorFrom.h) ? color.h : colorFrom.h;
			if (q) {
				// using queue
				_animationQ->push(new HSVTransition(colorFrom, color, time, direction, this));

			} else {

				// not using queue up -> clear the queue first
				clearAnimationQ();
				// clear any running animation
				cleanupCurrentAnimation();
				_currentAnimation = new HSVTransition(colorFrom, color, time, direction, this);
				_isAnimationActive = true;
			}

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

void RGBWWLed::clearAnimationQ() {
	while(!_animationQ->isEmpty()) {
		RGBWWLedAnimation* animation = _animationQ->pop();
		delete animation;
	}
}

bool RGBWWLed::isAnimationQFull() {
	return _animationQ->isFull();
}

/**************************************************************
                    COLORUTILS
 **************************************************************/



void RGBWWLed::HSVtoRGB(const HSVK& hsv, RGBWK& rgbw) {
	HSVtoRGB(hsv, rgbw, _hsvmode);
}

void RGBWWLed::HSVtoRGB(const HSVK& hsv, RGBWK& rgbw, HSVMODE m) {
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
	//TODO: write unit test

	hue = hsv.h;
	//gamma correction
	val = dim_curve[hsv.v];
	sat = hsv.s;
	sat = PWMMAXVAL-dim_curve[PWMMAXVAL-sat];
	//sat = hsv.s;

	rgbw.k = hsv.k;
	//DEBUG("==  HSV2RGB  ==");
	//DEBUG("HUE ", hue);
	//DEBUG("SAT ", sat);
	//DEBUG("VAL ", val);
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
		chroma = (sat * val)/PWMMAXVAL;
		m = val - chroma;
		//DEBUG("CHR ",chroma);
		//DEBUG("M   ", m);
		//DEBUG("===============");
		if ( hue < _HueWheelSector[0] || (hue > _HueWheelSector[5] && hue <= _HueWheelSector[6])) {
			//DEBUG("Sector 6");
			if (hue < _HueWheelSector[0]) {
				fract = PWMMAXVAL + hue ;
			} else {
				fract = hue - _HueWheelSector[5];
			}

			r = chroma;
			g = 0;
			//DEBUG(fract);
			//DEBUG(_HueWheelSectorWidth[5]);
			b = ( chroma * (PWMMAXVAL - (PWMMAXVAL * fract) / _HueWheelSectorWidth[5])) >> PWMDEPTH;

		} else if (  hue <= _HueWheelSector[1]  || hue > _HueWheelSector[6]) {
			// Sector 1
			//DEBUG("Sector 1");
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
			//DEBUG("Sector 2");
			fract = hue - _HueWheelSector[1];
			r = (chroma * (PWMMAXVAL - (PWMMAXVAL * fract) / _HueWheelSectorWidth[1])) >> PWMDEPTH;
			g = chroma;
			b = 0;

		} else if (hue <= _HueWheelSector[3]) {
			// Sector 3
			//DEBUG("Sector 3");
			fract = hue - _HueWheelSector[2];
			r = 0;
			g = chroma;
			b = (chroma * ((PWMMAXVAL * fract) / _HueWheelSectorWidth[2])) >> PWMDEPTH;

		} else if (hue <= _HueWheelSector[4]) {
			// Sector 4
			//DEBUG("Sector 4");
			fract = hue - _HueWheelSector[3];
			r = 0;
			g =(chroma * (PWMMAXVAL - (PWMMAXVAL * fract) / _HueWheelSectorWidth[3])) >> PWMDEPTH;
			b = chroma;

		} else  {
			// Sector 5
			//DEBUG("Sector 5");
			fract = hue - _HueWheelSector[4];
			r = (chroma * ((PWMMAXVAL * fract) / _HueWheelSectorWidth[4])) >> PWMDEPTH;
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

			//DEBUG("===============");
			//DEBUG("R ", rgbw.r);
			//DEBUG("G ", rgbw.g);
			//DEBUG("B ", rgbw.b);
			//DEBUG("===============");
		} else {
			rgbw.r = r;
			rgbw.g = g;
			rgbw.b = b;
			rgbw.w = m;

			//DEBUG("===============");
			//DEBUG("R ", rgbw.r);
			//DEBUG("G ", rgbw.g);
			//DEBUG("B ", rgbw.b);
			//DEBUG("W ", rgbw.w);
			//DEBUG("===============");
		}
	}
	//DEBUG("==  //HSV2RGB  ==");
}

void  RGBWWLed::RGBtoHSV(const RGBWK& rgbw, HSVK& hsv) {
	//DEBUG("RGBtoHSV");

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



