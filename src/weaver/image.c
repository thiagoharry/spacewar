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
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include <X11/Xutil.h>
#include <string.h>
#include "image.h"
#include "display.h"


static double _display_exponent;
static unsigned char _bg_red, _bg_green, _bg_blue;

// Only for debugging
void _print_png_version(void){
  return;
}

// An initialization function
int _readpng_init(FILE *infile, long *pWidth, long *pHeight){
  unsigned char sig[8]; // The first 8 bytes with magic number
  
  if(fread(sig, 1, 8, infile) <= 0)
    return 1; // Not a PNG file!
  if (png_sig_cmp(sig, 0, 8))
    return 1; // It's not a PNG file!

  // Let's set the PNG structs
  _png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!_png_ptr)
    return 4;   /* out of memory */
  _info_ptr = png_create_info_struct(_png_ptr);
  if (!_info_ptr) {
    //free(_png_ptr);
    png_destroy_read_struct(&_png_ptr, NULL, NULL);
    return 4;   /* out of memory */
  }

  if (setjmp(png_jmpbuf(_png_ptr))) {
    png_destroy_read_struct(&_png_ptr, &_info_ptr, NULL);
    return 2;
  }

  png_init_io(_png_ptr, infile);
  png_set_sig_bytes(_png_ptr, 8);
  png_read_info(_png_ptr, _info_ptr);

  png_get_IHDR(_png_ptr, _info_ptr, (png_uint_32 *) &_width, (png_uint_32 *) &_height, &_bit_depth,
               &_color_type, NULL, NULL, NULL);
  *pWidth = _width;
  *pHeight = _height;

  
  //free((char *) _png_ptr);
  return 0;
}

// Getting background
int _readpng_get_bgcolor(unsigned char *red, unsigned char *green, unsigned char *blue){
  png_color_16p pBackground;

  if (setjmp(png_jmpbuf(_png_ptr))) {
    png_destroy_read_struct(&_png_ptr, &_info_ptr, NULL);
    return 2;
  }  
  
  if (!png_get_valid(_png_ptr, _info_ptr, PNG_INFO_bKGD))
    return 1;
  
  png_get_bKGD(_png_ptr, _info_ptr, &pBackground);

  if (_bit_depth == 16) {
    *red   = pBackground->red   >> 8;
    *green = pBackground->green >> 8;
    *blue  = pBackground->blue  >> 8;
  } else if (_color_type == PNG_COLOR_TYPE_GRAY && _bit_depth < 8) {
    if (_bit_depth == 1)
      *red = *green = *blue = pBackground->gray? 255 : 0;
    else if (_bit_depth == 2)   /* i.e., max value is 3 */
      *red = *green = *blue = (255/3) * pBackground->gray;
    else /* _bit_depth == 4 */  /* i.e., max value is 15 */
      *red = *green = *blue = (255/15) * pBackground->gray;
  } else {
    *red   = pBackground->red;
    *green = pBackground->green;
    *blue  = pBackground->blue;
  }

  return 0;
}

