/**************************************************************************
 *
 * FILE NAME:			bitmapDb.h
 * FILE DESCRIPTION:	Database of all bitmaps in the system
 *
 * FILE CREATION DATE:	24-07-2009
 *
 *==========================================================================
 ***************************************************************************/

#ifndef __ALPHABET_H_
#define __ALPHABET_H_

// ==========================================================================
// structure definition
// ==========================================================================
#include <stdint.h>
// This structure describes a single character's display information
typedef struct
{
	const uint8_t widthBits;					// width, in bits (or pixels), of the character
	
	const uint16_t offset;					// offset of the character's bitmap, in bytes, into the the FONT_INFO's data array
	
} FONT_CHAR_INFO;	

extern const uint8_t alphabet_Bitmap[];
extern const FONT_CHAR_INFO alphabet_Descriptors[];

#endif /* __ALPHABET_H_ */
