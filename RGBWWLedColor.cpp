/**
 * RGBWWLed - simple Library for controlling RGB WarmWhite ColdWhite LEDs via PWM
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * All files of this project are provided under the LGPL v3 license.
 */
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"


RGBWWColorUtils::RGBWWColorUtils() {
	_colormode = RGBWWCW;
	_hsvmodel = NORMAL;
	_WarmWhiteKelvin = RGBWW_WARMWHITEKELVIN;
	_ColdWhiteKelvin = RGBWW_COLDWHITEKELVIN;
	createHueWheel();
	setBrightnessCorrection(RGBWW_PWMWIDTH, RGBWW_PWMWIDTH,
			RGBWW_PWMWIDTH, RGBWW_PWMWIDTH, RGBWW_PWMWIDTH);

 }


void RGBWWColorUtils::setColorMode(RGBWW_COLORMODE mode) {
	debugRGBW("COLORMODE %i", mode);
	_colormode = mode;
}


RGBWW_COLORMODE RGBWWColorUtils::getColorMode() {
	debugRGBW("COLORMODE %i", _colormode);
	return _colormode;
}


void RGBWWColorUtils::setHSVmodel(RGBWW_HSVMODEL model) {
	debugRGBW("HSVMODE %i", model);
	_hsvmodel = model;
}


RGBWW_HSVMODEL RGBWWColorUtils::getHSVmodel() {
	debugRGBW("HSVMODE %i", _hsvmodel);
	return _hsvmodel;
}


void RGBWWColorUtils::setWhiteTemperature(int WarmWhite, int ColdWhite) {
	_WarmWhiteKelvin = WarmWhite;
	_ColdWhiteKelvin = ColdWhite;
}


void RGBWWColorUtils::getWhiteTemperature(int& WarmWhite, int& ColdWhite) {
	WarmWhite = _WarmWhiteKelvin;
	ColdWhite = _ColdWhiteKelvin;
}


