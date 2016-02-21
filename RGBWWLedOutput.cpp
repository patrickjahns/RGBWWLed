/*
 * RGBWWLedOutput.cpp
 *
 *  Created on: 21.02.2016
 *      Author: PJ
 */
#include "RGBWWLed.h"
#include "RGBWWLedOutput.h"

#ifdef SMING_VERSION
/****************************************************************
 	SMING -> we can use hardware pwm from ESP
 ***************************************************************/

PWMOutput::PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq) {
	uint8_t pins[5] = { redPin, greenPin, bluePin, wwPin, cwPin }; // List of pins that you want to connect to pwm
	_hwpwm = new HardwarePWM(pins, 5);
	_redPIN = redPin;
	_redVal = 0;
	_greenPIN = greenPin;
	_greenVal = 0;
	_bluePIN = bluePin;
	_blueVal = 0;
	_wwPIN = wwPin;
	_wwVal = 0;
	_cwPIN = cwPin;
	_cwVal = 0;

	int period = int(float(1000)/(float(freq)/float(1000)));
	debugf("period %i", period);
	_hwpwm->setPeriod(period);
	_maxduty = _hwpwm->getMaxDuty();
	_hwpwm->analogWrite(_redPIN, 0);
	_hwpwm->analogWrite(_greenPIN, 0);
	_hwpwm->analogWrite(_bluePIN, 0);
	_hwpwm->analogWrite(_wwPIN, 0);
	_hwpwm->analogWrite(_cwPIN, 0);
}

void PWMOutput::setRed(int value){
	if (value != _redVal) {
		_redVal = value;
		_hwpwm->analogWrite(_redPIN, parseDuty(value));
	}
}

int	PWMOutput::getRed(){
	return _redVal;
}

void PWMOutput::setGreen(int value){
	//if (value != _greenVal) {
		_greenVal = value;
		_hwpwm->analogWrite(_greenPIN, parseDuty(value));
	//}
}

int	PWMOutput::getGreen() {
	return _greenVal;
}

void PWMOutput::setBlue(int value){
	//if (value != _blueVal) {
		_blueVal = value;
		_hwpwm->analogWrite(_bluePIN, parseDuty(value));
	//}
}

int PWMOutput::getBlue(){
	return _blueVal;
}

void PWMOutput::setWarmWhite(int value){
	//if (value != _wwVal) {
		_wwVal = value;
		_hwpwm->analogWrite(_wwPIN, parseDuty(value));
	//}
}

int	PWMOutput::getWarmWhite() {
	return _wwVal;
}

void PWMOutput::setColdWhite(int value) {
	//if (value != _cwVal) {
		_cwVal = value;
		_hwpwm->analogWrite(_cwPIN, parseDuty(value));
	//}
}
int	PWMOutput::getColdWhite(){
	return _cwVal;
}

void PWMOutput::setOutput(int red, int green, int blue, int warmwhite, int coldwhite){
	setRed(red);
	setGreen(green);
	setBlue(blue);
	setWarmWhite(warmwhite);
	setColdWhite(coldwhite);
}


int PWMOutput::parseDuty(int duty) {
	return (duty*_maxduty)/PWMMAXVAL ;
}


#else

/******************************************************
 * 	NOT SMING => we use Arduino ESP framework pwm
 ******************************************************/
PWMOutput::PWMOutput(uint8_t redPin, uint8_t greenPin, uint8_t bluePin, uint8_t wwPin, uint8_t cwPin, uint16_t freq) {

	_redPIN = redPin;
	_redVal = 0;
	_greenPIN = greenPin;
	_greenVal = 0;
	_bluePIN = bluePin;
	_blueVal = 0;
	_wwPIN = wwPin;
	_wwVal = 0;
	_cwPIN = cwPin;
	_cwVal = 0;
	pinMode(_redPIN, OUTPUT);
	pinMode(_greenPIN, OUTPUT);
	pinMode(_bluePIN, OUTPUT);
	pinMode(_wwPIN, OUTPUT);
	pinMode(_cwPIN, OUTPUT);
	analogWriteFreq(pwmFrequency);
}

void PWMOutput::setRed(int value){
	if (value != _redVal) {
		_redVal = value;
		analogWrite(_redPIN, value);
	}
}

int	PWMOutput::getRed(){
	return _redVal;
}

void PWMOutput::setGreen(int value){
	if (value != _greenVal) {
		_greenVal = value;
		analogWrite(_greenPIN, value);
	}
}

int	PWMOutput::getGreen() {
	return _greenVal;
}

void PWMOutput::setBlue(int value){
	if (value != _blueVal) {
		_blueVal = value;
		analogWrite(_bluePIN, value);
	}
}

int PWMOutput::getBlue(){
	return _blueVal;
}

void PWMOutput::setWarmWhite(int value){
	if (value != _wwVal) {
		_wwVal = value;
		analogWrite(_wwPIN, value);
	}
}

int	PWMOutput::getWarmWhite() {
	return _wwVal;
}

void PWMOutput::setColdWhite(int value) {
	if (value != _cwVal) {
		_cwVal = value;
		analogWrite(_cwPIN, value);
	}
}
int	PWMOutput::getColdWhite(){
	return _cwVal;
}

void PWMOutput::setOutput(int red, int green, int blue, int warmwhite, int coldwhite){
	setRed(red);
	setGreen(green);
	setBlue(blue);
	setWarmWhite(warmwhite);
	setColdWhite(coldwhite);
}



#endif //SMING_VERSION
