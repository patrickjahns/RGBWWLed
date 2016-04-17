/**
 * RGBWWLed - simple Library for controlling RGB WarmWhite ColdWhite LEDs via PWM
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * All files of this project are provided under the LGPL v3 license.
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
	RGBWWLedAnimationQ(int qsize);
	~RGBWWLedAnimationQ();

	/**
	 * Check if the queue is empty or not
	 *
	 * @return	bool
	 * @retval	true	queue is empty
	 * @retval	false	queue is not empty
	 */
	bool isEmpty();

	/**
	 * Check if the queue is full
	 *
	 * @return	bool
	 * @retval	true	queue is full
	 * @retval	false	queue is not full
	 */
	bool isFull();

	/**
	 * Add an animation to the queue
	 *
	 * @param 	animation
	 * @return	if the object was inserted successfully
	 * @retval 	true 	successfully inserted object
	 * @retval	false	failed to insert objec
	 */
	bool push(RGBWWLedAnimation* animation);

	/**
	 * Empty queue and delete all objects stored
	 */
	void clear();

	/**
	 * Returns first animation object pointer but keeps it in the queue
	 *
	 * @return RGBWWLedAnimation*
	 */
	RGBWWLedAnimation*  peek();

	/**
	 *	Returns first animation object pointer and removes it from queue
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
	 * Processing method, will be called from main loop
	 *
	 * @return status of the animation
	 * @retval true		the animation is finished
	 * @retval false 	the animation is not finished yet
	 */
	virtual bool run() {return true;};


	/**
	 * Generic interface method for changing a variable
	 * representing the speed of the current active animation
	 *
	 * @param newspeed
	 */
	virtual void setSpeed(int newspeed) {};

	/**
	 * Generic interface method for changing a variable
	 * representing the brightness of the current active animation
	 *
	 * @param newbrightness
	 */
	virtual void setBrightness(int newbrightness) {};

	/**
	 * Interface to reset the animation so it can run from the beginning
	 *
	 */
	virtual void reset() {};
};

/**
 * Set output to color without effect/transition
 *
 */
class HSVSetOutput: public RGBWWLedAnimation
{
public:

	/**
	 * Set output to color without effect/transition
	 *
	 * @param color color to show
	 * @param ctrl	pointer to RGBWWLed controller object
	 * @param time  minimal amount of time the color stays active
	 */
	HSVSetOutput(const HSVCT& color, RGBWWLed* rgbled, int time = 0);

	bool run();


private:
	int count;
	int steps;
	RGBWWLed* rgbwwctrl;
	HSVCT outputcolor;
};


struct BresenhamValues {
	int delta, error, count, step;
};

/**
 * A simple Colorfade utilizing the HSV colorspace
 *
 */
class HSVTransition: public RGBWWLedAnimation
{
public:


	/**
	 * Fade from the current color to another color (colorEnd).
	 *
	 * @param colorEnd		color at the end
	 * @param time			the amount of time the transition takes in ms
	 * @param direction 	shortest (direction == 0)/longest (direction == 1) way for transition
	 * @param ctrl			main RGBWWLed object for calling setOutput
	 */
	HSVTransition(const HSVCT& colorEnd, const int& time, const int& direction, RGBWWLed* ctrl);

	/**
	 * Fade from one color (colorFrom) to another color (colorFinish)
	 *
	 * @param colorFrom		color at the beginning
	 * @param colorEnd		color at the end of the transition
	 * @param time			the amount of time the transition takes in ms
	 * @param direction 	shortest (direction == 0)/longest (direction == 1) way for transition
	 * @param ctrl			main RGBWWLed object for calling setOutput
	 */
	HSVTransition(const HSVCT& colorFrom, const HSVCT& colorEnd, const int& time, const int& direction, RGBWWLed* ctrl);

	void reset();
	bool run();

private:
	bool init();

	HSVCT	_basecolor;
	HSVCT	_currentcolor;
	HSVCT	_finalcolor;
	bool	_hasbasecolor;
	int	_currentstep;
	int _steps;
	int _huedirection;
	BresenhamValues hue;
	BresenhamValues sat;
	BresenhamValues val;
	BresenhamValues ct;


	RGBWWLed*    rgbwwctrl;
	static	int bresenham(BresenhamValues& values, int& dx, int& base, int& current);
};



/**
 * Set output to a new state without effect/transition
 *
 */
class RAWSetOutput: public RGBWWLedAnimation
{
public:

	/**
	 * Set output to color without effect/transition
	 *
	 * @param output output to set
	 * @param ctrl	pointer to RGBWWLed controller object
	 * @param time  minimal amount of time the output stays active
	 */
	RAWSetOutput(const ChannelOutput& output, RGBWWLed* rgbled, int time = 0);

	bool run();


private:
	int count;
	int steps;
	RGBWWLed* rgbwwctrl;
	ChannelOutput outputcolor;
};


/**
 * A simple Fade from one output state to another
 *
 */
class RAWTransition: public RGBWWLedAnimation
{
public:


	/**
	 * Fade from the current output to a new one (output)
	 *
	 * @param output		output at the end of the transition
	 * @param time			the amount of time the transition takes in ms
	 * @param ctrl			main RGBWWLed object for calling setOutput
	 */
	RAWTransition(const ChannelOutput& output, const int& time, RGBWWLed* ctrl);

	/**
	 * Fade from one output state (output_from) to another(output)
	 *
	 * @param output_from	output at the beginning
	 * @param output		output at the end of the transition
	 * @param time			the amount of time the transition takes in ms
	 * @param ctrl			main RGBWWLed object for calling setOutput
	 */
	RAWTransition(const ChannelOutput& output_from, const ChannelOutput& output, const int& time, RGBWWLed* ctrl);

	void reset();
	bool run();

private:
	bool init();

	ChannelOutput	_basecolor;
	ChannelOutput	_currentcolor;
	ChannelOutput	_finalcolor;
	bool	_hasbasecolor;
	int	_currentstep;
	int _steps;
	BresenhamValues red;
	BresenhamValues green;
	BresenhamValues blue;
	BresenhamValues warmwhite;
	BresenhamValues coldwhite;


	RGBWWLed*    rgbwwctrl;
	static	int bresenham(BresenhamValues& values, int& dx, int& base, int& current);
};


/**
 *
 */
class RGBWWAnimationSet: public RGBWWLedAnimation {
public:
	/**
	 *
	 * @param animations
	 * @param count
	 * @param loop
	 */
	RGBWWAnimationSet(RGBWWLedAnimation** animations, int count, bool loop=false);

	~RGBWWAnimationSet();


	/**
	 *
	 * @param newspeed
	 */
	void setSpeed(int newspeed);


	/**
	 *
	 * @param newbrightness
	 */
	void setBrightness(int newbrightness);

	bool run();

private:
	int _current = 0;
	int _count;
	int _brightness = -1;
	int _speed = -1;
	bool _loop;
	RGBWWLedAnimation** q;

};

#endif // RGBWWLedAnimation_h
