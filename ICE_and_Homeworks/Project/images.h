// Copyright (c) 2015-18, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef __IMAGES_H__
#define __IMAGES_H__

#include <stdint.h>

/* Bitmap info for space_ship */
extern const uint8_t hotdog_Bitmap[];
extern const uint8_t hotdog_width;
extern const uint8_t hotdog_height;

extern const uint8_t octopus_width;
extern const uint8_t octopus_height;
extern const uint8_t octopus_Bitmap[];

extern const uint8_t fish_width;
extern const uint8_t fish_height;
extern const uint8_t fishRight_Bitmap[];
extern const uint8_t fishLeft_Bitmap[];


// maybe put in other file


extern const uint8_t start_map[];
extern const uint16_t start_width;
extern const uint16_t start_height;

extern const uint8_t endscreen_Bitmap[];

#endif
