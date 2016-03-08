/**
 * RGBWWLed - simple Library for controlling RGB WarmWhite ColdWhite LEDs via PWM
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * All files of this project are provided under the LGPL v3 license.
 */
#ifndef RGBWWLedOutput_h
#define RGBWWLedOutput_h
#include "RGBWWLed.h"


#ifdef RGBWW_USE_ESP_HWPWM

/*
 *  Use PWM Code from espressif sdk
 *  Provides a more stable pwm implementation compared to arduino esp
 *  framework
 */

extern "C" {
#include <pwm.h>
}



class PWMOutput
{

public:
	PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq = 200);

	void 	setFrequency(int freq);
	int		getFrequency();
	void	setRed(int value, bool update = true);
	int		getRed();
	void 	setGreen(int value, bool update = true);
	int		getGreen();
	void	setBlue(int value, bool update = true);
	int 	getBlue();
	void 	setWarmWhite(int value, bool update = true);
	int		getWarmWhite();
	void	setColdWhite(int value, bool update = true);
	int		getColdWhite();
	void	setOutput(int red, int green, int blue, int warmwhite, int coldwhite);


private:
	int		parseDuty(int duty);
	int		_freq;
	int		_duty[RGBWW_COLORS::NUM_COLORS];
	int		_maxduty;

	
};

#else

/*
 * If not using pwm implementation from espressif esp sdk
 * we fallback to the standard arduino pwm implementation
 *
 */

class PWMOutput
	{

	public:
		PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq = 200);

		void 	setFrequency(int freq);
		int		getFrequency();
		void	setRed(int value, bool update = true);
		int		getRed();
		void 	setGreen(int value, bool update = true);
		int		getGreen();
		void	setBlue(int value, bool update = true);
		int 	getBlue();
		void 	setWarmWhite(int value, bool update = true);
		int		getWarmWhite();
		void	setColdWhite(int value, bool update = true);
		int		getColdWhite();
		void	setOutput(int red, int green, int blue, int warmwhite, int coldwhite);


	private:
		int		_freq;
		int		_pins[5];
		int		_duty[5];
		
};
#endif //RGBWW_USE_ESP_HWPWM

#endif //RGBWWLedOutput_h
