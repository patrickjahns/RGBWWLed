/**
 * RGBWWLed - simple Library for controlling RGB WarmWhite ColdWhite LEDs via PWM
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * All files of this project are provided under the LGPL v3 license.
 */
#include "RGBWWLed.h"
#include "RGBWWLedOutput.h"

#ifdef RGBWW_USE_ESP_HWPWM
/*
 *  Use PWM Code from espressif sdk
 *  Provides a more stable pwm implementation compared to arduino esp
 *  framework
 */

PWMOutput::PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq /* = 200 */) {

	uint8_t pins[5] = { redPin, greenPin, bluePin, wwPin, cwPin};
	uint32 io_info[5][3];
	uint32 pwm_duty_init[5];
	for (uint8 i = 0; i < 5; i++) {
		io_info[i][0] = EspDigitalPins[pins[i]].mux;
		io_info[i][1] = EspDigitalPins[pins[i]].gpioFunc;
		io_info[i][2] = EspDigitalPins[pins[i]].id;
		pwm_duty_init[i] = 0;
		_duty[i] = 0;
	}
	_freq = freq;
	int period = int(float(1000)/(float(freq)/float(1000)));
	pwm_init(period, pwm_duty_init, 5, io_info);
	_maxduty = (period * 1000) / 45;
	pwm_set_period(period);
	pwm_start();
}

void PWMOutput::setFrequency(int freq){
	_freq = freq;
	int period = int(float(1000)/(float(freq)/float(1000)));
	_maxduty = (period * 1000) / 45;
	pwm_set_period(period);
	pwm_start();
}

int	PWMOutput::getFrequency() {
	return _freq;
}

void PWMOutput::setRed(int value, bool update /* = true */) {

	int duty = parseDuty(value);
	if (duty != getRed()) {
        pwm_set_duty(duty, RGBWW_CHANNELS::RED);
        _duty[RGBWW_CHANNELS::RED] = pwm_get_duty(RGBWW_CHANNELS::RED);

		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getRed(){
	return pwm_get_duty(RGBWW_CHANNELS::RED);
}

void PWMOutput::setGreen(int value, bool update /* = true */) {
	int duty = parseDuty(value);
    if (duty != getGreen()) {
        pwm_set_duty(duty, RGBWW_CHANNELS::GREEN);
        _duty[RGBWW_CHANNELS::GREEN] = pwm_get_duty(RGBWW_CHANNELS::GREEN);
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getGreen() {
	return pwm_get_duty(RGBWW_CHANNELS::GREEN);
}

void PWMOutput::setBlue(int value, bool update /* = true */) {
	int duty = parseDuty(value);
    if (duty != getBlue()) {
        pwm_set_duty(duty, RGBWW_CHANNELS::BLUE);
        _duty[RGBWW_CHANNELS::BLUE] = pwm_get_duty(RGBWW_CHANNELS::BLUE);
		if(update) {
			pwm_start();
		}
	}
}

int PWMOutput::getBlue(){
	return pwm_get_duty(RGBWW_CHANNELS::BLUE);
}

void PWMOutput::setWarmWhite(int value, bool update /* = true */) {
	debugRGBW("duty %i", value);
	int duty = parseDuty(value);
	debugRGBW("duty %i", duty);
	if (duty != getWarmWhite()) {
        pwm_set_duty(duty, RGBWW_CHANNELS::WW);
        _duty[RGBWW_CHANNELS::WW] = pwm_get_duty(RGBWW_CHANNELS::WW);
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getWarmWhite() {
	return pwm_get_duty(RGBWW_CHANNELS::WW);
}

void PWMOutput::setColdWhite(int value, bool update /* = true */) {
	int duty = parseDuty(value);
    if (duty != getColdWhite()) {
        pwm_set_duty(duty, RGBWW_CHANNELS::CW);
        _duty[RGBWW_CHANNELS::CW] = pwm_get_duty(RGBWW_CHANNELS::CW);
		if(update) {
			pwm_start();
		}
	}
}
int	PWMOutput::getColdWhite(){
	return pwm_get_duty(RGBWW_CHANNELS::CW);
}

void PWMOutput::setOutput(int red, int green, int blue, int warmwhite, int coldwhite){

	setRed(red, false);
	setGreen(green, false);
	setBlue(blue, false);
	setWarmWhite(warmwhite, false);
	setColdWhite(coldwhite, false);
	//only call pwm start at the end of all changes
	//might cause delay/missed changes otherwise
	pwm_start();

}


int PWMOutput::parseDuty(int duty) {
	return (duty * _maxduty) / RGBWW_PWMWIDTH;
}


#else

/*
 * If not using pwm implementation from espressif esp sdk
 * we fallback to the standard arduino pwm implementation
 *
 */
PWMOutput::PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq /* = 200 */) {

	_pins[RGBWW_CHANNELS::RED] = redPin;
	_pins[RGBWW_CHANNELS::GREEN] = greenPin;
	_pins[RGBWW_CHANNELS::BLUE] = bluePin;
	_pins[RGBWW_CHANNELS::WW] = wwPin;
	_pins[RGBWW_CHANNELS::CW] = cwPin;
	pinMode(redPin, OUTPUT);
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
	pinMode(wwPin, OUTPUT);
	pinMode(cwPin, OUTPUT);
	setFrequency(freq);
	_maxduty = RGBWW_ARDUINO_MAXDUTY;

}

void PWMOutput::setFrequency(int freq){
	_freq = freq;
	analogWriteFreq(freq);
}

int	PWMOutput::getFrequency() {
	return _freq;
}

void PWMOutput::setRed(int value, bool update /* = true */) {
	_duty[RGBWW_CHANNELS::RED] = parseDuty(value);
	analogWrite(_pins[RGBWW_CHANNELS::RED], value);
}

int PWMOutput::getRed() {
	return _duty[RGBWW_CHANNELS::RED];
}


void PWMOutput::setGreen(int value, bool update /* = true */) {
	_duty[RGBWW_CHANNELS::GREEN] = parseDuty(value);
	analogWrite(_pins[RGBWW_CHANNELS::GREEN], value);
}

int PWMOutput::getGreen() {
	return _duty[RGBWW_CHANNELS::GREEN];
}

void PWMOutput::setBlue(int value, bool update /* = true */) {
	_duty[RGBWW_CHANNELS::BLUE] = parseDuty(value);
	analogWrite(_pins[RGBWW_CHANNELS::BLUE], value);
}

int	PWMOutput::getBlue() {
	return _duty[RGBWW_CHANNELS::BLUE];
}


void PWMOutput::setWarmWhite(int value, bool update /* = true */) {
	_duty[RGBWW_CHANNELS::WW] = parseDuty(value);
	analogWrite(_pins[RGBWW_CHANNELS::WW], value);
}

int PWMOutput::getWarmWhite() {
	return _duty[RGBWW_CHANNELS::WW];
}

void PWMOutput::setColdWhite(int value, bool update /* = true */) {
	_duty[RGBWW_CHANNELS::CW] = parseDuty(value);
	analogWrite(_pins[RGBWW_CHANNELS::CW], value);
}

int	PWMOutput::getColdWhite() {
	return _duty[RGBWW_CHANNELS::CW];
}

void PWMOutput::setOutput(int red, int green, int blue, int warmwhite, int coldwhite){
	setRed(red);
	setGreen(green);
	setBlue(blue);
	setWarmWhite(warmwhite);
	setColdWhite(coldwhite);
}

int PWMOutput::parseDuty(int duty) {
	return (duty * _maxduty) / RGBWW_PWMWIDTH;
}
#endif //RGBWW_USE_ESP_HWPWM
