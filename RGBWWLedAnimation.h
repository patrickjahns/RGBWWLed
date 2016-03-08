/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
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

#ifndef RGBWWLedAnimation_h
#define RGBWWLedAnimation_h
#include "RGBWWLed.h"
#include "RGBWWLedColor.h"


class RGBWWLed;
class RGBWWLedAnimation;

/**
 * A simple queue implementation
 *
 */
class RGBWWLedAnimationQ
{
public:
	RGBWWLedAnimationQ() {};
	RGBWWLedAnimationQ(int qsize);
	~RGBWWLedAnimationQ();

	/**
	 * Check if the queue is empty or not
	 *
	 * @return	bool
	 */
	bool isEmpty();

	/**
	 * Check if the queue is full
	 *
	 * @return	BOOL
	 */
	bool isFull();

	/**
	 * Add an animation to the queue
	 *
	 * @param RGBWWLedAnimation* 	animation	pointer to Animation object
	 * @return	bool
	 * @retval 	true 	successfully inserted object queue
	 * @retval	false	did not insert object into queue
	 */
	bool push(RGBWWLedAnimation* animation);

	/**
	 * Empty Queue and delete all objects stored
	 */
	void clear();

	/**
	 * Returns first Animation object pointer but keeps it in the queue
	 *
	 * @return RGBWWLedAnimation*
	 */
	RGBWWLedAnimation*  peek();

	/**
	 *	Returns first Animation object pointer and removes it from queue
	 *
	 * @return RGBWWLedAnimation*
	 */
	RGBWWLedAnimation*  pop();


private:
	int _size, _count, _front, _back;
	RGBWWLedAnimation** q;

};

/**
 * Abstract class representing the interface for animations
 *
 */
class RGBWWLedAnimation
{
public:


	virtual ~RGBWWLedAnimation() {};
	/**
	 * Main method that will be called from processing loop
	 * Output/Calculation steps should be done here
	 *
	 * @return bool		value representing the status of the animation
	 * @retval true		the animation is finished
	 * @retval false 	the animation is not finished yet
	 */
	virtual bool run() {return true;};


	/**
	 * Generic interface method for changing a variable
	 * representing the speed of the animation
	 *
	 * @param newspeed
	 */
	virtual void setSpeed(int newspeed) {};

	/**
	 * Generic interface method for changing a variable
	 * representing the brightness of the animation
	 *
	 * @param newbrightness
	 */
	virtual void setBrightness(int newbrightness) {};
};

/**
 * Simple Animation Object to set the output to a certain color
 * without effects/transition
 *
 */
class HSVSetOutput: public RGBWWLedAnimation
{
public:

	/**
	 * Simple Animation Object to set the output to a certain color
	 * without effects/transition
	 *
	 * @param color New color to show
	 * @param ctrl	Pointer to RGBWWLed controller objekt
	 */
	HSVSetOutput(const HSVK& color, RGBWWLed* rgbled);
	bool run();


private:
	RGBWWLed* rgbwwctrl;
	HSVK outputcolor;
};



/**
 * A simple Colorfade animation using HSV
 *
 */
class HSVTransition: public RGBWWLedAnimation
{
public:

	/**
	 * Simple Anination to fade from the current color to another color (colorFinish)
	 * There are two options for the direction of the fade (short way/ long way)
	 *
	 * @param colorFinish	color where the animation should end
	 * @param time			the amount of time the animation takes in ms
	 * @param direction 	shortest (direction == 0)/longest (direction == 1) way for transition
	 * @param ctrl			main rgbww objekt
	 */
	HSVTransition(const HSVK& colorFinish, const int& time, const int& direction, RGBWWLed* ctrl);

	/**
	 * Simple Anination to fade from one color (colorFrom) to another color (colorFinish)
	 * There are two options for the direction of the fade (short way/ long way)
	 *
	 * @param colorFrom		color from which the animation should start
	 * @param colorFinish	color where the animation should end
	 * @param time			the amount of time the animation takes in ms
	 * @param direction 	shortest (direction == 0)/longest (direction == 1) way for transition
	 * @param ctrl			main rgbww objekt
	 */
	HSVTransition(const HSVK& colorFrom, const HSVK& colorFinish, const int& time, const int& direction, RGBWWLed* ctrl);

	bool run();

private:
	void        init();

	HSVK	_basecolor;
	HSVK	_currentcolor;
	HSVK	_finalcolor;
	bool	_hasbasecolor;
	int	_currentstep;
	int _steps;
	int _huedirection;
	int	_dhue;
	int _hueerror;
	int _huecount;
	int _huestep;
	int _dsat;
	int _saterror;
	int _satcount;
	int _satstep;
	int _dval;
	int _valerror;
	int _valcount;
	int _valstep;
	int _dkelvin;
	int _kelvinerror;
	int _kelvincount;
	int _kelvinstep;

	RGBWWLed*    rgbwwctrl;

	static 	int	bresenham(int& error, int& ctr, int& dx, int& dy, int& incr, int& base, int& current);

};

#endif // RGBWWLedAnimation_h
