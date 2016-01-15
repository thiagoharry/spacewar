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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "display.h"

// Flushes the buffer in the screen
void flush(void){
  XdbeSwapInfo info;
  info.swap_window = _w;           // Window for which to swap buffers
  info.swap_action = XdbeCopied;   // Action to be performed
  XdbeSwapBuffers(_dpy, &info, 1); // Swap 1 window
  XFlush(_dpy);                    // Flushes the output buffer
}

// This function adds mask bits in a surface
void draw_rectangle_mask(struct surface *my_surf, int x, int y, int width, 
			 int height){
  XSetForeground(_dpy, _mask_gc, 0l);
  XFillRectangle(_dpy, my_surf -> mask, _mask_gc, x, y, width, height);
}

// This function fills a surface with some color
void fill_surface(struct surface *surf, unsigned color){
  XSetForeground(_dpy, _gc, color);
  XFillRectangle(_dpy, surf -> pix, _gc, 0, 0, surf -> width, surf -> height);
  XFlush(_dpy);
}

// This function is used to create surfaces
struct surface *new_surface(int width, int height){
  struct surface *my_surf = (struct surface *) malloc(sizeof(struct surface));
  if(my_surf != NULL){
    my_surf -> pix = XCreatePixmap(_dpy, _w, width, height, _depth);
    my_surf -> width = width;
    my_surf -> height = height;
    my_surf -> mask = XCreatePixmap(_dpy, _w, width, height, (unsigned long) 1);
    if(_mask_gc == None)
        _mask_gc = XCreateGC(_dpy, my_surf -> mask, 0, NULL);
    XSetForeground(_dpy, _mask_gc, ~0l);
    XFillRectangle(_dpy, my_surf -> mask, _mask_gc, 0, 0, my_surf -> width, my_surf -> height);
      
  }
  return my_surf;
}

// And this destroys a surface
void destroy_surface(struct surface *my_surf){
  XFreePixmap(_dpy, my_surf -> pix);
  XFreePixmap(_dpy, my_surf -> mask);
  free(my_surf);
}


// This is used to bit blit 2 surfaces
void blit_surface(struct surface *src, struct surface *dest, int x_src, int y_src, 
		  int width, int height, int x_dest, int y_dest){
  //XGCValues gcValues; 
  
  //XGetGCValues(_dpy, _gc, GCFunction|GCForeground|GCBackground|GCPlaneMask, &gcValues);
  //XSetBackground(_dpy, _gc, BLACK);
  if(src -> mask != None){
    XSetClipMask(_dpy, _gc, src -> mask);
    XSetClipOrigin(_dpy, _gc, x_dest - x_src, y_dest - y_src);
  }
  
  XCopyArea(_dpy, src -> pix, dest -> pix, _gc, x_src, y_src, width, height, x_dest, y_dest);
  
  XSetClipMask(_dpy, _gc, None);
  //XChangeGC(_dpy, _gc, GCFunction|GCForeground|GCBackground|GCPlaneMask, &gcValues);
  XFlush(_dpy);
}

// This blit a pixmap using a mask passed as argument
void blit_masked_pixmap(Pixmap pix, Pixmap mask, struct surface *dest, int x_src, 
			int y_src, int width, int height, int x_mask, 
			int y_mask, int x_dest, int y_dest){
  XSetClipMask(_dpy, _gc, mask);
  XSetClipOrigin(_dpy, _gc, x_dest - x_mask, y_dest - y_mask);
  XCopyArea(_dpy, pix, dest -> pix, _gc, x_src, y_src, width, height, x_dest, y_dest);
  XSetClipMask(_dpy, _gc, None);
  XFlush(_dpy);
}


