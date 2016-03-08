/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
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
 * @brief	Some utils used for working with different colortables (i.e HSV, RGB..)
 * @section DESCRIPTION
 *
 *
 */
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"

/**
 *
 */
RGBWWColorUtils::RGBWWColorUtils() {
	_colormode = RGB;
	_hsvmode = NORMAL;
	_WarmWhiteKelvin = RGBWW_WARMWHITEKELVIN;
	_ColdWhiteKelvin = RGBWW_COLDWHITEKELVIN;
	createHueWheel();
	setBrightnessCorrection(RGBWW_PWMWIDTH, RGBWW_PWMWIDTH,
			RGBWW_PWMWIDTH, RGBWW_PWMWIDTH, RGBWW_PWMWIDTH);

 }


/**
 *
 * @param mode
 */

void RGBWWColorUtils::setColorMode(RGBWW_COLORMODE mode) {
	_colormode = mode;
}

/**
 *
 * @return
 */
RGBWW_COLORMODE RGBWWColorUtils::getColorMode() {
	return _colormode;
}

/**
 *
 *
 * @param mode
 */
void RGBWWColorUtils::setHSVmode(RGBWW_HSVMODE mode) {
	_hsvmode = mode;
}

/**
 *
 * @return
 */
RGBWW_HSVMODE RGBWWColorUtils::getHSVmode() {
	return _hsvmode;
}

/**
 *
 * @param WarmWhite
 * @param ColdWhite
 */
void RGBWWColorUtils::setWhiteTemperature(int WarmWhite, int ColdWhite) {
	_WarmWhiteKelvin = WarmWhite;
	_ColdWhiteKelvin = ColdWhite;
}

/**
 *
 * @param WarmWhite
 * @param ColdWhite
 */
void RGBWWColorUtils::getWhiteTemperature(int& WarmWhite, int& ColdWhite) {
	WarmWhite = _WarmWhiteKelvin;
	ColdWhite = _ColdWhiteKelvin;
}

/**
 * Set the maximum brightness if output for the channels
 * Assumes that r,g,b,cw and ww are contained in the
 * set [0 , 100]
 *
 * @param int	r	red channel
 * @param int 	g	green channel
 * @param int	b	blue channel
 * @param int 	cw 	cold white channel
 * @param int 	ww 	warm white channel (
 *
 */
