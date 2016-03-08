/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
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
#include "RGBWWLed.h"
#include "RGBWWLedOutput.h"

#ifdef RGBWW_USE_ESP_HWPWM
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
        pwm_set_duty(duty, RGBWW_COLORS::RED);
        _duty[RGBWW_COLORS::RED] = pwm_get_duty(RGBWW_COLORS::RED);
		
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getRed(){
	return pwm_get_duty(RGBWW_COLORS::RED);
}

void PWMOutput::setGreen(int value, bool update /* = true */) {
	int duty = parseDuty(value);
	//debugRGBW("GREEN - new duty %i, old duty %i channel %i", duty, getBlue(), COLORS::BLUE);
    if (duty != getGreen()) {
        pwm_set_duty(duty, RGBWW_COLORS::GREEN);
        _duty[RGBWW_COLORS::GREEN] = pwm_get_duty(RGBWW_COLORS::GREEN);
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getGreen() {
	return pwm_get_duty(RGBWW_COLORS::GREEN);
}

void PWMOutput::setBlue(int value, bool update /* = true */) {
	int duty = parseDuty(value);
	//debugRGBW("BLUE - new duty %i, old duty %i channel %i", duty, getGreen(), COLORS::GREEN);
    if (duty != getBlue()) {
        pwm_set_duty(duty, RGBWW_COLORS::BLUE);
        _duty[RGBWW_COLORS::BLUE] = pwm_get_duty(RGBWW_COLORS::BLUE);
		if(update) {
			pwm_start();
		}
	}
}

int PWMOutput::getBlue(){
	return pwm_get_duty(RGBWW_COLORS::BLUE);
}

void PWMOutput::setWarmWhite(int value, bool update /* = true */) {
	int duty = parseDuty(value);
    //debugRGBW("WW - new duty %i, old duty %i channel %i", duty, getWarmWhite(), COLORS::WW);
	if (duty != getWarmWhite()) {
        pwm_set_duty(duty, RGBWW_COLORS::WW);
        _duty[RGBWW_COLORS::WW] = pwm_get_duty(RGBWW_COLORS::WW);
		if(update) {
			pwm_start();
		}
	}
}

int	PWMOutput::getWarmWhite() {
	return pwm_get_duty(RGBWW_COLORS::WW);
}

void PWMOutput::setColdWhite(int value, bool update /* = true */) {
	int duty = parseDuty(value);
	//debugRGBW("CW - new duty %i, old duty %i channel %i", duty, getColdWhite(), COLORS::CW);
    if (duty != getColdWhite()) {
        pwm_set_duty(duty, RGBWW_COLORS::CW);
        _duty[RGBWW_COLORS::CW] = pwm_get_duty(RGBWW_COLORS::CW);
		if(update) {
			pwm_start();
		}
	}
}
int	PWMOutput::getColdWhite(){
	return pwm_get_duty(RGBWW_COLORS::CW);
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
	return (duty*_maxduty)/RGBWW_PWMMAXVAL;
}


#else

/******************************************************
 * 	NOT ESP HW_PWM => we use Arduino ESP framework pwm
 ******************************************************/
PWMOutput::PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq) {

	_pins[RGBWW_COLORS::RED] = redPin;
	_pins[RGBWW_COLORS::GREEN] = greenPin;
	_pins[RGBWW_COLORS::BLUE] = bluePin;
	_pins[RGBWW_COLORS::WW] = wwPin;
	_pins[RGBWW_COLORS::CW] = cwPin;
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
	_duty[RGBWW_COLORS::RED] = value;
	analogWrite(_pins[RGBWW_COLORS::RED], value);
}

int PWMOutput::getRed() {
	return _duty[RGBWW_COLORS::RED];
}


void PWMOutput::setGreen(int value, bool update /* = true */) {
	_duty[RGBWW_COLORS::GREEN] = value;
	analogWrite(_pins[RGBWW_COLORS::GREEN], value);
}

int PWMOutput::getGreen() {
	return _duty[RGBWW_COLORS::GREEN];
}

void PWMOutput::setBlue(int value, bool update /* = true */) {
	_duty[RGBWW_COLORS::BLUE] = value;
	analogWrite(_pins[RGBWW_COLORS::BLUE], value);
}

int	PWMOutput::getBlue() {
	return _duty[RGBWW_COLORS::BLUE];
}


void PWMOutput::setWarmWhite(int value, bool update /* = true */) {
	_duty[RGBWW_COLORS::WW] = value;
	analogWrite(_pins[RGBWW_COLORS::WW], value);
}

int PWMOutput::getWarmWhite() {
	return _duty[RGBWW_COLORS::WW];
}

void PWMOutput::setColdWhite(int value, bool update /* = true */) {
	_duty[RGBWW_COLORS::CW] = value;
	analogWrite(_pins[RGBWW_COLORS::CW], value);
}

int	PWMOutput::getColdWhite() {
	return _duty[RGBWW_COLORS::CW];
}

void PWMOutput::setOutput(int red, int green, int blue, int warmwhite, int coldwhite){
	setRed(red);
	setGreen(green);
	setBlue(blue);
	setWarmWhite(warmwhite);
	setColdWhite(coldwhite);
}

/*
int PWMOutput::parseDuty(int duty) {
	return (duty*_maxduty)/RGBWW_PWMMAXVAL;
}
*/
#endif //USE_ESP_HWPWM
