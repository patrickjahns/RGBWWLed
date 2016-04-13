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
	_hsvmodel = RAW;
	_WarmWhiteKelvin = RGBWW_WARMWHITEKELVIN;
	_ColdWhiteKelvin = RGBWW_COLDWHITEKELVIN;
	createHueWheel();
	setBrightnessCorrection(100, 100, 100, 100, 100);

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
	_BrightnessFactor[RGBWW_CHANNELS::RED] = (constrain(r, 0, 100) * RGBWW_PWMMAXVAL) / 100;
	_BrightnessFactor[RGBWW_CHANNELS::GREEN] = (constrain(g, 0, 100) *  RGBWW_PWMMAXVAL) / 100;
	_BrightnessFactor[RGBWW_CHANNELS::BLUE] = (constrain(b, 0, 100) * RGBWW_PWMMAXVAL) / 100;
	_BrightnessFactor[RGBWW_CHANNELS::WW] = (constrain(ww, 0, 100) * RGBWW_PWMMAXVAL) / 100;
	_BrightnessFactor[RGBWW_CHANNELS::CW] = (constrain(cw, 0, 100) * RGBWW_PWMMAXVAL) / 100;

};


void RGBWWColorUtils::getBrightnessCorrection(int& r, int& g, int& b, int& ww, int& cw) {
	r = (_BrightnessFactor[RGBWW_CHANNELS::RED] * 100) / RGBWW_PWMMAXVAL;
	g = (_BrightnessFactor[RGBWW_CHANNELS::GREEN] * 100) / RGBWW_PWMMAXVAL;
	b = (_BrightnessFactor[RGBWW_CHANNELS::BLUE] * 100) / RGBWW_PWMMAXVAL;
	ww = (_BrightnessFactor[RGBWW_CHANNELS::WW] * 100) / RGBWW_PWMMAXVAL;
	cw = (_BrightnessFactor[RGBWW_CHANNELS::CW] * 100) / RGBWW_PWMMAXVAL;
}