void RGBWWColorUtils::setBrightnessCorrection(int r, int g, int b, int ww, int cw) {
	_BrightnessFactor[RGBWW_COLORS::RED] = (constrain(r, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::GREEN] = (constrain(g, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::BLUE] = (constrain(b, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::WW] = (constrain(ww, 0, 100)/100) * RGBWW_PWMWIDTH;
	_BrightnessFactor[RGBWW_COLORS::CW] = (constrain(cw, 0, 100)/100) * RGBWW_PWMWIDTH;

};

/**
 * Copies the current value of the brightness factor into the specified variables
 *
 * @param int&	r
 * @param int&	g
 * @param int&	b
 * @param int&	ww
 * @param int&	cw
 */
void RGBWWColorUtils::getBrightnessCorrection(int& r, int& g, int& b, int& ww, int& cw) {
	r = _BrightnessFactor[RGBWW_COLORS::RED]/RGBWW_PWMWIDTH;
	g = _BrightnessFactor[RGBWW_COLORS::GREEN]/RGBWW_PWMWIDTH;
	b = _BrightnessFactor[RGBWW_COLORS::BLUE]/RGBWW_PWMWIDTH;
	ww = _BrightnessFactor[RGBWW_COLORS::WW]/RGBWW_PWMWIDTH;
	cw = _BrightnessFactor[RGBWW_COLORS::CW]/RGBWW_PWMWIDTH;
}


/**
 * Corrects the values in the parsed array according to the set
 * color correction
 *
 * @param int[] color
 */
void RGBWWColorUtils::correctBrightness(int color[]) {
	color[RGBWW_COLORS::RED] = (color[RGBWW_COLORS::RED] * _BrightnessFactor[RGBWW_COLORS::RED]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::GREEN] = (color[RGBWW_COLORS::GREEN] * _BrightnessFactor[RGBWW_COLORS::GREEN]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::BLUE] = (color[RGBWW_COLORS::BLUE] * _BrightnessFactor[RGBWW_COLORS::BLUE]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::WW] = (color[RGBWW_COLORS::WW]* _BrightnessFactor[RGBWW_COLORS::WW]) >> RGBWW_PWMDEPTH;
	color[RGBWW_COLORS::CW] = (color[RGBWW_COLORS::CW] * _BrightnessFactor[RGBWW_COLORS::CW]) >> RGBWW_PWMDEPTH;
}

/**
 * Correction for HSVtoRGB Normal Mode. Moves the boundaries
 * of each color further left/right. Assumes all variables are
 * contained in [-30.0, 30.0]
 *
 * @param float	red
 * @param float	yellow
 * @param float	green
 * @param float	cyan
 * @param float	blue
 * @param float	magenta
 */

void RGBWWColorUtils::setHSVcorrection(float red, float yellow, float green, float cyan, float blue, float magenta) {
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

/**
 * Copies the current values used for HSV correction into the
 * provided params
 *
 * @param float&	red
 * @param float&	yellow
 * @param float&	green
 * @param float&	cyan
 * @param float&	blue
 * @param float&	magenta
 */
void RGBWWColorUtils::getHSVcorrection(float& red, float& yellow, float& green, float& cyan, float& blue, float& magenta) {
	red = -1 * (float(_HueWheelSector[6] - 6*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	yellow = -1 * (float(_HueWheelSector[1] - 1*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	green = -1 * (float(_HueWheelSector[2] - 2*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	cyan = -1 * (float(_HueWheelSector[3] - 3*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	blue = -1 * (float(_HueWheelSector[4] - 4*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	magenta = -1 * (float(_HueWheelSector[5] - 5*RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;

}

/**
 *
 * @param RGBWK&	rgbw
 * @param int& 		ww
 * @param int&		cw
 */
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
		cw = 0;
		ww = 0;
		break;
	}
 }

/**
 *
 * @param HSVK 	hsv
 * @param RGBWK	rgbw
 */
void RGBWWColorUtils::HSVtoRGB(const HSVK& hsv, RGBWK& rgbw) {
	HSVtoRGB(hsv, rgbw, _hsvmode);
}

/**
 *
 * @param HSVK 	hsv
 * @param RGBWK	rgbw
 * @param m
 */
void RGBWWColorUtils::HSVtoRGB(const HSVK& hsv, RGBWK& rgbw, RGBWW_HSVMODE m) {
	switch(m) {
	case SPEKTRUM: HSVtoRGBspektrum(hsv, rgbw); break;
	case RAINBOW: HSVtoRGBrainbow(hsv, rgbw); break;
	default: HSVtoRGBn(hsv, rgbw); break;
	}

}

/**
 * Convert HSV values to RGB colorspace by keeping
 * the max total color output equal.
 * Information see:
 * https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors
 *
 * @param HSVK 	hsv
 * @param RGBWK	rgbw
 */
void RGBWWColorUtils::HSVtoRGBspektrum(const HSVK& hsv, RGBWK& rgbw) {
	//TODO: implement linear spectrum
	HSVtoRGBn(hsv, rgbw);
}

/**


 */

/**
 * Convert HSV values to RGB colorspace with rainbow color table
 * Information see:
 * https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors
 *
 * @param HSVK 	hsv
 * @param RGBWK	rgbw
 */
void RGBWWColorUtils::HSVtoRGBrainbow(const HSVK& hsv, RGBWK& rgbw) {
	//TODO: implement rainbow spectrum
	HSVtoRGBn(hsv, rgbw);
}


/**
 * Convert HSV values to RGB colorspace using the algorithm
 * from https://en.wikipedia.org/wiki/HSL_and_HSV#From_HSV
 *
 *
 * @param HSVK 	hsv
 * @param RGBWK	rgbw
 */
void RGBWWColorUtils::HSVtoRGBn(const HSVK& hsv, RGBWK& rgbw) {
	int val, hue, sat, r, g, b, fract, chroma, m;

	hue = hsv.h;
	//gamma correction
	val = RGBWW_dim_curve[hsv.v];
	sat = hsv.s;
	sat = RGBWW_PWMMAXVAL-RGBWW_dim_curve[RGBWW_PWMMAXVAL-sat];
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

/**
 *
 *
 * @param rgbw
 * @param hsv
 */
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


/**
 * Helper function to keep HUE within boundaries [0, HUELWHEELMAX]
 *
 * @param hue
 */
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


