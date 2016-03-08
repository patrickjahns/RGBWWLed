/**************************************************************


    @author Patrick Jahns
 **************************************************************/
#ifndef RGBWWLedColor_h
#define RGBWWLedColor_h
#include "RGBWWLed.h"



//struct for RGBW + Kelvin
struct RGBWK {
    int r, g, b, w, k;
    RGBWK() {}
    //TODO: default value für White color?
    RGBWK(int red, int green, int blue, int white) : r(red), g(green), b(blue), w(white), k(0) {}
    RGBWK(int red, int green, int blue, int white, int kelvin) : r(red), g(green), b(blue), w(white), k(kelvin) {}
    RGBWK(const RGBWK& rgbwk)
    {
        r = rgbwk.r;
        g = rgbwk.g;
        b = rgbwk.b;
        w = rgbwk.w;
        k = rgbwk.k;
    }

    RGBWK& operator= (const RGBWK& rgbwk)
    {
        r = rgbwk.r;
        g = rgbwk.g;
        b = rgbwk.b;
        w = rgbwk.w;
        k = rgbwk.k;
        return *this;
    }
};



// struct for HSV + Kelvin
struct HSVK {
    int h, s, v, k;
    HSVK() {}
    //TODO: default value für White color?
    HSVK(int hue, int sat, int val) : h(hue), s(sat), v(val), k(0) {}
    HSVK(int hue, int sat, int val, int kelvin) : h(hue), s(sat), v(val), k(kelvin) {}

    //construct from float values
    HSVK( float hue, float sat, float val) {
        h = (constrain(hue, 0.0, 360.0) / 360) * (RGBWW_PWMHUEWHEELMAX);
        s = (constrain(sat, 0.0, 100.0) / 100) * (RGBWW_PWMMAXVAL);
        v = (constrain(val, 0.0, 100.0) / 100) * (RGBWW_PWMMAXVAL);
        //TODO: default value für White color?
        k = 0;
    }

    HSVK( float hue, float sat, float val, int kelvin) {
        h = (constrain(hue, 0.0, 360.0) / 360) * (RGBWW_PWMHUEWHEELMAX);
        s = (constrain(sat, 0.0, 100.0) / 100) * (RGBWW_PWMMAXVAL);
        v = (constrain(val, 0.0, 100.0) / 100) * (RGBWW_PWMMAXVAL);
        k = constrain(kelvin, 0, 10000);
    }

    HSVK(const HSVK& hsvk)
    {
        h = hsvk.h;
        s = hsvk.s;
        v = hsvk.v;
        k = hsvk.k;
    }

    HSVK& operator= (const HSVK& hsvk)
    {
        h = hsvk.h;
        s = hsvk.s;
        v = hsvk.v;
        k = hsvk.k;
        return *this;
    }

    void asRadian(float& hue, float& sat, float& val) {
		hue = (float(h) / float(RGBWW_PWMHUEWHEELMAX)) * 360.0;
		sat = (float(s) / float(RGBWW_PWMMAXVAL)) * 100.0;
		val = (float(v) / float(RGBWW_PWMMAXVAL)) * 100.0;
    }
    void asRadian(float& hue, float& sat, float& val, int& kelvin) {
    	hue = (float(h) / float(RGBWW_PWMHUEWHEELMAX)) * 360.0;
		sat = (float(s) / float(RGBWW_PWMMAXVAL)) * 100.0;
		val = (float(v) / float(RGBWW_PWMMAXVAL)) * 100.0;
		kelvin = k;
    }



};

#endif