//This blits a surface combined with a mask in a given destiny
void blit_masked_surface(Pixmap pix, Pixmap mask, struct surface *dest, int x_src, int y_src, 
			 int mask_x, int mask_y, int width, int height, int x_dest, int y_dest){
  if(mask != None){
    XSetClipMask(_dpy, _gc, mask);
    XSetClipOrigin(_dpy, _gc, x_dest - x_src - mask_x, y_dest - y_src - mask_y);
  }
  XCopyArea(_dpy, pix, dest -> pix, _gc, x_src, y_src, width, height, x_dest, y_dest);
  XSetClipMask(_dpy, _gc, None);
  XFlush(_dpy);
}

// This function fills a surface with a texture defined in other surface
void apply_texture(struct surface *src, struct surface *dest){
  int x, y;
  if(src -> width < 1 || src -> height < 1)
    return;
  for(x = 0; x < dest -> width; x += src -> width)
    for(y = 0; y < dest -> height; y += src -> height){
      XCopyArea(_dpy, src -> pix, dest -> pix, _gc, 0, 0, src -> width, src -> height, x, y);
    }
  XFlush(_dpy);
}

// This function creates a black fullscreen window where we can play.
void _initialize_screen(void){
  XVisualInfo *visual_list;
  XVisualInfo visual_info; 
  unsigned long valuemask = CWOverrideRedirect;
  XSetWindowAttributes attributes;

  
  // Connecting with the X server...
  _dpy = XOpenDisplay(NULL);          
  if(_dpy == NULL){
    fprintf(stderr, "ERROR: The program couldn't open a "
	    "connection to the X Server.\n");
    exit(1);
  }

  // Getting screen information
  _screen = DefaultScreen(_dpy);
  _depth = DisplayPlanes(_dpy, _screen);

  // Creating a black window...
  _w = XCreateSimpleWindow(_dpy,                     
                           DefaultRootWindow(_dpy), // Parent window
                           0, 0,                    // Coordinates
                           window_width = DisplayWidth(_dpy,       
                                        DefaultScreen(_dpy)),
                           window_height = DisplayHeight(_dpy, 
                                         DefaultScreen(_dpy)),
                           0, NOT_IMPORTANT,  // Border
                           BLACK); // Background
  // Changing setting...
  attributes.override_redirect = True;
  attributes.event_mask = ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | PointerMotionMask;
  attributes.event_mask = 0l;
  XChangeWindowAttributes(_dpy, _w, valuemask, &attributes);
  XSelectInput(_dpy, _w, StructureNotifyMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
	       PointerMotionMask);

  // Mapping the window..
  XMapWindow(_dpy, _w);                             
              

  //  Getting visual info
  if(_depth != 16 && _depth != 24 && _depth != 32){
    int visuals_matched;
    
    visual_info.screen = _screen;
    visual_info.depth = 24;
    visual_list = XGetVisualInfo(_dpy, VisualScreenMask | VisualDepthMask, &visual_info, &visuals_matched);
    if(visuals_matched == 0){
      fprintf(stderr, "Default screen depth not supported.\n");
      exit(1);
    }
    _visual = visual_list[0].visual;
    _depth = visual_list[0].depth;
  }
  else{
    XMatchVisualInfo(_dpy, _screen, _depth, TrueColor, &visual_info);
    _visual = visual_info.visual;
  }


  // Waiting the window being produced...
  {
    XEvent e;                                            
    XNextEvent(_dpy, &e);
    while(e.type != MapNotify){
      XNextEvent(_dpy, &e);
    }
  }

  XSetInputFocus(_dpy, _w, RevertToParent, CurrentTime);

  // Creating the back buffer
  _b = XdbeAllocateBackBufferName(_dpy, _w, XdbeUndefined);


  // Creating our virtual window.
  // TODO: Make it portable with other depths.
  {
    background = NULL;
    background = new_surface(window_width, window_height);
    window = (struct surface *) malloc(sizeof(struct surface));
    if(window != NULL && background != NULL){
      window -> pix = _b;
      window -> width = window_width;
      window -> height = window_height;
      window -> mask = None;
    }
    else{
      fprintf(stderr, "ERROR: I wasn't able to alloc memory for the window image.\n");
      exit(1);
    }
  }
  
  
                                    
  // Creates a Graphic Context...
  {
   
    _gc = XCreateGC(_dpy, _w, 0, NULL);
    _mask_gc = None;
  }

  fill_surface(background, BLACK);
  transparent_color = 0x00029a;
   //flush();                                        
}