// Getting image
unsigned char *_readpng_get_image(double display_exponent, int *pChannels, unsigned long *pRowbytes){
  double  gamma;
  png_uint_32  i, rowbytes;
  png_bytepp  row_pointers = NULL;
  //unsigned char *image_data;
  
  if (setjmp(png_jmpbuf(_png_ptr))) {
    png_destroy_read_struct(&_png_ptr, &_info_ptr, NULL);
    return NULL;
  }

  // Expanding pallete to RGB[A]
  if (_color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_expand(_png_ptr);
  if (_color_type == PNG_COLOR_TYPE_GRAY && _bit_depth < 8)
    png_set_expand(_png_ptr);
  if (png_get_valid(_png_ptr, _info_ptr, PNG_INFO_tRNS))
    png_set_expand(_png_ptr);

  if (_bit_depth == 16)
    png_set_strip_16(_png_ptr);
  if (_color_type == PNG_COLOR_TYPE_GRAY ||
      _color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(_png_ptr);

  // Gamma correction
  if (png_get_gAMA(_png_ptr, _info_ptr, &gamma))
    png_set_gamma(_png_ptr, display_exponent, gamma);
  
  // Updating information in structs
  png_read_update_info(_png_ptr, _info_ptr);

  *pRowbytes = rowbytes = png_get_rowbytes(_png_ptr, _info_ptr);
  *pChannels = (int)png_get_channels(_png_ptr, _info_ptr);
  
  if ((_image_data = (unsigned char *)malloc(rowbytes*_height)) == NULL) {
    png_destroy_read_struct(&_png_ptr, &_info_ptr, NULL);
    return NULL;
  }
  
  if ((row_pointers = (png_bytepp)malloc(_height*sizeof(png_bytep))) == NULL) {
    png_destroy_read_struct(&_png_ptr, &_info_ptr, NULL);
    free(_image_data);
    _image_data = NULL;
    return NULL;
  }


  for (i = 0;  i < _height;  ++i)
    row_pointers[i] = _image_data + i*rowbytes;

  
  png_read_image(_png_ptr, row_pointers);
  
  png_read_end(_png_ptr, NULL);
  free(row_pointers);

  return _image_data;
}

struct surface *new_image(char *file){
  int rc;
  double default_display_exponent;   /* whole display system */
  FILE *infile;
  char *path = (char *) malloc(strlen(file)+60);
  path[0] = '\0';

  //unsigned char *image_data;
  struct surface *my_surf;

  _display_exponent = default_display_exponent = 2.2;
  
  strcat(path, "/usr/share/games/spacewar/images/");
  strcat(path, file);
  if(!(infile = fopen(path, "rb"))){
    path[0] = '\0';
    strcat(path, "images/");
    strcat(path, file);
    if(!(infile = fopen(path, "rb")))
      return NULL;
  }

  if ((rc = _readpng_init(infile, (long *) &_image_width, (long *) &_image_height)) != 0)
    return NULL;

  if (_readpng_get_bgcolor(&_bg_red, &_bg_green, &_bg_blue) > 1){
    return NULL;
  }
  _image_data = _readpng_get_image(_display_exponent, &_image_channels,
                                 &_image_rowbytes);
  //fclose(infile);
  //my_image = XCreateImage(_dpy, _visual, _depth, ZPixmap, 0, _image_data, _image_width, _image_height, 32, 0);  
  my_surf = new_surface(_image_width, _image_height);
  _display_image(my_surf, 0, 0);
  //XPutImage(_dpy, my_surf -> pix, _gc, my_image, 0, 0, 0, 0, _image_width, _image_height);
  //XDestroyImage(my_image);
  fclose(infile);
  png_destroy_read_struct(&_png_ptr, &_info_ptr, NULL);
  free(_image_data);
  
  return my_surf;
}

int _display_image(struct surface *my_surf, int x, int y){
  unsigned char *src;
  char *dest;
  unsigned char r, g, b, a;
  unsigned long i, row, lastrow = 0;
  unsigned long pixel;
  unsigned char *xdata = NULL;
  unsigned char *maskdata;
  int pad;
  int RShift, GShift, BShift;
  unsigned long RMask, GMask, BMask;
  XImage *_ximage, *mask;

  RMask = _visual->red_mask;
  GMask = _visual->green_mask;
  BMask = _visual->blue_mask;

  maskdata = (unsigned char *) malloc(_image_width * _image_height);
  if(_depth == 24 || _depth == 32){
    xdata = (unsigned char *) malloc(4*_image_width*_image_height);
    pad = 32;
    RShift = 7 - _rpng_x_msb(RMask);     // these are right-shifts, too 
    GShift = 7 - _rpng_x_msb(GMask);
    BShift = 7 - _rpng_x_msb(BMask);
  }
  else if(_depth == 16){
    xdata = (unsigned char *)malloc(2*_image_width*_image_height);
    pad = 16;
    RShift = 15 - _rpng_x_msb(RMask);    // these are right-shifts
    GShift = 15 - _rpng_x_msb(GMask);
    BShift = 15 - _rpng_x_msb(BMask);
  }
  else { // Unknown depth.
    xdata = (unsigned char *)malloc(_image_width*_image_height);
    pad = 8;
    RShift = GShift = BShift = 0;
    fprintf(stderr, "ERROR: Not supported depth... :-(\n");
    exit(1);
  }
  if(xdata == NULL){
    fprintf(stderr, "ERROR: I can't allocate image memory.\n");
    exit(1);
  }
  _ximage = XCreateImage(_dpy, _visual, _depth, ZPixmap, 0, (char *) xdata, _image_width, _image_height, pad, 0);
  if(!_ximage){
    fprintf(stderr, "ERROR: XCreateImage() failed!\n");
    exit(1);
  }
  XInitImage(_ximage);
  _ximage -> byte_order =  MSBFirst; 
  
  mask = XCreateImage(_dpy, _visual, 1, XYBitmap, 0, (char *) maskdata, _image_width, _image_height, pad, 0);
  XInitImage(mask);
  if(!mask){
    fprintf(stderr, "ERROR: XCreateImage() failed while creating a bitmap!\n");
    exit(1);
  }
  mask -> byte_order = MSBFirst;
  mask -> bitmap_bit_order = MSBFirst;
  mask -> bits_per_pixel = 1;

  // Beginning the loop drawing...
  if(_depth == 24 || _depth == 32){
    unsigned long red, green, blue;
    
    //LOOP!
    for(lastrow = row = 0; row < _image_height; ++row){
      src = _image_data + row * _image_rowbytes;
      dest = _ximage -> data + row * _ximage -> bytes_per_line;
      if(_image_channels == 3){
        for(i = _image_width; i > 0; --i){
          red = *src ++;
          green = *src ++;
          blue = *src ++;
          red   = (RShift < 0)? red   << (-RShift) : red   >> RShift;
          green = (GShift < 0)? green << (-GShift) : green >> GShift;
          blue  = (BShift < 0)? blue  << (-BShift) : blue  >> BShift;
          pixel = (red & RMask) | (green & GMask) | (blue & BMask);
          *dest++ = (char)((pixel >> 24) & 0xff);
          *dest++ = (char)((pixel >> 16) & 0xff);
          *dest++ = (char)((pixel >>  8) & 0xff);
          *dest++ = (char)( pixel        & 0xff);
        } // End of for
      }
      else{ // image channels == 4
        for(i = _image_width; i > 0; --i){
          r = *src++;
          g = *src++;
          b = *src++;
          a = *src++;
          if(a == 255){
            red   = r;
            green = g;
            blue  = b;
          }
          else if(a == 0){
            red   = _bg_red;
            green = _bg_green;
            blue  = _bg_blue;
          }
          else{
            _alpha_composite(red,   r, a, _bg_red);
            _alpha_composite(green, g, a, _bg_green);
            _alpha_composite(blue,  b, a, _bg_blue);
          }
          
          //printf("DEBUG: %d %d %d\n", RShift, GShift, BShift);
          pixel = (red   << -RShift) |
            (green << -GShift) |
            (blue  << -BShift);
          //printf("%d ", pixel);
          // recall that we set ximage->byte_order = MSBFirst above
          *dest++ = (char)((pixel >> 24) & 0xff);
          *dest++ = (char)((pixel >> 16) & 0xff);
          *dest++ = (char)((pixel >>  8) & 0xff);
          *dest++ = (char)( pixel        & 0xff);
        } // End of for
      }// End of else if channels == 4
      //if(((row + 1) & 0xf) ==0){
      //XPutImage(_dpy, _w, _gc, _ximage, 0, (int) lastrow, x, (int) lastrow + y, _image_width, 16);
        //flush();
        //lastrow = row + 1;
      //}
    }// End of for
  } // End of depth == 24 || 32
  if (lastrow < _image_height) {
    int i, j;
    GC bit_gc;
    unsigned long pixel;
    XPutImage(_dpy, my_surf -> pix, _gc, _ximage, 0, 0, 0,
              0,  _image_width, _image_height);
    for(i = 0; i < _image_width; i ++)
      for(j = 0; j < _image_height; j ++){
	pixel = XGetPixel(_ximage, i, j);
	if(pixel == transparent_color)
	  XPutPixel(mask, i, j, 1);
	else
	  XPutPixel(mask, i, j, 0);
      }
    
    bit_gc = XCreateGC(_dpy, my_surf -> mask, 0, NULL);
    XPutImage(_dpy, my_surf -> mask, bit_gc, mask, 0, 0, 0, 0, _image_width, _image_height);
    XFreeGC(_dpy, bit_gc); 
    XDestroyImage(_ximage);
    XDestroyImage(mask);
    //flush();
  }
  return 0;
}


int _rpng_x_msb(unsigned long u32val){
    int i;
    for (i = 31;  i >= 0;  --i) {
        if (u32val & 0x80000000L)
            break;
        u32val <<= 1;
    }
    return i;
}
