/**
 * @file
 * @author  Patrick Jahns http://github.com/patrickjahns
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
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
