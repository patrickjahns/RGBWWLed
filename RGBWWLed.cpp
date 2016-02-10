/**************************************************************


    @author Patrick Jahns
    @version 0.1
 **************************************************************/
#include "RGBWWLed.h"
#include "debugUtils.h"
#ifndef ESP8266
    #include "compat.h"
#endif // ESP8266

RGBWWLed::RGBWWLed() {
	_pwmDepth = PWMWIDTH;
	_pwmWidth = pow(2, _pwmDepth);
	_pwmMaxVal = _pwmWidth -1;
	_pwmHueWheelMax = _pwmMaxVal * (hueV/60);
	_colormode = RGBW;
    createHueWheel();

}



void RGBWWLed::init(uint8_t redPIN, uint8_t greenPIN, uint8_t bluePIN, uint8_t wwPIN, uint8_t cwPIN) {
    _redPIN = redPIN;
    _greenPIN = greenPIN;
    _bluePIN = bluePIN;
    _wwPIN = wwPIN;
    _cwPIN = cwPIN;

    pinMode(_redPIN, OUTPUT);
    pinMode(_greenPIN, OUTPUT);
    pinMode(_bluePIN, OUTPUT);
    pinMode(_wwPIN, OUTPUT);
    pinMode(_cwPIN, OUTPUT);


}



void RGBWWLed::setMode(int mode) {
    if (mode == RGB) {
        _colormode = RGB;
    } else {
        _colormode = RGBW;
    }
}

/**
    Directly set the PWM values

*/
void RGBWWLed::setRGBwWcW(uint16_t red, uint16_t green, uint16_t blue, uint16_t wwhite, uint16_t cwhite) {

    analogWrite(_redPIN, red);
    analogWrite(_greenPIN, green);
    analogWrite(_bluePIN, blue);
    analogWrite(_wwPIN, wwhite);
    analogWrite(_cwPIN, cwhite);

}


/**
*    Correct
*
*    @param red
*    @param yellow
*    @param green
*    @param cyan
*    @param blue
*    @param magenta
*
*/

void RGBWWLed::correctHSV(float red, float yellow, float green, float cyan, float blue, float magenta) {
    //reset color wheel before applying any changes
    createHueWheel();
    DEBUG("==  correctHSV  ==");
    DEBUG("==  before      ==");
    DEBUG("W0 ", _HueWheelSectorWidth[0]);
    DEBUG("W1 ", _HueWheelSectorWidth[1]);
    DEBUG("W2 ", _HueWheelSectorWidth[2]);
    DEBUG("W3 ", _HueWheelSectorWidth[3]);
    DEBUG("W4 ", _HueWheelSectorWidth[4]);
    DEBUG("W5 ", _HueWheelSectorWidth[5]);
    DEBUG(" ");
    DEBUG("B0 ", _HueWheelSector[0]);
    DEBUG("B1 ", _HueWheelSector[1]);
    DEBUG("B2 ", _HueWheelSector[2]);
    DEBUG("B3 ", _HueWheelSector[3]);
    DEBUG("B4 ", _HueWheelSector[4]);
    DEBUG("B5 ", _HueWheelSector[5]);
    DEBUG("B6 ", _HueWheelSector[6]);
    DEBUG(" ");

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


    DEBUG("==  after      ==");
    DEBUG("W0 ", _HueWheelSectorWidth[0]);
    DEBUG("W1 ", _HueWheelSectorWidth[1]);
    DEBUG("W2 ", _HueWheelSectorWidth[2]);
    DEBUG("W3 ", _HueWheelSectorWidth[3]);
    DEBUG("W4 ", _HueWheelSectorWidth[4]);
    DEBUG("W5 ", _HueWheelSectorWidth[5]);
    DEBUG(" ");
    DEBUG("B0 ", _HueWheelSector[0]);
    DEBUG("B1 ", _HueWheelSector[1]);
    DEBUG("B2 ", _HueWheelSector[2]);
    DEBUG("B3 ", _HueWheelSector[3]);
    DEBUG("B4 ", _HueWheelSector[4]);
    DEBUG("B5 ", _HueWheelSector[5]);
    DEBUG("B6 ", _HueWheelSector[6]);
    DEBUG(" ");
}

