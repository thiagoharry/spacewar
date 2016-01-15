/*
  Copyright (C) 2009 Thiago Leucz Astrizi

 This file is part of Weaver API.

 Weaver API is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Weaver API is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with Weaver API.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <X11/Xlib.h>
#include <png.h>
#include "display.h"

#define _alpha_composite(composite, fg, alpha, bg){\
    unsigned short temp = ((unsigned short)(fg)*(unsigned short)(alpha) +\
                           (unsigned short)(bg)*(unsigned short)(255 - (unsigned short)(alpha)) + (unsigned short)128); \
    (composite) = (unsigned char)((temp + (temp >> 8)) >> 8);           \
  }



png_structp _png_ptr;
png_infop _info_ptr;
long _width, _height;
int _bit_depth, _color_type;

unsigned long _image_width, _image_height, _image_rowbytes;
int _image_channels;

unsigned char *_image_data;


struct surface *new_image(char *);
void _print_png_version(void);
int _readpng_init(FILE *, long *, long *);
int _readpng_get_bgcolor(unsigned char *, unsigned char *, unsigned char *);
unsigned char *_readpng_get_image(double, int *, unsigned long *);
void _readpng_cleanup(int);
int _rpng_x_msb(unsigned long);
int _display_image(struct surface *, int, int);

#endif