// A function to hide the mouse pointer
// XXX: Is there a better way?
void hide_cursor(void){
  Colormap cmap;
  Cursor no_ptr;
  XColor black, dummy;
  static char bm_no_data[] = {0, 0, 0, 0, 0, 0, 0, 0};
  Pixmap bm_no;

  cmap = DefaultColormap(_dpy, DefaultScreen(_dpy));
  XAllocNamedColor(_dpy, cmap, "black", &black, &dummy);
  bm_no = XCreateBitmapFromData(_dpy, _w, bm_no_data, 8, 8);
  no_ptr = XCreatePixmapCursor(_dpy, bm_no, bm_no, &black, &black, 0, 0);

  XDefineCursor(_dpy, _w, no_ptr);
  XFreeCursor(_dpy, no_ptr);
  if (bm_no != None)
    XFreePixmap(_dpy, bm_no);
  XFreeColors(_dpy, cmap, &black.pixel, 1, 0);
}


// This function draws a point in the screen
void draw_point(unsigned x, unsigned y, unsigned color){
  XSetForeground(_dpy, _gc, color);
  XDrawPoint(_dpy, _b, _gc, x, y);
  XFlush(_dpy);
}

// This function draws a line in the screen
void draw_line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, unsigned color){
  XSetForeground(_dpy, _gc, color);
  XDrawLine(_dpy, _b, _gc, x1, y1, x2, y2);
  XFlush(_dpy);
}

// This function draws a circle
void draw_circle(unsigned x, unsigned y, unsigned r, unsigned color){
  unsigned diameter = r + r;
  XSetForeground(_dpy, _gc, color); 
  XDrawArc(_dpy, _b, _gc, x-r, y-r, diameter, diameter, 0, 23040);
  XFlush(_dpy);
}

// This function fills a circle
void fill_circle(unsigned x, unsigned y, unsigned r, unsigned color){
  unsigned diameter = r + r;
  XSetForeground(_dpy, _gc, color); 
  XDrawArc(_dpy, _b, _gc, x-r, y-r, diameter, diameter, 0, 23040);
  XFillArc(_dpy, _b, _gc, x-r, y-r, diameter, diameter, 0, 23040);
  XFlush(_dpy);
}


// This draws a rectangle
void draw_rectangle(unsigned x, unsigned y, unsigned width, unsigned height, unsigned color){
  XSetForeground(_dpy, _gc, color);
  XDrawRectangle(_dpy, _b, _gc, x, y, width, height);
  XFlush(_dpy);
}

// This fills a rectangle
void fill_rectangle(unsigned x, unsigned y, unsigned width, unsigned height, unsigned color){
  XSetForeground(_dpy, _gc, color);
  XDrawRectangle(_dpy, _b, _gc, x, y, width, height);
  XFillRectangle(_dpy, _b, _gc, x, y, width, height);
  XFlush(_dpy);
}

// And this draws ellipses
void draw_ellipse(unsigned x, unsigned y, unsigned width, unsigned height, unsigned color){
  XSetForeground(_dpy, _gc, color);
  XDrawArc(_dpy, _b, _gc, x - width / 2, y - height / 2, width, height, 0, 23040);
  XFlush(_dpy);
}

// We also can fill an ellipse
void fill_ellipse(unsigned x, unsigned y, unsigned width, unsigned height, unsigned color){
  XSetForeground(_dpy, _gc, color);
  XDrawArc(_dpy, _b, _gc, x - width / 2, y - height / 2, width, height, 0, 23040);
  XFillArc(_dpy, _b, _gc, x - width / 2, y - height / 2, width, height, 0, 23040);
  XFlush(_dpy);
}
