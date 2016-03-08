/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 * @version 0.6
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
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

#ifndef RGBWWLed_h
#define RGBWWLed_h


#define RGBWW_VERSION "0.6"
#define RGBWW_PWMDEPTH 10
#define RGBWW_PWMWIDTH int(pow(2, RGBWW_PWMDEPTH))
#define	RGBWW_PWMMAXVAL int(RGBWW_PWMWIDTH - 1)
#define	RGBWW_PWMHUEWHEELMAX int(RGBWW_PWMMAXVAL * 6)

#define RGBWW_UPDATEFREQUENCY 50
#define RGBWW_MINTIMEDIFF  int(1000 / RGBWW_UPDATEFREQUENCY)
#define RGBWW_ANIMATIONQSIZE 50
#define	RGBWW_WARMWHITEKELVIN 2700
#define RGBWW_COLDWHITEKELVIN 6000

#include <ARDUINO.h>
#ifdef SMING_VERSION
	#include "../../SmingCore/SmingCore.h"
#endif
#define RGBWW_USE_ESP_HWPWM
#define DEBUG_RGBWW
#include "debugUtils.h"
#include "RGBWWLedColor.h"
#include "RGBWWLedAnimation.h"
#include "RGBWWLedOutput.h"


enum RGBWW_COLORMODE {
	RGB = 0,
	RGBWW = 1,
	RGBCW = 2,
	RGBWWCW = 3
};

enum RGBWW_HSVMODE {
	NORMAL = 0,
	SPEKTRUM = 1,
	RAINBOW = 2
};

class RGBWWLedAnimation;
class RGBWWLedAnimationQ;
class PWMOutput;

class RGBWWLed
{
public:
	//init & settings
	RGBWWLed();
	virtual 		~RGBWWLed();
	void			setColorMode(RGBWW_COLORMODE mode);
	RGBWW_COLORMODE getColorMode();
	void    		setHSVmode(RGBWW_HSVMODE mode);
	RGBWW_HSVMODE	getHSVmode();
	void    		init(int redPIN, int greenPIN, int bluePIN, int wwPIN, int cwPIN, int pwmFrequency=200);
	void    		setHSVcorrection(float red, float yellow, float green, float cyan, float blue, float magenta);
	void    		getHSVcorrection(float& red, float& yellow, float& green, float& cyan, float& blue, float& magenta);
	void    		setBrightnessCorrection(int r, int g, int b, int ww, int cw);
	void    		getBrightnessCorrection(int& r, int& g, int& b, int& ww, int& cw);


	//output related
	bool    	show();
	void		refresh();
	void    	setOutput(HSVK color);
	void    	setOutput(RGBWK color);
	void    	setOutputRaw(int red, int green, int blue, int cwhite, int wwhite);
	HSVK    	getCurrentColor();

	//animation related
	void    	setHSV(HSVK& color);
	void    	setHSV(HSVK& color, int time, int direction);
	void    	setHSV(HSVK& color, int time, bool q);
	void    	setHSV(HSVK& color, int time, int direction, bool q);
	void    	setHSV(HSVK& colorFrom, HSVK& color, int time, int direction=1, bool q=false);
	void		setAnimationCallback( void (*func)(RGBWWLed* led) );
	bool		isAnimationActive();
	bool    	isAnimationQFull();
	void    	skipAnimation();
	void    	clearAnimationQueue();
	void		setAnimationSpeed(int speed);
	void		setAnimationBrightness(int brightness);


	//colorutils
	void		whiteBalance(RGBWK& rgbw, int& ww, int& cw);
	void    	HSVtoRGB(const HSVK& hsv, RGBWK& rgbw);
	void    	HSVtoRGB(const HSVK& hsv, RGBWK& rgbw, RGBWW_HSVMODE mode);
	void    	HSVtoRGBn(const HSVK& hsv, RGBWK& rgbw);
	void    	HSVtoRGBspektrum(const HSVK& hsv, RGBWK& rgbw);
	void    	HSVtoRGBrainbow(const HSVK& hsv, RGBWK& rgbw);
	void    	RGBtoHSV(const RGBWK& rgbw, HSVK& hsv);


	//helpers
	int     	parseHue(float hue);
	int     	parseSat(float sat);
	int     	parseVal(float val);
	void    	circleHue(int& hue);
	int     	parseColorCorrection(float val);

private:
	int         _BrightnessFactor[5];
	int         _HueWheelSector[7];
	int         _HueWheelSectorWidth[6];
	int			_WarmWhiteKelvin;
	int			_ColdWhiteKelvin;
	
	RGBWW_COLORMODE       _colormode;
	RGBWW_HSVMODE         _hsvmode;
	HSVK            _current_color;
	RGBWK           _current_output;

	unsigned long   last_active;

	bool            _cancelAnimation;
	bool            _clearAnimationQueue;
	bool            _isAnimationActive;
	void (*_animationcallback)(RGBWWLed* led) = NULL;

	RGBWWLedAnimation*  	_currentAnimation;
	RGBWWLedAnimationQ*  	_animationQ;
	PWMOutput*				_pwm_output;

	//helpers

	void    createHueWheel();
	void    cleanupCurrentAnimation();
	void    clearAnimationQ();

};

#endif //RGBWWLed_h