void RGBWWColorUtils::correctBrightness(ChannelOutput& output) {
	output.red = (output.red * _BrightnessFactor[RGBWW_CHANNELS::RED]) / RGBWW_PWMMAXVAL;
	output.green = (output.green * _BrightnessFactor[RGBWW_CHANNELS::GREEN]) / RGBWW_PWMMAXVAL;
	output.blue = (output.blue * _BrightnessFactor[RGBWW_CHANNELS::BLUE]) / RGBWW_PWMMAXVAL;
	output.warmwhite = (output.warmwhite * _BrightnessFactor[RGBWW_CHANNELS::WW]) / RGBWW_PWMMAXVAL;
	output.coldwhite = (output.coldwhite * _BrightnessFactor[RGBWW_CHANNELS::CW]) / RGBWW_PWMMAXVAL;
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
	red = -1 * (float(_HueWheelSector[6] - 6* RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	yellow = -1 * (float(_HueWheelSector[1] - 1* RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	green = -1 * (float(_HueWheelSector[2] - 2* RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	cyan = -1 * (float(_HueWheelSector[3] - 3* RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	blue = -1 * (float(_HueWheelSector[4] - 4* RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;
	magenta = -1 * (float(_HueWheelSector[5] - 5* RGBWW_PWMMAXVAL)/ float(RGBWW_PWMMAXVAL)) * 60.0;

}


 void RGBWWColorUtils::whiteBalance(RGBWK& rgbw, ChannelOutput& output) {
	output.r = rgbw.r;
	output.g = rgbw.g;
	output.b = rgbw.b;
	switch(_colormode) {
	case RGBWWCW:
		if (_WarmWhiteKelvin <= rgbw.k && _ColdWhiteKelvin >= rgbw.k) {
			int wwfactor = ((_ColdWhiteKelvin - rgbw.k) *RGBWW_PWMMAXVAL) /  (_ColdWhiteKelvin - _WarmWhiteKelvin);
			//balance between CW and WW Leds
			output.warmwhite = (rgbw.w * wwfactor) /RGBWW_PWMMAXVAL;
			output.coldwhite = (rgbw.w * (1 - wwfactor)) /RGBWW_PWMMAXVAL;
		} else {
			// if kelvin outside range - different calculation algorithm
			// for now we asume a neutral white (i.e 0.5 CW, 0.5 WW)
			output.warmwhite = rgbw.w/2;
			output.coldwhite = rgbw.w/2;
		}
		break;
	case RGBCW:
		//TODO implement valid algorithm

		output.warmwhite = 0;
		output.coldwhite = rgbw.w;
		break;

	case RGBWW:
		//TODO implement valid algorithm
		output.warmwhite = rgbw.w;
		output.coldwhite = 0;
		break;

	case RGB:
		//TODO implement valid algorithm
		output.r += rgbw.w;
		output.g += rgbw.w;
		output.b += rgbw.w;
		output.coldwhite = 0;
		output.warmwhite = 0;
		break;
	}
 }


void RGBWWColorUtils::HSVtoRGB(const HSVK& hsvk, RGBWK& rgbwk) {
	HSVtoRGB(hsvk, rgbwk, _hsvmodel);
}


void RGBWWColorUtils::HSVtoRGB(const HSVK& hsvk, RGBWK& rgbwk, RGBWW_HSVMODEL mode) {

	switch(mode) {
		case SPEKTRUM: {
			HSVtoRGBspektrum(hsvk, rgbwk); break;
		}
		case RAINBOW: {
			// TODO: transform normal HUE range to rainbowHue range
			// map 0 - 60 to 0-90
			// map 60 - 120 to 90 - 135
			// map 120 - 180 to 135 - 180
			// map 180 - 240 to 180 - 225
			// map 240 - 300 to 225 - 270
			// map 300 - 360 to 270 - 360
			HSVtoRGBrainbow(hsvk, rgbwk); break;
		}
		default: {
			HSVtoRGBraw(hsvk, rgbwk); break;
		}
	}

}




// Method is based one the method from FASTLed
// https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors#color-map-rainbow-vs-spectrum
// https://github.com/FastLED/FastLED/blob/master/colorutils.h
//
#define rainbow_third int(RGBWW_PWMMAXVAL / 3)
#define rainbow_two_third int(rainbow_third *2)
#define rainbow_sector_width int (RGBWW_PWMHUEWHEELMAX / 8)

void RGBWWColorUtils::HSVtoRGBrainbow(const HSVK& hsvk, RGBWK& rgbwk) {
	int val, hue, sat, r, g, b, chroma, m, sector;


	hue = hsvk.h;

	//gamma correction
	val = RGBWW_dim_curve[hsvk.v];
	sat = hsvk.s;
	sat = RGBWW_PWMMAXVAL - RGBWW_dim_curve[RGBWW_PWMMAXVAL-sat];

	if(sat == 0) {
		// color is grayscale
		rgbwk.r = 0;
		rgbwk.g = 0;
		rgbwk.b = 0;
		rgbwk.w = val;
	} else {
		chroma = (sat * val) / RGBWW_PWMMAXVAL;
		m = val - chroma;

		sector = hue / rainbow_sector_width;
		hue = hue - sector * rainbow_sector_width;

		if (sector < 4) {
			//sector 0 - 3
			if(sector < 1) {
				// red - > orange
				debugRGBW("HSVtoRGBrainbow sector 0");
				r = (chroma * (RGBWW_PWMMAXVAL -  (rainbow_third * hue) / rainbow_sector_width  )) / RGBWW_PWMMAXVAL;
				g = (chroma * ((rainbow_third * hue) / rainbow_sector_width  )) / RGBWW_PWMMAXVAL;
				b = 0;
			} else if(sector < 2) {
				// orange -> yellow
				debugRGBW("HSVtoRGBrainbow sector 1");
				r = (chroma * (rainbow_two_third )) / RGBWW_PWMMAXVAL;
				g = (chroma * (rainbow_third + (rainbow_third * hue) / rainbow_sector_width )) / RGBWW_PWMMAXVAL;
				b = 0;
			} else if(sector < 3) {
				// yellow -> green
				debugRGBW("HSVtoRGBrainbow sector 2");
				r = (chroma * (rainbow_two_third - (rainbow_two_third * hue) / rainbow_sector_width )) / RGBWW_PWMMAXVAL;
				g = (chroma * (rainbow_two_third + (rainbow_third * hue) / rainbow_sector_width )) / RGBWW_PWMMAXVAL;
				b = 0;
			} else {
				// green ->  aqua
				debugRGBW("HSVtoRGBrainbow sector 3");
				r = 0;
				g = (chroma * (RGBWW_PWMMAXVAL -  (rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
				b = (chroma * ((rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
			}
		} else {
			//sector 4 - 7
			if(sector < 5) {
				// aqua -> blue
				debugRGBW("HSVtoRGBrainbow sector 4");
				r = 0;
				g = (chroma * (rainbow_two_third - (rainbow_two_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
				b = (chroma * (rainbow_third + (rainbow_two_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
			} else if(sector < 6) {
				// blue -> purple
				debugRGBW("HSVtoRGBrainbow sector 5");
				r = (chroma * ((rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
				g = 0;
				b = (chroma * (RGBWW_PWMMAXVAL -  (rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
			} else if(sector < 7) {
				// purple -> pink
				debugRGBW("HSVtoRGBrainbow sector 6");
				r = (chroma * (rainbow_third + (rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
				g = 0;
				b = (chroma * (rainbow_two_third - (rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
			} else {
				// pink -> red
				debugRGBW("HSVtoRGBrainbow sector 7");
				r = (chroma * (rainbow_two_third + (rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
				g = 0;
				b = (chroma * (rainbow_third - (rainbow_third * hue) / rainbow_sector_width)) / RGBWW_PWMMAXVAL;
			}
		}
		rgbwk.r = r;
		rgbwk.g = g;
		rgbwk.b = b;
		rgbwk.w = m;
	}
	debugRGBW("HSVtoRGBrainbow R %i | G %i | B %i | W %i", rgbwk.r, rgbwk.g, rgbwk.b, rgbwk.w);
}


void RGBWWColorUtils::HSVtoRGBspektrum(const HSVK& hsvk, RGBWK& rgbwk) {
	int val, hue, sat, r, g, b, fract, chroma, half_chroma, m;

	hue = hsvk.h;

	//gamma correction
	val = RGBWW_dim_curve[hsvk.v];
	sat = hsvk.s;
	sat = RGBWW_PWMMAXVAL-RGBWW_dim_curve[RGBWW_PWMMAXVAL-sat];
	//sat = hsv.s;

	if(sat == 0) {
		// color is grayscale
		rgbwk.r = 0;
		rgbwk.g = 0;
		rgbwk.b = 0;
		rgbwk.w = val;
	} else {
		chroma = (sat * val) / RGBWW_PWMMAXVAL;
		half_chroma = chroma >> 1;
		m = val - chroma;
		if ( hue < _HueWheelSector[0] || (hue > _HueWheelSector[5] && hue <= _HueWheelSector[6])) {
			debugRGBW("HSVtoRGBspektrum Sector 6");
			if (hue < _HueWheelSector[0]) {
				fract = RGBWW_PWMMAXVAL + hue ;
			} else {
				fract = hue - _HueWheelSector[5];
			}
			fract = ( half_chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[5])) / RGBWW_PWMMAXVAL;
			r = half_chroma + fract;
			g = 0;
			b = half_chroma - fract;

		} else if (  hue <= _HueWheelSector[1]  || hue > _HueWheelSector[6]) {
			// Sector 1
			debugRGBW("HSVtoRGBspektrum Sector 1");
			if (hue > _HueWheelSector[6]) {
				fract = hue - _HueWheelSector[6];
			} else {
				fract = hue + (RGBWW_PWMHUEWHEELMAX - _HueWheelSector[6]);
			}
			fract = (half_chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[0])) / RGBWW_PWMMAXVAL;
			r = chroma - fract;
			g = fract;
			b = 0;

		} else if (hue <= _HueWheelSector[2]) {
			// Sector 2
			debugRGBW("HSVtoRGBspektrum Sector 2");
			fract = hue - _HueWheelSector[1];
			fract = (half_chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[1])) / RGBWW_PWMMAXVAL ;
			r = half_chroma - fract;
			g = half_chroma + fract;
			b = 0;

		} else if (hue <= _HueWheelSector[3]) {
			// Sector 3
			debugRGBW("HSVtoRGBspektrum Sector 3");
			fract = hue - _HueWheelSector[2];
			fract = (half_chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[2])) / RGBWW_PWMMAXVAL;
			r = 0;
			g = chroma - fract;
			b = fract;

		} else if (hue <= _HueWheelSector[4]) {
			// Sector 4
			debugRGBW("HSVtoRGBspektrum Sector 4");
			fract = hue - _HueWheelSector[3];
			fract = (half_chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[3])) / RGBWW_PWMMAXVAL;
			r = 0;
			g = half_chroma - fract;
			b = half_chroma + fract;

		} else  {
			// Sector 5
			debugRGBW("HSVtoRGBspektrum Sector 5");
			fract = hue - _HueWheelSector[4];
			fract = (half_chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[4])) / RGBWW_PWMMAXVAL;
			r = fract;
			g = 0;
			b = chroma - fract;

		}
		rgbwk.r = r;
		rgbwk.g = g;
		rgbwk.b = b;
		rgbwk.w = m;
	}
	debugRGBW("HSVtoRGBspektrum R %i | G %i | B %i | W %i", rgbwk.r, rgbwk.g, rgbwk.b, rgbwk.w);

}



void RGBWWColorUtils::HSVtoRGBraw(const HSVK& hsvk, RGBWK& rgbwk) {
	int val, hue, sat, r, g, b, fract, chroma, m;

	hue = hsvk.h;

	//gamma correction
	val = RGBWW_dim_curve[hsvk.v];
	sat = hsvk.s;
	sat = RGBWW_PWMMAXVAL-RGBWW_dim_curve[RGBWW_PWMMAXVAL-sat];

	rgbwk.k = hsvk.k;


	if(sat == 0) {
		/* color is grayscale */
		rgbwk.r = 0;
		rgbwk.g = 0;
		rgbwk.b = 0;
		rgbwk.w = val;


	} else {
		/*
         * We have 6 sectors
         * We need 7 "borders" to incorporate the shift oft sectors
         *
         * Example: 8bit values
         * Sector 1 is from 0 - 255. We need border 0 as lower border, 255 as upper border.
         * Sector 6 is from 1275 - 1530. Lower border 1275, upper border 1530
		 *
         * Apply a color correct for red with +10deg (value 25) results in
         * Sector 1 from 0 - 255 & 1505 - 1530
         * Sector 6 from 1275 - 1505

         * Apply a color correct for red with -10deg (value 25) results in
         * Sector 1 from 25 - 255
         * Sector 6 from 1275 - 1530 && 0 - 25
		 */
		chroma = (sat * val) / RGBWW_PWMMAXVAL;
		m = val - chroma;
		if ( hue < _HueWheelSector[0] || (hue > _HueWheelSector[5] && hue <= _HueWheelSector[6])) {
			debugRGBW("HSVtoRGBraw Sector 6");
			if (hue < _HueWheelSector[0]) {
				fract = RGBWW_PWMMAXVAL + hue ;
			} else {
				fract = hue - _HueWheelSector[5];
			}

			r = chroma;
			g = 0;
			b = ( chroma * (RGBWW_PWMMAXVAL - (RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[5])) / RGBWW_PWMMAXVAL;

		} else if (  hue <= _HueWheelSector[1]  || hue > _HueWheelSector[6]) {
			// Sector 1
			debugRGBW("HSVtoRGBraw Sector 1");
			if (hue > _HueWheelSector[6]) {
				fract = hue - _HueWheelSector[6];
			} else {
				fract = hue + (RGBWW_PWMHUEWHEELMAX - _HueWheelSector[6]);
			}
			r = chroma;
			g = (chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[0])) / RGBWW_PWMMAXVAL;
			b = 0;

		} else if (hue <= _HueWheelSector[2]) {
			// Sector 2
			debugRGBW("HSVtoRGBraw Sector 2");
			fract = hue - _HueWheelSector[1];
			r = (chroma * (RGBWW_PWMMAXVAL - (RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[1])) / RGBWW_PWMMAXVAL;
			g = chroma;
			b = 0;

		} else if (hue <= _HueWheelSector[3]) {
			// Sector 3
			debugRGBW("HSVtoRGBraw Sector 3");
			fract = hue - _HueWheelSector[2];
			r = 0;
			g = chroma;
			b = (chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[2])) / RGBWW_PWMMAXVAL;

		} else if (hue <= _HueWheelSector[4]) {
			// Sector 4
			debugRGBW("HSVtoRGBraw Sector 4");
			fract = hue - _HueWheelSector[3];
			r = 0;
			g =(chroma * (RGBWW_PWMMAXVAL - (RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[3])) / RGBWW_PWMMAXVAL;
			b = chroma;

		} else  {
			// Sector 5
			debugRGBW("HSVtoRGBraw Sector 5");
			fract = hue - _HueWheelSector[4];
			r = (chroma * ((RGBWW_PWMMAXVAL * fract) / _HueWheelSectorWidth[4])) / RGBWW_PWMMAXVAL;
			g = 0;
			b = chroma;

		}
		// m equals the white part
		// for rgbw we use it for the white channels
		rgbwk.r = r;
		rgbwk.g = g;
		rgbwk.b = b;
		rgbwk.w = m;

	}
	debugRGBW("HSVtoRGBraw R %i | G %i | B %i | W %i", rgbwk.r, rgbwk.g, rgbwk.b, rgbwk.w);
}


void  RGBWWColorUtils::RGBtoHSV(const RGBWK& rgbw, HSVK& hsv) {
	debugRGBW("RGBWWColorUtils::RGBtoHSV");
	//TODO: needs implementation

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