/**
    Converts HSV values to RGB colorspace


    @param hue Hue of HSV [0 - 360]
	@param sat Saturation of HSV [0 - 100]
    @param val Value of HSV [0 - 100]
*/
void RGBWWLed::HSVtoRGB(float hue, float sat, float val, int colors[3]) {
	HSVtoRGB(parseHue(hue), parseSat(sat), parseVal(val), colors);
}


void RGBWWLed::HSVtoRGB(int hue, int sat, int val, int colors[3]) {
    int red_val, green_val, blue_val, fract, chroma, m;

    //gamma correction
    //val = dim_curve[val];
    //sat = _pwmMaxVal-dim_curve[_pwmMaxVal-sat];

    DEBUG("==  HSV2RGB  ==");
    DEBUG("HUE ", hue);
    DEBUG("SAT ", sat);
    DEBUG("VAL ", val);
    if(sat == 0) {
        /* color is grayscale */
        colors[0] = val;
        colors[1] = val;
        colors[2] = val;
    } else {
        /*
        We have 6 sectors
        We need 7 "borders" to incorporate the shift oft sectors

        Example:
        Sector 1 is from 0 - 255. We need border 0 as lower border, 255 as upper border.
        Sector 6 is from 1275 - 1530. Lower border 1275, upper border 1530

        Apply a color correct for red with +10deg (value 25) results in
        Sector 1 from 0 - 255 & 1505 - 1530
        Sector 6 from 1275 - 1505

        Apply a color correct for red with -10deg (value 25) results in
        Sector 1 from 25 - 255
        Sector 6 from 1275 - 1530 && 0 - 25
        */
        chroma = (sat * val)/_pwmMaxVal;
        m = val - chroma;
        DEBUG("CHR ",chroma);
        DEBUG("M   ", m);
        DEBUG("===============");

        if ( hue < _HueWheelSector[0] || (hue > _HueWheelSector[5] && hue <= _HueWheelSector[6])) {
            DEBUG("Sector 6");
            if (hue < _HueWheelSector[0]) {
                fract = _pwmMaxVal + hue ;
            } else {
                fract = hue - (_pwmMaxVal * 5);
            }
            red_val = chroma;
            green_val = 0;
            //blue_val = (65280 - sat * (hue - 1275)) >> 8;
            //blue_val = _pwmMaxVal - ((sat * ((_pwmMaxVal * fract) / _HueWheelSectorWidth[5])) >> _pwmDepth);
            blue_val= ( chroma * (_pwmMaxVal - (_pwmMaxVal * fract) / _HueWheelSectorWidth[5])) >> _pwmDepth;

        //} else if ( ( hue >= _HueWheelSector[0] && hue <= _HueWheelSector[1] ) || hue > _HueWheelSector[6]) {
        } else if (  hue <= _HueWheelSector[1]  || hue > _HueWheelSector[6]) {
            // Sector 1
            DEBUG("Sector 1");
            if (hue > _HueWheelSector[6]) {
                fract = hue - _HueWheelSector[6];
            } else {
                fract = hue + (_pwmHueWheelMax - _HueWheelSector[6]);
            }
            red_val = chroma;
            //green_val = (65280 - sat * (255 - hue)) >> 8;
            // chroma * (1 - | h mod 2 -1|)
             //green_val =  _pwmMaxVal - ((sat * (_pwmMaxVal - ((_pwmMaxVal * fract) / _HueWheelSectorWidth[0]))) >> _pwmDepth);
            green_val = (chroma * ((_pwmMaxVal * fract) / _HueWheelSectorWidth[0])) >> _pwmDepth;
            blue_val = 0;



        } else if (hue <= _HueWheelSector[2]) {
            // Sector 2
            DEBUG("Sector 2");
            fract = hue - _pwmMaxVal;
            //red_val = (65280 - sat * (hue - 255)) >> 8;
            // chroma * (1 - | h mod 2 -1|)
            //red_val = _pwmMaxVal - (( sat * ((_pwmMaxVal * fract) / _HueWheelSectorWidth[1]))>> _pwmDepth);
            red_val = (chroma * (_pwmMaxVal - (_pwmMaxVal * fract) / _HueWheelSectorWidth[1])) >> _pwmDepth;
            green_val = chroma;
            blue_val = 0;

        } else if (hue <= _HueWheelSector[3]) {
            // Sector 3
            DEBUG("Sector 3");
            fract = hue - (_pwmMaxVal * 2);
            red_val = 0;
            green_val = chroma;
            //blue_val = (65280 - sat * (765 - hue)) >> 8;
            //blue_val = _pwmMaxVal - ((sat * (_pwmMaxVal - ((_pwmMaxVal * fract) / _HueWheelSectorWidth[2]))) >> _pwmDepth);
            blue_val = (chroma * ((_pwmMaxVal * fract) / _HueWheelSectorWidth[2])) >> _pwmDepth;

        } else if (hue <= _HueWheelSector[4]) {
            // Sector 4
            DEBUG("Sector 4");
            fract = hue - (_pwmMaxVal * 3);
            red_val = 0;
            //green_val = (65280 - sat * (hue - 765)) >> 8;
            //green_val = _pwmMaxVal - ((sat * ((_pwmMaxVal * fract) / _HueWheelSectorWidth[3])) >> _pwmDepth);
            green_val =(chroma * (_pwmMaxVal - (_pwmMaxVal * fract) / _HueWheelSectorWidth[3])) >> _pwmDepth;
            blue_val = chroma;

        } else  {
            // Sector 5
            DEBUG("Sector 5");
            fract = hue - (_pwmMaxVal * 4);
            //red_val = (65280 - sat * (1275 - hue)) >> 8;
            //red_val = _pwmMaxVal - (( sat * (_pwmMaxVal - ((_pwmMaxVal * fract) / _HueWheelSectorWidth[4])))>> _pwmDepth);
            red_val = (chroma * ((_pwmMaxVal * fract) / _HueWheelSectorWidth[4])) >> _pwmDepth;
            green_val = 0;
            blue_val = chroma;

        }
    // m equals the white part
    // for rgbw we use it for the white channels
    if (_colormode == RGBW) {
        colors[0] = red_val;
        colors[1] = green_val;
        colors[2] = blue_val;
        colors[3] = m;

        DEBUG("===============");
        DEBUG("R ", colors[0]);
        DEBUG("G ",colors[1]);
        DEBUG("B ", colors[2]);
        DEBUG("W ", colors[3]);
        DEBUG("===============");
    } else {
        colors[0] = red_val + m;
        colors[1] = green_val + m;
        colors[2] = blue_val + m;

        DEBUG("===============");
        DEBUG("R ", colors[0]);
        DEBUG("G ", colors[1]);
        DEBUG("B ", colors[2]);
        DEBUG("===============");
    }



    }
}



