/*
 * RGBWWLedOutput.cpp
 *
 *  Created on: 21.02.2016
 *      Author: PJ
 */
#include "RGBWWLed.h"
#include "RGBWWLedOutput.h"

#ifdef USE_ESP_HWPWM
/****************************************************************
 	SMING -> we can use hardware pwm from ESP
 ***************************************************************/

PWMOutput::PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq) {
	
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
	//debugRGBW("RED - new duty %i, old duty %i channel %i", duty, getRed(), COLORS::RED);
	if (duty != getRed()) {
        pwm_set_duty(duty, COLORS::RED);
        _duty[COLORS::RED] = pwm_get_duty(COLORS::RED);
		
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getRed(){
	return pwm_get_duty(COLORS::RED);
}

void PWMOutput::setGreen(int value, bool update /* = true */) {
	int duty = parseDuty(value);
	//debugRGBW("GREEN - new duty %i, old duty %i channel %i", duty, getBlue(), COLORS::BLUE);
    if (duty != getGreen()) {
        pwm_set_duty(duty, COLORS::GREEN);
        _duty[COLORS::GREEN] = pwm_get_duty(COLORS::GREEN);
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getGreen() {
	return pwm_get_duty(COLORS::GREEN);
}

void PWMOutput::setBlue(int value, bool update /* = true */) {
	int duty = parseDuty(value);
	//debugRGBW("BLUE - new duty %i, old duty %i channel %i", duty, getGreen(), COLORS::GREEN);
    if (duty != getBlue()) {
        pwm_set_duty(duty, COLORS::BLUE);
        _duty[COLORS::BLUE] = pwm_get_duty(COLORS::BLUE);
		if(update) {
			pwm_start();
		}
	}
}

int PWMOutput::getBlue(){
	return pwm_get_duty(COLORS::BLUE);
}

void PWMOutput::setWarmWhite(int value, bool update /* = true */) {
	int duty = parseDuty(value);
    //debugRGBW("WW - new duty %i, old duty %i channel %i", duty, getWarmWhite(), COLORS::WW);
	if (duty != getWarmWhite()) {
        pwm_set_duty(duty, COLORS::WW);
        _duty[COLORS::WW] = pwm_get_duty(COLORS::WW);
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getWarmWhite() {
	return pwm_get_duty(COLORS::WW);
}

void PWMOutput::setColdWhite(int value, bool update /* = true */) {
	int duty = parseDuty(value);
	//debugRGBW("CW - new duty %i, old duty %i channel %i", duty, getColdWhite(), COLORS::CW);
    if (duty != getColdWhite()) {
        pwm_set_duty(duty, COLORS::CW);
        _duty[COLORS::CW] = pwm_get_duty(COLORS::CW);
		if(update) {
			pwm_start();
		}
	}
}
int	PWMOutput::getColdWhite(){
	return pwm_get_duty(COLORS::CW);
}

void PWMOutput::setOutput(int red, int green, int blue, int warmwhite, int coldwhite){
	setRed(red, false);
	setGreen(green, false);
	setBlue(blue, false);
	setWarmWhite(warmwhite, false);
	setColdWhite(coldwhite, false);
	pwm_start();
}


int PWMOutput::parseDuty(int duty) {
	return (duty*_maxduty)/PWMMAXVAL;
}


#else

/******************************************************
 * 	NOT ESP HW_PWM => we use Arduino ESP framework pwm
 ******************************************************/
PWMOutput::PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq) {

	_pins[COLORS::RED] = redPin;
	_pins[COLORS::GREEN] = greenPin;
	_pins[COLORS::BLUE] = bluePin;
	_pins[COLORS::WW] = wwPin;
	_pins[COLORS::CW] = cwPin;
	pinMode(redPin, OUTPUT);
	pinMode(greenPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
	pinMode(wwPin, OUTPUT);
	pinMode(cwPin, OUTPUT);
	setFrequency(freq);

}

void PWMOutput::setFrequency(int freq){
	_freq = freq;
	analogWriteFreq(freq);
}

int	PWMOutput::getFrequency() {
	return _freq;
}

void PWMOutput::setRed(int value, bool update /* = true */) {
	_duty[COLORS::RED] = value;
	analogWrite(_pins[COLORS::RED], value);
}

int PWMOutput::getRed() {
	return _duty[COLORS::RED];
}


void PWMOutput::setGreen(int value, bool update /* = true */) {
	_duty[COLORS::GREEN] = value;
	analogWrite(_pins[COLORS::GREEN], value);
}

int PWMOutput::getGreen() {
	return _duty[COLORS::GREEN];
}

void PWMOutput::setBlue(int value, bool update /* = true */) {
	_duty[COLORS::BLUE] = value;
	analogWrite(_pins[COLORS::BLUE], value);
}

int	PWMOutput::getBlue() {
	return _duty[COLORS::BLUE];
}


void PWMOutput::setWarmWhite(int value, bool update /* = true */) {
	_duty[COLORS::WW] = value;
	analogWrite(_pins[COLORS::WW], value);
}

int PWMOutput::getWarmWhite() {
	return _duty[COLORS::WW];
}

void PWMOutput::setColdWhite(int value, bool update /* = true */) {
	_duty[COLORS::CW] = value;
	analogWrite(_pins[COLORS::CW], value);
}

int	PWMOutput::getColdWhite() {
	return _duty[COLORS::CW];
}

void PWMOutput::setOutput(int red, int green, int blue, int warmwhite, int coldwhite){
	setRed(red);
	setGreen(green);
	setBlue(blue);
	setWarmWhite(warmwhite);
	setColdWhite(coldwhite);
}



#endif //USE_ESP_HWPWM
