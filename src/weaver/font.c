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

#include "font.h"
#include "display.h"
#include <X11/Xutil.h>
/*
// Initializes the FreeType Library
int _initialize_font(void){
  int error = 0;
  error = FT_Init_FreeType(&_library);
  if(error){
    printf("WARNING: FreeType Library couldn't be initialized.\n");
    return 0;
  }
  _face = NULL;
  return 1;
}

// Loads a font face number 'index' from the 'file' file
int load_font(char *file, int size){
  int error;
  FILE *fp;
  char *path = (char *) malloc(strlen(file)+50);
  path[0] = '\0';

  // Opening file passed as argument
  strcat(path, "/usr/share/games/spider/fonts/");
  strcat(path, file);
  fp = fopen(path, "rb");
  if(!fp){
    path[0] = '\0';
    strcat(path, "fonts/");
    strcat(path, file);
    fp = fopen(path, "rb");
    if(!fp){
      printf("WARNING: The font %s was not found.\n", file);
      free(path);
      return 0;
    }
  }
  fclose(fp);

  // Loading font
  error = FT_New_Face(_library, path, 0, &_face);
  free(path);
  
  // Handling some possible errors
  if(error == FT_Err_Unknown_File_Format){
    printf("WARNING: The font %s has an unsupported format.\n", file);
    _face = NULL;
    return 0;
  }
  if(error){
    printf("WARNING: Error while loading font %s.\n", file);
    _face = NULL;
    return 0;
  }
  
  return 1;
}

int draw_text(unsigned x, unsigned y, int size, char *text, unsigned color){
  int error;
  char *p;
  XImage *ximage = NULL;


  if(_face == NULL){
    printf("WARNING: Trying to write, but no font were selected.\n");
    return 0;
  }

  // Setting font size
  error = FT_Set_Char_Size(_face, 0, size, window_width, window_height);
  if(error){
    printf("WARNING: Error while setting size in font.\n");
    _face = NULL;
    return 0;
  }  



  for(p = text; *p != '\0'; p++){
    surface *surf;

    // retrieve glyph index from character code  
    //glyph_index = FT_Get_Char_Index(_face, *p);
    
    printf("-6\n"); fflush(stdout);
    // load glyph image into the slot (erase previous one)
    error = FT_Load_Char(_face, *p, FT_LOAD_MONOCHROME);
    if(error) // If we can't handle this character, go to the next
      continue;

    printf("-5\n"); fflush(stdout);
    // convert to an anti-aliased bitmap
    error = FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_MONO);
    if(error)
      continue;

    printf("-4\n"); fflush(stdout);

    if(_face->glyph->bitmap.width <= 0 || _face->glyph->bitmap.rows <= 0){
      x += _face -> glyph -> advance.x >> 6;
      continue;
    }

    printf("-3\n"); fflush(stdout);
    // Now we have a charactere in _face->glyph-> bitmap
    ximage = XCreateImage(_dpy, _visual, 1, XYBitmap, 0, (char *) _face->glyph-> bitmap.buffer,
			  _face->glyph->bitmap.width, _face->glyph->bitmap.rows, 32, 0);
    if(!ximage){
      fprintf(stderr, "ERROR: XCreateImage() failed!\n");
      exit(1);
    }

    printf("-2\n"); fflush(stdout);

   

    printf("-1\n"); fflush(stdout);

    XInitImage(ximage);
    ximage -> byte_order =  MSBFirst;
    ximage -> bitmap_bit_order = MSBFirst;
    ximage -> bits_per_pixel = 1;

    printf("0\n"); fflush(stdout);
    surf = new_surface(_face->glyph->bitmap.width*2, _face->glyph->bitmap.rows*2);
    printf("1\n"); fflush(stdout);
    XSetForeground(_dpy, _gc, color);
    printf("2\n"); fflush(stdout);
    XFillRectangle(_dpy, surf -> pix, _gc, 0, 0, surf -> width, surf -> height);
    printf("3\n"); fflush(stdout);
    XPutImage(_dpy, surf -> mask, _mask_gc, ximage, 0, 0, 0, 0, 
          _face->glyph->bitmap.width*2, _face->glyph->bitmap.rows*2);
    printf("4\n"); fflush(stdout);
    blit_surface(surf, window, 0, 0, surf -> width, surf -> height, x, y);
    printf("5\n"); fflush(stdout);
    x += _face -> glyph -> advance.x >> 6;
    printf("6\n"); fflush(stdout);
    printf("7\n"); fflush(stdout);
    destroy_surface(surf);
    printf("8\n"); fflush(stdout);
  }
  XDestroyImage(ximage);
  return 0;
}
*/
