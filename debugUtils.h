/**
 * RGBWWLed - simple Library for controlling RGB WarmWhite ColdWhite LEDs via PWM
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * All files of this project are provided under the LGPL v3 license.
 */
#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

    #ifdef DEBUG_RGBWW
		#if DEBUG_RGBWW == 1
			#ifdef ARDUINO
				#define debugRGBW(fmt, ...) Serial.printf(fmt"\r\n", ##__VA_ARGS__)
			#else
				#define debugRGBW(fmt, ...) printf(fmt"\r\n", ##__VA_ARGS__)
			#endif //ARDUINO
		#else
			#define debugRGBW(...)
		#endif // DEBUG_RGBWW
	#else
		
		#define debugRGBW(...)
		
	#endif // DEBUG_RGBWW


#endif //DEBUGUTILS_H
