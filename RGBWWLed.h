/**************************************************************


    @author Patrick Jahns
    @version 0.5
 **************************************************************/

#ifndef RGBWWLed_h
#define RGBWWLed_h


#define RGBWW_VERSION "0.5"
#define PWMDEPTH 10
#define PWMWIDTH int(pow(2, PWMDEPTH))
#define	PWMMAXVAL int(PWMWIDTH - 1)
#define	PWMHUEWHEELMAX int(PWMMAXVAL * 6)

#define UPDATEFREQUENCY 50
#define MINTIMEDIFF  int(1000 / UPDATEFREQUENCY)
#define ANIMATIONQSIZE 50
#define	WARMWHITEKELVIN 2700
#define COLDWHITEKELVIN 6000

#include <ARDUINO.h>
#ifndef ARDUINO
	#include "../../SmingCore/SmingCore.h"
#endif
#define USE_ESP_HWPWM
#define DEBUG_RGB
#include "debugUtils.h"
#include "RGBWWLedColor.h"
#include "RGBWWLedAnimation.h"
#include "RGBWWLedOutput.h"


enum COLORMODE {
	RGB = 0,
	RGBWW = 1,
	RGBCW = 2,
	RGBWWCW = 3
};

enum HSVMODE {
	NORMAL = 0,
	SPEKTRUM = 1,
	RAINBOW = 2
};

class RGBWWLedAnimation;
class RGBWWLedAnimationQ;
class WMOutput;

class RGBWWLed
{
public:
	//init & settings
	RGBWWLed();
	virtual ~RGBWWLed();
	void		setColorMode(COLORMODE mode);
	COLORMODE 	getColorMode();
	void    	setHSVmode(HSVMODE mode);
	HSVMODE 	getHSVmode();
	void    	init(int redPIN, int greenPIN, int bluePIN, int wwPIN, int cwPIN, int pwmFrequency=200);
	void    	setHSVcorrection(float red, float yellow, float green, float cyan, float blue, float magenta);
	void    	getHSVcorrection(float& red, float& yellow, float& green, float& cyan, float& blue, float& magenta);
	void    	setBrightnessCorrection(int r, int g, int b, int ww, int cw);
	void    	getBrightnessCorrection(int& r, int& g, int& b, int& ww, int& cw);


	//output related
	bool    	show();
	void		refresh();
	void    	setOutput(HSVK color);
	void    	setOutput(RGBWK color);
	void    	setOutputRaw(int red, int green, int blue, int cwhite, int wwhite);
	HSVK    	getCurrentColor();

	//animation related
	void    	setHSV(HSVK& color);
	void    	setHSV(HSVK& color, int time, int direction);
	void    	setHSV(HSVK& color, int time, bool q);
	void    	setHSV(HSVK& color, int time, int direction, bool q);
	void    	setHSV(HSVK& colorFrom, HSVK& color, int time, int direction=1, bool q=false);
	void		setAnimationCallback( void (*func)(RGBWWLed* led) );
	bool		isAnimationActive();
	bool    	isAnimationQFull();
	void    	skipAnimation();
	void    	clearAnimationQueue();


	//colorutils
	void		whiteBalance(RGBWK& rgbw, int& ww, int& cw);
	void    	HSVtoRGB(const HSVK& hsv, RGBWK& rgbw);
	void    	HSVtoRGB(const HSVK& hsv, RGBWK& rgbw, HSVMODE mode);
	void    	HSVtoRGBn(const HSVK& hsv, RGBWK& rgbw);
	void    	HSVtoRGBspektrum(const HSVK& hsv, RGBWK& rgbw);
	void    	HSVtoRGBrainbow(const HSVK& hsv, RGBWK& rgbw);
	void    	RGBtoHSV(const RGBWK& rgbw, HSVK& hsv);


	//helpers
	int     	parseHue(float hue);
	int     	parseSat(float sat);
	int     	parseVal(float val);
	void    	circleHue(int& hue);
	int     	parseColorCorrection(float val);

private:
	int         _BrightnessFactor[5];
	int         _HueWheelSector[7];
	int         _HueWheelSectorWidth[6];
	int			_WarmWhiteKelvin;
	int			_ColdWhiteKelvin;
	
