/*
debugUtils.h - Simple debugging utilities.
*/

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

    #ifdef DEBUG_RGBWW
		#ifdef ARDUINO
			#define debugRGBW(fmt, ...) Serial.printf(fmt"\r\n", ##__VA_ARGS__)
		#else 
			#define debugRGBW(fmt, ...) printf(fmt"\r\n", ##__VA_ARGS__)
		#endif //ARDUINO
	#else
		
		#define debugRGBW(...)
		
	#endif // USE_DEBUG


#endif //DEBUGUTILS_H
