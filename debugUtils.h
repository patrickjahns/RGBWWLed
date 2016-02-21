/*
debugUtils.h - Simple debugging utilities.
*/

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

    #ifdef USE_DEBUG
		#ifndef SMING_VERSION
			#ifdef ARDUINO
				#define debugf(fmt, ...) Serial.printf(fmt"\r\n", ##__VA_ARGS__)
			#else 
				#define debugf(fmt, ...) printf(fmt"\r\n", ##__VA_ARGS__)
			#endif //ARDUINO
		
		#endif //SMING_VERSION
		
	#else
		#define debugf(...)
		#define SERIAL_DEBUG_SETUP(SPEED)
		
	#endif // USE_DEBUG


#endif //DEBUGUTILS_H