	COLORMODE       _colormode;
	HSVMODE         _hsvmode;
	HSVK            _current_color;
	RGBWK           _current_output;

	unsigned long   last_active;

	bool            _cancelAnimation;
	bool            _clearAnimationQueue;
	bool            _isAnimationActive;
	void (*_animationcallback)(RGBWWLed* led) = NULL;

	RGBWWLedAnimation*  	_currentAnimation;
	RGBWWLedAnimationQ*  	_animationQ;
	PWMOutput*				_pwm_output;

	//helpers

	void    createHueWheel();
	void    cleanupCurrentAnimation();
	void    clearAnimationQ();

};

//TODO move to progmem
/*
const uint8_t dim_curve[256] {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};
 */

const uint16_t dim_curve[1024] = {
		0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 9, 9, 9,
		9, 9, 9, 9, 9, 9, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 11, 11, 11, 11, 11,
		11, 11, 11, 12, 12, 12, 12, 12, 12, 12,
		13, 13, 13, 13, 13, 13, 13, 14, 14, 14,
		14, 14, 14, 14, 15, 15, 15, 15, 15, 15,
		16, 16, 16, 16, 16, 16, 16, 17, 17, 17,
		17, 17, 17, 18, 18, 18, 18, 18, 19, 19,
		19, 19, 19, 19, 20, 20, 20, 20, 20, 21,
		21, 21, 21, 21, 22, 22, 22, 22, 22, 23,
		23, 23, 23, 23, 24, 24, 24, 24, 24, 25,
		25, 25, 25, 26, 26, 26, 26, 26, 27, 27,
		27, 27, 28, 28, 28, 28, 28, 29, 29, 29,
		29, 30, 30, 30, 30, 31, 31, 31, 31, 32,
		32, 32, 32, 33, 33, 33, 34, 34, 34, 34,
		35, 35, 35, 35, 36, 36, 36, 37, 37, 37,
		37, 38, 38, 38, 39, 39, 39, 39, 40, 40,
		40, 41, 41, 41, 41, 42, 42, 42, 43, 43,
		43, 44, 44, 44, 45, 45, 45, 46, 46, 46,
		47, 47, 47, 48, 48, 48, 49, 49, 49, 50,
		50, 50, 51, 51, 51, 52, 52, 52, 53, 53,
		53, 54, 54, 55, 55, 55, 56, 56, 56, 57,
		57, 58, 58, 58, 59, 59, 59, 60, 60, 61,
		61, 61, 62, 62, 63, 63, 63, 64, 64, 65,
		65, 65, 66, 66, 67, 67, 68, 68, 68, 69,
		69, 70, 70, 71, 71, 71, 72, 72, 73, 73,
		74, 74, 75, 75, 75, 76, 76, 77, 77, 78,
		78, 79, 79, 80, 80, 81, 81, 82, 82, 82,
		83, 83, 84, 84, 85, 85, 86, 86, 87, 87,
		88, 88, 89, 89, 90, 90, 91, 91, 92, 93,
		93, 94, 94, 95, 95, 96, 96, 97, 97, 98,
		98, 99, 99, 100, 101, 101, 102, 102, 103, 103,
		104, 104, 105, 106, 106, 107, 107, 108, 108, 109,
		110, 110, 111, 111, 112, 113, 113, 114, 114, 115,
		116, 116, 117, 117, 118, 119, 119, 120, 120, 121,
		122, 122, 123, 124, 124, 125, 126, 126, 127, 127,
		128, 129, 129, 130, 131, 131, 132, 133, 133, 134,
		135, 135, 136, 137, 137, 138, 139, 139, 140, 141,
		141, 142, 143, 144, 144, 145, 146, 146, 147, 148,
		149, 149, 150, 151, 151, 152, 153, 154, 154, 155,
		156, 157, 157, 158, 159, 159, 160, 161, 162, 163,
		163, 164, 165, 166, 166, 167, 168, 169, 169, 170,
		171, 172, 173, 173, 174, 175, 176, 177, 177, 178,
		179, 180, 181, 181, 182, 183, 184, 185, 186, 186,
		187, 188, 189, 190, 191, 191, 192, 193, 194, 195,
		196, 196, 197, 198, 199, 200, 201, 202, 203, 203,
		204, 205, 206, 207, 208, 209, 210, 211, 211, 212,
		213, 214, 215, 216, 217, 218, 219, 220, 221, 222,
		223, 223, 224, 225, 226, 227, 228, 229, 230, 231,
		232, 233, 234, 235, 236, 237, 238, 239, 240, 241,
		242, 243, 244, 245, 246, 247, 248, 249, 250, 251,
		252, 253, 254, 255, 256, 257, 258, 259, 260, 261,
		262, 263, 264, 265, 266, 267, 268, 269, 271, 272,
		273, 274, 275, 276, 277, 278, 279, 280, 281, 282,
		284, 285, 286, 287, 288, 289, 290, 291, 292, 294,
		295, 296, 297, 298, 299, 300, 301, 303, 304, 305,
		306, 307, 308, 310, 311, 312, 313, 314, 315, 317,
		318, 319, 320, 321, 323, 324, 325, 326, 327, 329,
		330, 331, 332, 333, 335, 336, 337, 338, 340, 341,
		342, 343, 345, 346, 347, 348, 350, 351, 352, 353,
		355, 356, 357, 359, 360, 361, 362, 364, 365, 366,
		368, 369, 370, 372, 373, 374, 376, 377, 378, 380,
		381, 382, 384, 385, 386, 388, 389, 390, 392, 393,
		394, 396, 397, 399, 400, 401, 403, 404, 405, 407,
		408, 410, 411, 412, 414, 415, 417, 418, 420, 421,
		422, 424, 425, 427, 428, 430, 431, 433, 434, 435,
		437, 438, 440, 441, 443, 444, 446, 447, 449, 450,
		452, 453, 455, 456, 458, 459, 461, 462, 464, 465,
		467, 468, 470, 472, 473, 475, 476, 478, 479, 481,
		482, 484, 486, 487, 489, 490, 492, 493, 495, 497,
		498, 500, 501, 503, 505, 506, 508, 510, 511, 513,
		514, 516, 518, 519, 521, 523, 524, 526, 528, 529,
		531, 533, 534, 536, 538, 539, 541, 543, 544, 546,
		548, 550, 551, 553, 555, 556, 558, 560, 562, 563,
		565, 567, 569, 570, 572, 574, 576, 577, 579, 581,
		583, 584, 586, 588, 590, 592, 593, 595, 597, 599,
		601, 602, 604, 606, 608, 610, 612, 613, 615, 617,
		619, 621, 623, 625, 626, 628, 630, 632, 634, 636,
		638, 640, 641, 643, 645, 647, 649, 651, 653, 655,
		657, 659, 661, 662, 664, 666, 668, 670, 672, 674,
		676, 678, 680, 682, 684, 686, 688, 690, 692, 694,
		696, 698, 700, 702, 704, 706, 708, 710, 712, 714,
		716, 718, 720, 722, 724, 726, 728, 731, 733, 735,
		737, 739, 741, 743, 745, 747, 749, 751, 753, 756,
		758, 760, 762, 764, 766, 768, 770, 773, 775, 777,
		779, 781, 783, 786, 788, 790, 792, 794, 796, 799,
		801, 803, 805, 807, 810, 812, 814, 816, 819, 821,
		823, 825, 827, 830, 832, 834, 837, 839, 841, 843,
		846, 848, 850, 852, 855, 857, 859, 862, 864, 866,
		869, 871, 873, 876, 878, 880, 883, 885, 887, 890,
		892, 894, 897, 899, 901, 904, 906, 909, 911, 913,
		916, 918, 921, 923, 925, 928, 930, 933, 935, 938,
		940, 942, 945, 947, 950, 952, 955, 957, 960, 962,
		965, 967, 970, 972, 975, 977, 980, 982, 985, 987,
		990, 992, 995, 997, 1000, 1002, 1005, 1008, 1010, 1013,
		1015, 1018, 1020, 1023,
};

#endif //RGBWWLed_h
