/**************************************************************


    @author Patrick Jahns
 **************************************************************/
#ifndef RGBWWLedColor_h
#define RGBWWLedColor_h
#include "RGBWWLed.h"


//struct for RGBW
struct RGBW {
    int r, g, b, w;
    RGBW() {}
    RGBW(int red, int green, int blue, int white) : r(red), g(green), b(blue), w(white) {}
    RGBW(const RGBW& rgbw)
    {
        r = rgbw.r;
        g = rgbw.g;
        b = rgbw.b;
        w = rgbw.w;
    }

    RGBW& operator= (const RGBW& rgbw)
    {
        r = rgbw.r;
        g = rgbw.g;
        b = rgbw.b;
        w = rgbw.w;
        return *this;
    }
};
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

// struct for HSV
struct HSV {
    int h, s, v;
    HSV() {}
    HSV(int hue, int sat, int val) : h(hue), s(sat), v(val) {}

    //construct from float values
    HSV( float hue, float sat, float val) {
        h = (constrain(hue, 0.0, 360.0) / 360) * (PWMHUEWHEELMAX);
        s = (constrain(sat, 0.0, 100.0) / 100) * (PWMMAXVAL);
        v = (constrain(val, 0.0, 100.0) / 100) * (PWMMAXVAL);
    }

    HSV(const HSV& hsv)
    {
        h = hsv.h;
        s = hsv.s;
        v = hsv.v;
    }

    HSV& operator= (const HSV& hsv)
    {
        h = hsv.h;
        s = hsv.s;
        v = hsv.v;
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
        h = (constrain(hue, 0.0, 360.0) / 360) * (PWMHUEWHEELMAX);
        s = (constrain(sat, 0.0, 100.0) / 100) * (PWMMAXVAL);
        v = (constrain(val, 0.0, 100.0) / 100) * (PWMMAXVAL);
        //TODO: default value für White color?
        k = 0;
    }

    HSVK( float hue, float sat, float val, int kelvin) {
        h = (constrain(hue, 0.0, 360.0) / 360) * (PWMHUEWHEELMAX);
        s = (constrain(sat, 0.0, 100.0) / 100) * (PWMMAXVAL);
        v = (constrain(val, 0.0, 100.0) / 100) * (PWMMAXVAL);
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

};

#endif
