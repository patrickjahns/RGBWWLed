/**************************************************************


    @author Patrick Jahns
 **************************************************************/
#ifndef RGBWWLedOutput_h
#define RGBWWLedOutput_h
#include "RGBWWLed.h"


enum COLORS {
	RED = 0,
	GREEN = 1,
	BLUE = 2,
	WW = 3,
	CW = 4,
	NUM_COLORS = 5
};

#ifdef USE_ESP_HWPWM

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
	int			_duty[COLORS::NUM_COLORS];
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
