/**************************************************************


    @author Patrick Jahns
    @version 0.1
 **************************************************************/

#ifndef RGBWWLedAnimation_h
#define RGBWWLedAnimation_h
#include "RGBWWLedColor.h"
#include <queue>
#include <vector>

class RGBWWLed;
class RGBWWLedAnimation;

class RGBWWLedAnimationQ
{
    public:
        RGBWWLedAnimationQ() {};
        RGBWWLedAnimationQ(int qsize);
        bool                isEmpty();
        bool                isFull();
        bool                push(RGBWWLedAnimation* animation);
        RGBWWLedAnimation*  peek();
        RGBWWLedAnimation*  pop();


    private:
        int _size, _count;
        std::queue<RGBWWLedAnimation*>  q;

};

class RGBWWLedAnimation
{
    public:
        virtual bool run() {return true;};
        virtual bool run(int st) {return true;};
        virtual ~RGBWWLedAnimation() {};
};

class HSVTransition: public RGBWWLedAnimation
{
    public:
        HSVTransition() {};
        HSVTransition(HSVK colorFrom, HSVK color, const int& tm, const int& direction, RGBWWLed* rgbled);
        bool        run();
        bool        run(int st);

    private:
        HSVK         _basecolor;
        HSVK         _currentcolor;
        HSVK         _finalcolor;
        int         _currentstep;
        int         _steps;
        int         _dhue;
        int         _hueerror;
        int         _huecount;
        int         _huestep;
        int         _dsat;
        int         _saterror;
        int         _satcount;
        int         _satstep;
        int         _dval;
        int         _valerror;
        int         _valcount;
        int         _valstep;
        int         _dkelvin;
        int         _kelvinerror;
        int         _kelvincount;
        int         _kelvinstep;

        RGBWWLed*    rgbled;

        int         bresenham(int& error, int& ctr, int& dx, int& dy, int& incr, int& base, int& current);

};

#endif // RGBWWLedAnimation_h
