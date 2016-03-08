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
#ifndef RGBWWLedOutput_h
#define RGBWWLedOutput_h
#include "RGBWWLed.h"


enum RGBWW_COLORS {
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	WW = 3,
	CW = 4,
	NUM_COLORS = 5
};

#ifdef RGBWW_USE_ESP_HWPWM

extern "C" {
#include <pwm.h>
}



class PWMOutput
{

public:
	PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq = 200);

	void 		setFrequency(int freq);
	int			getFrequency();
	void		setRed(int value, bool update = true);
	int			getRed();
	void 		setGreen(int value, bool update = true);
	int			getGreen();
	void		setBlue(int value, bool update = true);
	int 		getBlue();
	void 		setWarmWhite(int value, bool update = true);
	int			getWarmWhite();
	void		setColdWhite(int value, bool update = true);
	int			getColdWhite();
	void		setOutput(int red, int green, int blue, int warmwhite, int coldwhite);


private:
	int			parseDuty(int duty);
	int			_freq;
	int			_duty[RGBWW_COLORS::NUM_COLORS];
	int			_maxduty;

	
};

#else

class PWMOutput
	{

	public:
		PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq = 200);

		void 		setFrequency(int freq);
		int			getFrequency();
		void		setRed(int value, bool update = true);
		int			getRed();
		void 		setGreen(int value, bool update = true);
		int			getGreen();
		void		setBlue(int value, bool update = true);
		int 		getBlue();
		void 		setWarmWhite(int value, bool update = true);
		int			getWarmWhite();
		void		setColdWhite(int value, bool update = true);
		int			getColdWhite();
		void		setOutput(int red, int green, int blue, int warmwhite, int coldwhite);


	private:
		int			_freq;
		int			_pins[5];
		int			_duty[5];
		
};
#endif //USE_ESP_HWPWM


#endif //RGBWWLedOutput_h