/*
Helper function to create the 6 sectors for the HUE wheel
*/
void RGBWWLed::createHueWheel() {
    _HueWheelSector[0] = 0;
    for (int i = 1; i <= 6; ++i) {
        _HueWheelSector[i] = i*_pwmMaxVal;
        _HueWheelSectorWidth[i-1] = _pwmMaxVal;
    }
}

/*
Helper function to keep Hue between 0 - HueWheelMax
*/
int RGBWWLed::circleHue(int hue ) {
    while (hue >= _pwmHueWheelMax) hue -= _pwmHueWheelMax;
    while (hue < 0) hue += _pwmHueWheelMax;
    return hue;
}

/*
Helper functions to parse hue,sat,val,colorcorrection
Converts to the according pwm size (8bit/10bit)
*/
int RGBWWLed::parseColorCorrection(float val) {
    if (val >= 29.5) val = 29.5;
    if (val <= -29.5) val = -29.5;
    return int(((val / 60) * (_pwmMaxVal)) * -1);
}

int RGBWWLed::parseHue(float hue) {
    hue = constrain(hue, 0.0, 360.0);
    hue = int((hue / hueV) * (_pwmHueWheelMax));
	return hue;
}

int RGBWWLed::parseSat(float sat) {
	sat = constrain(sat, 0.0, 100.0);
	return int((sat / satV) * (_pwmMaxVal));
}

int RGBWWLed::parseVal(float val){
	val = constrain(val, 0.0, 100.0);
	return int((val / valV) * (_pwmMaxVal));
}