void RGBWWColorUtils::setBrightnessCorrection(int r, int g, int b, int ww, int cw) {
	_BrightnessFactor[RGBWW_COLORS::RED] = (constrain(r, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::GREEN] = (constrain(g, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::BLUE] = (constrain(b, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::WW] = (constrain(ww, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::CW] = (constrain(cw, 0, 100)/100) * RGBWW_PWMWIDTH;

};


void RGBWWColorUtils::getBrightnessCorrection(int& r, int& g, int& b, int& ww, int& cw) {
	r = _BrightnessFactor[RGBWW_COLORS::RED]/RGBWW_PWMWIDTH;
	g = _BrightnessFactor[RGBWW_COLORS::GREEN]/RGBWW_PWMWIDTH;
	b = _BrightnessFactor[RGBWW_COLORS::BLUE]/RGBWW_PWMWIDTH;
	ww = _BrightnessFactor[RGBWW_COLORS::WW]/RGBWW_PWMWIDTH;
	cw = _BrightnessFactor[RGBWW_COLORS::CW]/RGBWW_PWMWIDTH;
}



void RGBWWColorUtils::correctBrightness(int color[]) {
	color[RGBWW_COLORS::RED] = (color[RGBWW_COLORS::RED] * _BrightnessFactor[RGBWW_COLORS::RED]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::GREEN] = (color[RGBWW_COLORS::GREEN] * _BrightnessFactor[RGBWW_COLORS::GREEN]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::BLUE] = (color[RGBWW_COLORS::BLUE] * _BrightnessFactor[RGBWW_COLORS::BLUE]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::WW] = (color[RGBWW_COLORS::WW]* _BrightnessFactor[RGBWW_COLORS::WW]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::CW] = (color[RGBWW_COLORS::CW] * _BrightnessFactor[RGBWW_COLORS::CW]) >> RGBWW_PWMDEPTH;
}


void RGBWWColorUtils::setHSVcorrection(float red, float yellow, float green, float cyan, float blue, float magenta) {
	// reset color wheel before applying any changes
	// otherwise we apply changes to any previous colorwheel
	createHueWheel();

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

}


void RGBWWColorUtils::getHSVcorrection(float& red, float& yellow, float& green, float& cyan, float& blue, float& magenta) {
	red = -1 * (float(_HueWheelSector[6] - 6*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	yellow = -1 * (float(_HueWheelSector[1] - 1*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	green = -1 * (float(_HueWheelSector[2] - 2*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	cyan = -1 * (float(_HueWheelSector[3] - 3*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	blue = -1 * (float(_HueWheelSector[4] - 4*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	magenta = -1 * (float(_HueWheelSector[5] - 5*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;

}


 void RGBWWColorUtils::whiteBalance(RGBWK& rgbw, int& ww, int& cw) {
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
		rgbw.r += rgbw.w;
		rgbw.g += rgbw.w;
		rgbw.b += rgbw.w;
		cw = 0;
		ww = 0;
		break;
	}
 }


void RGBWWColorUtils::HSVtoRGB(const HSVK& hsvk, RGBWK& rgbwk) {
	HSVtoRGB(hsvk, rgbwk, _hsvmodel);
}


void RGBWWColorUtils::HSVtoRGB(const HSVK& hsvk, RGBWK& rgbwk, RGBWW_HSVMODEL mode) {

	switch(mode) {
	case SPEKTRUM: HSVtoRGBspektrum(hsvk, rgbwk); break;
	case RAINBOW: HSVtoRGBrainbow(hsvk, rgbwk); break;
	default: HSVtoRGBn(hsvk, rgbwk); break;
	}

}


void RGBWWColorUtils::HSVtoRGBspektrum(const HSVK& hsvk, RGBWK& rgbwk) {
	//TODO: implement linear spectrum
	HSVtoRGBn(hsvk, rgbwk);
}



void RGBWWColorUtils::HSVtoRGBrainbow(const HSVK& hsvk, RGBWK& rgbwk) {
	//TODO: implement rainbow spectrum
	HSVtoRGBn(hsvk, rgbwk);
}



void RGBWWColorUtils::HSVtoRGBn(const HSVK& hsvk, RGBWK& rgbw) {
	int val, hue, sat, r, g, b, fract, chroma, m;

	hue = hsvk.h;

	//gamma correction
	val = RGBWW_dim_curve[hsvk.v];
	sat = hsvk.s;
	sat = RGBWW_PWMMAXVAL-RGBWW_dim_curve[RGBWW_PWMMAXVAL-sat];
	//sat = hsv.s;

	rgbw.k = hsvk.k;
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
		rgbw.r = r;
		rgbw.g = g;
		rgbw.b = b;
		rgbw.w = m;

	}
	debugRGBW("R %i | G %i | B %i | W %i", rgbw.r, rgbw.g, rgbw.b, rgbw.w);
	debugRGBW("==  //HSV2RGB  ==");
}


void  RGBWWColorUtils::RGBtoHSV(const RGBWK& rgbw, HSVK& hsv) {
	debugRGBW("RGBtoHSV");


};


/*
 * Helper function to create the 6 sectors for the HUE wheel
 */
void RGBWWColorUtils::createHueWheel() {
	_HueWheelSector[0] = 0;
	for (int i = 1; i <= 6; ++i) {
		_HueWheelSector[i] = i*RGBWW_PWMMAXVAL;
		_HueWheelSectorWidth[i-1] = RGBWW_PWMMAXVAL;
	}
}


void RGBWWColorUtils::circleHue(int& hue ) {
	while (hue >= RGBWW_PWMHUEWHEELMAX) hue -= RGBWW_PWMHUEWHEELMAX;
	while (hue < 0) hue += RGBWW_PWMHUEWHEELMAX;

}

/*
 * Helper functions to parse hue,sat,val,colorcorrection
 * Converts to the according pwm size (8bit/10bit)
 */
int RGBWWColorUtils::parseColorCorrection(float val) {
	if (val >= 30.0) val = 30.0;
	if (val <= -30.0) val = -30.0;
	return int(((val / 60) * (RGBWW_PWMMAXVAL)) * -1);
}


