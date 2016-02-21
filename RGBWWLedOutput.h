/**************************************************************


    @author Patrick Jahns
 **************************************************************/
#ifndef RGBWWLedOutput_h
#define RGBWWLedOutput_h
#include "RGBWWLed.h"
#ifdef SMING_VERSION
#include <HardwarePWM.h>
#endif

class PWMOutput
{

public:
	PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq = 200);

	void 		setFrequency(int freq);
	int			getFrequency();
	void		setRed(int value);
	int			getRed();
	void 		setGreen(int value);
	int			getGreen();
	void		setBlue(int value);
	int 		getBlue();
	void 		setWarmWhite(int value);
	int			getWarmWhite();
	void		setColdWhite(int value);
	int			getColdWhite();
	void		setOutput(int red, int green, int blue, int warmwhite, int coldwhite);


private:
	int 		_redPIN;
	int			_redVal;
	int 		_greenPIN;
	int			_greenVal;
	int			_bluePIN;
	int			_blueVal;
	int			_wwPIN;
	int			_wwVal;
	int			_cwPIN;
	int			_cwVal;

	#ifdef SMING_VERSION
	int			_maxduty;
	int			parseDuty(int duty);
	HardwarePWM* _hwpwm;

	#endif //SMING_VERSION
};


#endif //RGBWWLedOutput_h
