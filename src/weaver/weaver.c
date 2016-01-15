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
#include <math.h>
#include <sys/time.h>
#include <float.h>
#include <signal.h>
#include "weaver.h"
#include "display.h"

// This function initializes everything
void awake_the_weaver(void){
  _initialize_screen();
  _initialize_keyboard();
  _initialize_mouse();
  srand(time(NULL));
  gettimeofday(&current_time, NULL);
  fps = 50;
  _music = 0;
  _sound = 0;
}

void may_the_weaver_sleep(void){
  stop_music();
  if(_sound)
    kill(_sound, 9);
  XFreeGC(_dpy, _gc);
  free(window);
  destroy_surface(background);
  XdbeDeallocateBackBufferName(_dpy, _b);  
  XCloseDisplay(_dpy);

  // It's causing a segmentation fault in some systems...
  // Why?
  //png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
}

// This function pauses the program for the number of nanoseconds
// passed as argumment
void weaver_rest(long nanoseconds){
  XdbeSwapInfo info;
  info.swap_window = _w;
  info.swap_action = XdbeCopied;
  struct timespec req = {0, nanoseconds};
  XFlush(_dpy);
  nanosleep(&req, NULL);
  _b_frame.tv_sec = current_time.tv_sec;
  _b_frame.tv_usec = current_time.tv_usec;
  gettimeofday(&current_time, NULL);
  fps = 1000000 / (1000000 * (current_time.tv_sec - _b_frame.tv_sec) + \
		   current_time.tv_usec - _b_frame.tv_usec);
  if(fps == 0) fps = 1;
  XdbeSwapBuffers(_dpy, &info, 1);
  XFlush(_dpy);
}


// This centers a camera represented by a struct vector4
void center_camera(struct vector4 *camera, float x, float y){
  camera -> x = x - (camera -> w / 2);
  camera -> y = y - (camera -> z / 2);
}

// This functions zooms in and out wihout change the central point (focus)
void zoom_camera(struct vector4 *cam, float zoom){
  float x, y;
  if(zoom <= 0.0) return;
  // Storing the central point
  x = cam -> x + (cam -> w / 2);
  y = cam -> y + (cam -> z / 2);
  // Zooming
  cam -> w /= zoom;
  cam -> z /= zoom;
  // Restoring the central point
  cam -> x = x - (cam -> w / 2);
  cam -> y = y - (cam -> z / 2);
}

// This function limits the area where a camera can draw things in the screen
void limit_camera(struct vector4 *camera, int x, int y, int width, int height){
  camera -> previous = (struct vector4 *) (long) x;
  camera -> top = (struct vector4 *) (long) y;
  camera -> next = (struct vector4 *) (long) width;
  camera -> down = (struct vector4 *) (long) height;
}

// This erases a rectangle from the screen
void _erase_rectangle(struct vector4 *camera, struct vector4 *rectangle,
		      int full){
   int x, y, height, width, limited_camera = 0;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
               (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
                   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
               (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
                    (float) window_height);
  }
  if(limited_camera){
    if(rectangle -> x + rectangle -> w > camera -> x &&
       rectangle -> x < camera -> x + camera -> w &&
       rectangle -> y + rectangle -> z > camera -> y &&
       rectangle -> y < camera -> y + camera -> z){
      int surf_x, surf_y, surf_w, surf_h;
      surf_x =  (int) (long) camera -> previous;
      surf_y = (int) (long) camera -> top;
      surf_w = (int) (long) camera -> next;
      surf_h = (int) (long) camera -> down;
      if(x > 0){
	if(x > surf_w)
	  return; // Outside range
	surf_x += x;
	surf_w -= x;
	x = 0;
      }
      if(y > 0){
	if(y > surf_h)
	  return; // Outside range
	surf_y += y;
	surf_h -= y;
	y = 0;
      }
      if(x + width < surf_w){
	surf_w = x + width;
	surf_w ++;
      }
      if(y + height < surf_h){
	surf_h = y + height;
	surf_h ++;
      }
      // If we are here, the rectangle appears in the camera
      // Creating a temporary surface
      struct surface *surf = new_surface(surf_w, surf_h);
      blit_surface(background, surf, surf_x, surf_y, 
		   surf_w, surf_h, 0, 0);
      draw_rectangle_mask(surf, 0, 0, surf_w, surf_h);
      // Drawing the rectangle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      if(full)
	XFillRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   surf_x, surf_y);
      destroy_surface(surf);
    }
  }
  else{
    struct surface *surf = new_surface(width + 1, height + 1);
    draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
    blit_surface(background, surf, x, y, width + 1, height + 1, 0, 0);
    XSetForeground(_dpy, _mask_gc, ~0l);
    XDrawRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, width, height);
    if(full)
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, width, height);
    draw_surface(surf, window, x, y);
    destroy_surface(surf);
  }
}

// This shows a rectangle (or square) in the area covered by the camera
void film_rectangle(struct vector4 *camera, struct vector4 *rectangle, 
		    unsigned color){
  int x, y, height, width, limited_camera = 0;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);

  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
               (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
                   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
               (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
                    (float) window_height);
  }
  if(limited_camera){
    if(rectangle -> x + rectangle -> w > camera -> x &&
       rectangle -> x < camera -> x + camera -> w &&
       rectangle -> y + rectangle -> z > camera -> y &&
       rectangle -> y < camera -> y + camera -> z){
      // If we are here, the rectangle appears in the camera
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
                                         (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
                     surf -> height);
      
      // Drawing the rectangle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawRectangle(_dpy, surf -> pix, _gc, x, y, width, height);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    draw_rectangle(x, y, width, height, color);
}

void film_fullrectangle(struct vector4 *camera, struct vector4 *rectangle, 
			unsigned color){
  int x, y, height, width, limited_camera = 0;
  x = (int) (((rectangle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((rectangle -> y - camera -> y) / camera -> z) * window_height);
  height = (int) ((rectangle -> z / camera -> z) * window_height);
  width = (int) ((rectangle -> w / camera -> w) * window_width);

  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
               (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
                   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
               (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
                    (float) window_height);
  }

  if(limited_camera){
    if(rectangle -> x + rectangle -> w > camera -> x &&
       rectangle -> x < camera -> x + camera -> w &&
       rectangle -> y + rectangle -> z > camera -> y &&
       rectangle -> y < camera -> y + camera -> z){
      // If we are here, the rectangle appears in the camera
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
                                         (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
                     surf -> height);
      
      // Drawing the rectangle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawRectangle(_dpy, surf -> pix, _gc, x, y, width, height);
      XFillRectangle(_dpy, surf -> pix, _gc, x, y, width, height);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, x, y, width, height);

      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    fill_rectangle(x, y,width, height, color);
}

// Erase a circle from the screen
void erase_circle(struct vector4 *camera, struct vector3 *circle){
  int x, y, height, width, limited_camera;
  limited_camera = 0;
  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      int surf_x, surf_y, surf_w, surf_h;
      surf_x =  (int) (long) camera -> previous;
      surf_y = (int) (long) camera -> top;
      surf_w = (int) (long) camera -> next;
      surf_h = (int) (long) camera -> down;
      if(x - width /2 > 0){ 
	if(x - width /2 > surf_w){
	  return; // Circle outside camera range
	}
	else{
	  surf_x += x - width /2;
	  surf_w -= x - width /2;
	  x -= surf_x - (int) (long) camera -> previous;
	  
	}
      }
      
      if(y - height /2 > 0){
	if(y - height /2 > surf_h){
	  return; // Circle outside camera range
	}
	else{
	  surf_y += y - height /2;
	  surf_h -= y - height /2;
	  y -= surf_y - (int) (long) camera -> top;
	
	}
      }	
      x -= width / 2;
      y -= height / 2;
      if(width < surf_w)
	surf_w = width + 1;
      if(height  < surf_h)
	surf_h = height + 1;
      
      if(surf_w <= 0 || surf_h <= 0)
	return;

      // Creating a surface with part of the background
      surface *surf = new_surface(surf_w, surf_h);
      draw_rectangle_mask(surf, 0, 0, surf_w, surf_h);
      blit_surface(background, surf, surf_x,
		   surf_y, surf_w, surf_h,
		   0, 0);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x, 
	       y, 
	       width, height, 0, 23040);
      // Blitting the surface in the screen
       blit_surface(surf, window, 0, 0,
		   surf_w, surf_h,
		   surf_x, 
		   surf_y);
      destroy_surface(surf);
    }
  }
  else{
    surface *surf = new_surface(width + 1, height + 1);
    draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
    blit_surface(background, surf, x - width / 2,
		   y - height / 2, surf -> width, surf -> height,
		   0, 0);
    // Drawing the circle in the transparency map
    XSetForeground(_dpy, _mask_gc, ~0l);
    XDrawArc(_dpy, surf -> mask, _mask_gc, 0, 0, 
	     width, height, 0, 23040);
    // Blitting the surface in the screen
    blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		 x - width / 2, y - height / 2);
    destroy_surface(surf);
  }  
}

// Erase a full circle from the screen
void erase_fullcircle(struct vector4 *camera, struct vector3 *circle){
  int x, y, height, width, limited_camera;
  limited_camera = 0;
  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      int surf_x, surf_y, surf_w, surf_h;
      surf_x =  (int) (long) camera -> previous;
      surf_y = (int) (long) camera -> top;
      surf_w = (int) (long) camera -> next;
      surf_h = (int) (long) camera -> down;
      if(x - width /2 > 0){ 
	if(x - width /2 > surf_w){
	  return; // Circle outside camera range
	}
	else{
	  surf_x += x - width /2;
	  surf_w -= x - width /2;
	  x -= surf_x - (int) (long) camera -> previous;
	  
	}
      }
      
      if(y - height /2 > 0){
	if(y - height /2 > surf_h){
	  return; // Circle outside camera range
	}
	else{
	  surf_y += y - height /2;
	  surf_h -= y - height /2;
	  y -= surf_y - (int) (long) camera -> top;
	
	}
      }	
      x -= width / 2;
      y -= height / 2;
      if(width < surf_w)
	surf_w = width + 1;
      if(height  < surf_h)
	surf_h = height + 1;
      
      if(surf_w <= 0 || surf_h <= 0)
	return;

      // Creating a surface with part of the background
      surface *surf = new_surface(surf_w, surf_h);
      draw_rectangle_mask(surf, 0, 0, surf_w, surf_h);
      blit_surface(background, surf, surf_x,
		   surf_y, surf_w, surf_h,
		   0, 0);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x, 
	       y, 
	       width, height, 0, 23040);
      XFillArc(_dpy, surf -> mask, _mask_gc, x, 
	       y, 
	       width, height, 0, 23040);
      // Blitting the surface in the screen
       blit_surface(surf, window, 0, 0,
		   surf_w, surf_h,
		   surf_x, 
		   surf_y);
      destroy_surface(surf);
    }
  }
  else{
    surface *surf = new_surface(width + 1, height + 1);
    draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
    blit_surface(background, surf, x - width / 2,
		   y - height / 2, surf -> width, surf -> height,
		   0, 0);
    // Drawing the circle in the transparency map
    XSetForeground(_dpy, _mask_gc, ~0l);
    XDrawArc(_dpy, surf -> mask, _mask_gc, 0, 0, 
	     width, height, 0, 23040);
    XFillArc(_dpy, surf -> mask, _mask_gc, 0, 0, 
	     width, height, 0, 23040);
    // Blitting the surface in the screen
    blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		 x - width / 2, y - height / 2);
    destroy_surface(surf);
  }  
}

// This shows a circle in the screen if it's in the area covered by the camera
void film_circle(struct vector4 *camera, struct vector3 *circle, 
		 unsigned color){
  int x, y, height, width, limited_camera;
  limited_camera = 0;

  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
					 (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
		     surf -> height);
      
      // Drawing the circle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawArc(_dpy, surf -> pix, _gc, x - width / 2, y - height / 2, width, 
	       height, 0, 23040);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x - width / 2, y - height / 2, 
	       width, height, 0, 23040);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    draw_ellipse(x, y, width, height, color);
}

// This draws a full circle in the screen if it's in the area covered by the 
// camera
void film_fullcircle(struct vector4 *camera, struct vector3 *circle, 
		     unsigned color){
  int x, y, height, width, limited_camera;
  int inside = 0;
  limited_camera = 0;
  
  // If the circle is outside the camera, return
  if(circle -> x + circle -> z > camera -> x && 
     circle -> x - circle -> z < camera -> x + camera -> w &&
     circle -> y + circle -> z > camera -> y &&
     circle -> y - circle -> z < camera -> y + camera -> z)
    inside = 1;
  if(!inside)
    return;
  inside = 0;

  x = (int) (((circle -> x - camera -> x) / camera -> w) * window_width);
  y = (int) (((circle -> y - camera -> y) / camera -> z) * window_height);
  height = 2 * (int) ((circle -> z / camera -> z) * window_height);
  width = 2 * (int) ((circle -> z / camera -> w) * window_width);
  if(camera -> previous != NULL || camera -> next != NULL){
    limited_camera ++;
    x = (int) ((float) x * ((float) (long) camera -> next) / 
	       (float) window_width);
    width = (int) ((float) width * ((float) (long) camera -> next) / 
		   (float) window_width);
  }
  if(camera -> top != NULL || camera -> down != NULL){
    limited_camera ++;
    y = (int) ((float) y * ((float) (long) camera -> down) / 
	       (float) window_height);
    height = (int) ((float) height * ((float) (long) camera -> down) / 
		    (float) window_height);
  }
  if(limited_camera){
    // If the circle is entirely inside the camera, we don't need to use 
    // aux surfaces
    // FIXME: There's some error in the calculation here
    /*if(circle -> x > camera -> x + circle -> z &&
       circle -> x < camera -> x + camera -> w - circle -> z &&
       circle -> y > camera -> y + circle -> z &&
       circle -> y < camera -> y + camera -> z - circle -> z){
      fill_ellipse((long) camera -> previous + x - width / 2, 
		   (long) camera -> top + y - height / 2, width, height, color);
      return;
    }*/
    if(circle -> x < (long) camera -> x + (long) camera -> w + 
       (long) (2 * circle -> z) && 
       circle -> x > (long) camera -> x - (long) (2 * circle -> z) &&
       circle -> y < (long) camera -> y + (long) camera -> z + 
       (long) (2 * circle -> z) && 
       circle -> y > (long) camera -> y - (long) (2 * circle -> z)){
      // Creating a temporary and transparent surface
      struct surface *surf = new_surface((long) camera -> next, 
					 (long) camera -> down);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
		     surf -> height);
      
      // Drawing the circle in the surface
      XSetForeground(_dpy, _gc, color);
      XDrawArc(_dpy, surf -> pix, _gc, x - width / 2, y - height / 2, width, 
	       height, 0, 23040);
      XFillArc(_dpy, surf -> pix, _gc, x - width / 2, y - height / 2, width, 
	       height, 0, 23040);
      
      // Drawing the circle in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawArc(_dpy, surf -> mask, _mask_gc, x - width / 2, y - height / 2, 
	       width, height, 0, 23040);
      XFillArc(_dpy, surf -> mask, _mask_gc, x - width / 2, y - height / 2, 
	       width, height, 0, 23040);

      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      destroy_surface(surf);
    }
  }
  else
    fill_ellipse(x, y, width, height, color);
}

// This films a full polygon
void _film_fullpolygon(struct vector4 *camera, struct vector2 *polygon, 
		       unsigned color, int erase){
  struct vector2 *current_vertex = polygon;
  XPoint *points;
  int limited_camera = 0, number_of_points = 0;
  float smallest_x = FLT_MAX, smallest_y = FLT_MAX;
  float biggest_x  = FLT_MIN, biggest_y  = FLT_MIN;
  int inside = 0; // The polygon is entirely inside the camera?

  do{
    if(current_vertex -> x > camera -> x && 
       current_vertex -> x < camera -> x + camera -> w &&
       current_vertex -> y > camera -> y &&
       current_vertex -> y < camera -> y + camera -> z){
      inside = 1;
      current_vertex = polygon;
      break;
    }      
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
  if(!inside)
    return;
  inside = 0;


  if(!erase)
    XSetFillRule(_dpy, _gc, WindingRule);
  else
    XSetFillRule(_dpy, _mask_gc, WindingRule);
  
  /* Empty polygon or henagon.*/
  if(polygon == NULL || polygon -> next == polygon)
    return;

  if(camera -> previous != NULL || camera -> next != NULL)
    limited_camera = 1;
  
  // Discovering the number of points, and also
  // the extreme points
  do{
    number_of_points ++;
    if(erase || limited_camera){
      if(current_vertex -> x < smallest_x)
	smallest_x = current_vertex -> x;
      if(current_vertex -> y < smallest_y)
	smallest_y = current_vertex -> y;
      if(current_vertex -> x > biggest_x)
	biggest_x = current_vertex -> x;
      if(current_vertex -> y > biggest_y)
	biggest_y = current_vertex -> y;
    }
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
  if(limited_camera){
    // Check if the polygon is entirely inside the camera
    if(smallest_x > camera -> x && smallest_y > camera -> y &&
       biggest_x < camera -> x + camera -> w &&
       biggest_y < camera -> y + camera -> z)
      inside = 1;
  }
  
  // Allocating space for the XPoints
  points = (XPoint *) malloc(sizeof(XPoint) * number_of_points);
  
  //Getting the points coordinates 
  number_of_points = 0;
  current_vertex = polygon;
  do{
    points[number_of_points].x = (int) (((current_vertex -> x - camera -> x)/
					 camera -> w) * window_width);
    points[number_of_points].y = (int) (((current_vertex -> y - camera -> y)/
					 camera -> z) * window_height);
    if(erase && !limited_camera){  
      points[number_of_points].x -= (int) (((smallest_x - camera -> x)/
		      camera -> w) * window_width);
      points[number_of_points].y -= (int) (((smallest_y - camera -> y)/
		      camera -> z) * window_height);
    }
    number_of_points ++;
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
  // Correcting values if we are under a limited camera
  if(limited_camera){
    /*
      If the polygon in entirely inside the camera, we don't
      need to use auxiliar surfaces.
    */
    if(inside && !erase){      
      int i;
      for(i = 0; i < number_of_points; i ++){
	points[i].x = (int) ((float) points[i].x * 
			     ((float) (long) camera -> next) / 
			     (float) window_width) + 
	  (long) camera -> previous;
	points[i].y = (int) ((float) points[i].y * 
			     ((float) (long) camera -> down) / 
			     (float) window_height) +
	  (long) camera -> top;
      }
      XSetForeground(_dpy, _gc, color);
      XFillPolygon(_dpy, window -> pix, _gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
      return;
    }
    
    int width = (int) ((float) ((int) (((biggest_x - smallest_x)/
					camera -> w) * window_width)) * 
		       ((float) (long) camera -> next) / 
		       (float) window_width) + 1;
    
    int height =  (int) ((float) ((int) (((biggest_y - smallest_y)/
					camera -> z) * window_height)) * 
		       ((float) (long) camera -> down) / 
		       (float) window_height) +1;
    
    int x = (int) ((float) ((int) (((smallest_x - camera -> x)/
					  camera -> w) * window_width)) * 
			 ((float) (long) camera -> next) / 
		   (float) window_width) + (int) (long) camera -> previous;
    int y = (int) ((float) ((int) (((smallest_y - camera -> y)/
				    camera -> z) * window_height)) * 
		   ((float) (long) camera -> down) / 
		   (float) window_height) + (int) (long) camera -> top;
    if(x < (int) (long) camera -> previous){
      width -= (int) (long) camera -> previous - x;
      x = (int) (long) camera -> previous;
    }
    else if(x > (int) (long) camera -> previous + (int) (long) camera -> next)
      x = (int) (long) camera -> previous + (int) (long) camera -> next;
    if(y < (int) (long) camera -> top){
      height -= (int) (long) camera -> top - y;
      y = (int) (long) camera -> top;
    }
    else if(y > (int) (long) camera -> top + (int) (long) camera -> down)
      y = (int) (long) camera -> top + (int) (long) camera -> down;
    if(width > (int) (long) camera -> next)
      width = (int) (long) camera -> next;
    if(height > (int) (long) camera -> down)
      height = (int) (long) camera -> down - 1; 
    if(x + width > (long) camera -> previous + (long) camera -> next)
      width = (long) camera -> previous + (long) camera -> next - x;
    if(y + height > (long) camera -> top + (long) camera -> down)
      height = (long) camera -> top + (long) camera -> down - y;
    
    
    if(erase && (width <= 0 || height <= 0))
      return;
    
    number_of_points = 0;
    current_vertex = polygon;
    do{
      points[number_of_points].x = (int) ((float) points[number_of_points].x * 
                                          ((float) (long) camera -> next) / 
                                          (float) window_width);
      points[number_of_points].y = (int) ((float) points[number_of_points].y * 
                                          ((float) (long) camera -> down) / 
                                          (float) window_height);
      if(erase){
	points[number_of_points].x += (long) camera -> previous - x;
	points[number_of_points].y += (long) camera -> top - y;
      }
      number_of_points ++;
      current_vertex = current_vertex -> next;
    }while(current_vertex != polygon);
    
    
    if(erase){
      struct surface *surf = new_surface(width, height);
      draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
      blit_surface(background, surf, x, y, width, height, 0, 0);
      // Drawing the polygon in the surface
      XSetForeground(_dpy, _mask_gc, ~0l);
      XFillPolygon(_dpy, surf -> mask, _mask_gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
      draw_surface(surf, window, x, y);
      destroy_surface(surf);
    }
    else{
      struct surface *surf = new_surface((long) camera -> next, 
					 (long) camera -> down);
      XSetFillRule(_dpy, _mask_gc, WindingRule);
      XSetForeground(_dpy, _mask_gc, 0l);
      XFillRectangle(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width, 
		     surf -> height);
      
      // Drawing the polygon in the surface
      XSetForeground(_dpy, _gc, color);
      XFillPolygon(_dpy, surf -> pix, _gc, points, number_of_points, Complex, 
		   CoordModeOrigin);
      
      
      // Drawing the polygon in the transparency map
      XSetForeground(_dpy, _mask_gc, ~0l);
      XFillPolygon(_dpy, surf -> mask, _mask_gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
      
      // Blitting the surface in the screen
      blit_surface(surf, window, 0, 0, surf -> width, surf -> height, 
		   (long) camera -> previous, (long) camera -> top);
      
      
      destroy_surface(surf);
    }
  }
  else{ // Camera is not limited
    if(erase){
      int x = (int) (((smallest_x - camera -> x)/
		      camera -> w) * window_width);
      int y = (int) (((smallest_y - camera -> y)/
		      camera -> z) * window_height);
      int width = (int) (((biggest_x - smallest_x)/
		      camera -> w) * window_width) + 1;
      int height = (int) (((biggest_y- smallest_y)/
		      camera -> z) * window_height) + 1;
      surface *surf = new_surface(width, height);
      draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
      blit_surface(background, surf, x, y, width, height, 0, 0);
      XSetForeground(_dpy, _mask_gc, ~0l);
      XFillPolygon(_dpy, surf -> mask, _mask_gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
      draw_surface(surf, window, x, y);      
      destroy_surface(surf);
    }
    else{
      XSetForeground(_dpy, _gc, color);
      XFillPolygon(_dpy, window -> pix, _gc, points, number_of_points, 
		   Complex, CoordModeOrigin);
    }
  }
  free(points);
  
}

// This films an empty polygon
void _film_polygon(struct vector4 *camera, struct vector2 *polygon, 
		   unsigned color, int erase){
  struct vector2 *current_vertex = polygon;
  struct vector2 *next;
  int x1, y1, x2, y2;
  int limited_camera = 0;
  surface *lim_surf = NULL;
  int inside = 0; // The polygon really will need to be drawn?

  if(current_vertex == NULL)
    return;
  
  do{
    if(current_vertex -> x > camera -> x && 
       current_vertex -> x < camera -> x + camera -> w &&
       current_vertex -> y > camera -> y &&
       current_vertex -> y < camera -> y + camera -> z){
      inside = 1;
      break;
    }      
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
  if(!inside)
    return;

  // If the camera is limited, we'll work inside a surface first
  if(camera -> previous != NULL && camera -> next != NULL)
    limited_camera = 1;

  // First we handle the degenerate cases
  // This is a "Empty Polygon". Don't draw anything.
  if(polygon == NULL)
    return;

  // This is a henagon. A polygon with only one vertex
  // They can be used as particles, so it's usefull to draw them
  if(polygon -> next == polygon){

    x1 = (int) (((polygon -> x - camera -> x) / camera -> w) * window_width);
    y1 = (int) (((polygon -> y - camera -> y) / camera -> z) * window_height);

    // If our camera is limited, we need some more calculations
    if(limited_camera){
      x1 = (int) ((float) x1 * (((float) (long) camera -> next) / 
				(float) window_width)) + 
	(long) (camera -> previous);
      y1 = (int) ((float) y1 * (((float) (long) camera -> down) / 
				(float) window_height)) + 
	(long) (camera -> top);
    }
    if((!limited_camera) || (x1 < (long) camera -> previous + 
			     (long) camera -> next && x1 > 
			     (long) camera -> previous &&
			     y1 < (long) camera -> top + 
			     (long) camera -> down && y1 > 
			     (long) camera -> top)){
      if(!erase)
	draw_point(x1, y1, color);
      else
	blit_surface(background, window, x1, y1, 1, 1, x1, y1);
    }

    // We draw the point only if it's inside the camera limits
    return;
  } // End if it's a henagon  

    // Now the usual case. We have a polygon with more than one vertex.
  do{
    next = current_vertex -> next;
    
    x1 = (int) (((current_vertex -> x - camera -> x) / camera -> w) * 
		window_width);
    y1 = (int) (((current_vertex -> y - camera -> y) / camera -> z) * 
		window_height);
    x2 = (int) (((next -> x - camera -> x) / camera -> w) * window_width);
    y2 = (int) (((next -> y - camera -> y) / camera -> z) * window_height);

    // If the camera is limited, we shall first work inside a surface
    // with the right size.
    if(limited_camera){      
      x1 = (int) ((float) x1 * ((float) (long) camera -> next) / 
		  (float) window_width);
      x2 = (int) ((float) x2 * ((float) (long) camera -> next) / 
		  (float) window_width);
      y1 = (int) ((float) y1 * ((float) (long) camera -> down) / 
		  (float) window_height);
      y2 = (int) ((float) y2 * ((float) (long) camera -> down) / 
		  (float) window_height);


      if((x1 < 0 && x2 < 0) || (y1 < 0 && y2 < 0) ||
	 (x1 >  (long) camera -> next && 
	  x2 > (long) camera -> next) ||
	 (y1 >  (long) camera -> down 
	  && y2 > (long) camera -> down)){
	current_vertex = current_vertex -> next;
	continue;
      }
      
      /*
	If the line is completely inside the camera, we should
	be able to draw directly in the screen. It should improve
	performance.
      */
      // FIXME: 
      /*{
	if(!erase && x1 > 0 && x2 > 0 && 
	   x1 < (long) camera -> next && x2 < (long) camera -> next &&
	   y1 > 0 && y2 > 0 &&
	   y1 < (long) camera -> down && y2 < (long) camera -> down){
	  draw_line(x1 + (long) camera -> previous, 
		    y1 + (long) camera -> top, 
		    x2 + (long) camera -> previous, 
		    y2 + (long) camera -> top, color);
	  current_vertex = current_vertex -> next;
	  continue;
	}
	}*/

      if(lim_surf == NULL){
	lim_surf = new_surface((long) camera -> next, 
			       (long) camera -> down);
	blit_surface(background, lim_surf, (long) camera -> previous, 
		     (long) camera -> top,
		     lim_surf -> width, lim_surf -> height, 
		     0, 0);
	draw_rectangle_mask(lim_surf, 0, 0, lim_surf -> width, 
			    lim_surf -> height);
      }
   
      XSetForeground(_dpy, _mask_gc, ~0l);
      XDrawLine(_dpy, lim_surf -> mask, _mask_gc, x1, y1, x2, y2);
      if(!erase){
	XSetForeground(_dpy, _gc, color);
	XDrawLine(_dpy, lim_surf -> pix, _gc, x1, y1, x2, y2);
      }
	 
    }
    else{
      // We don't have a limited camera. Much easier!
      if(!erase){
	draw_line(x1, y1, x2, y2, color);
      }
      else{
	int width = (x1 - x2 >= 0)?(x1 - x2):(x2 - x1);
	int height = (y1 - y2 >= 0)?(y1 - y2):(y2 - y1);
	int x = (x1 >= x2)?(x2):(x1);
	int y = (y1 >= y2)?(y2):(y1);
	if(!width){
	  blit_surface(background, window, x, y, 1, height, x, y);
	  continue;
	}
	if(!height){
	   blit_surface(background, window, x, y, width, 1, x, y);
	  continue;
	  }
	surface *surf = new_surface(width+1, height+1);
	draw_rectangle_mask(surf, 0, 0, surf -> width, surf -> height);
	blit_surface(background, surf, x, y,
		     surf -> width, surf -> height, 
		     0, 0);
	// Drawing the edge in the surface
	if((x1 <= x2 && y1 <= y2) || ((x1 >= x2 && y1 >= y2))){ 
	  XSetForeground(_dpy, _mask_gc, ~0l);
	  XDrawLine(_dpy, surf -> mask, _mask_gc, 0, 0, surf -> width-1, 
		    surf-> height-1);
	 
	}
	else{
	  XSetForeground(_dpy, _mask_gc, ~0l);
	  XDrawLine(_dpy, surf -> mask, _mask_gc, surf -> width, 0, 0, 
		    surf-> height); 
	}

	draw_surface(surf, window, x, y);
	
	destroy_surface(surf);
      }
    }
    current_vertex = current_vertex -> next;
  }while(current_vertex != polygon);
  // Destroying the surface for limited cameras
  if(limited_camera && lim_surf != NULL){
    draw_surface(lim_surf, window, (long) camera -> previous, 
		 (long) camera -> top);
    destroy_surface(lim_surf);
  }
}

// This detects collision between two rectangles
int collision_rectangle_rectangle(struct vector4 *r1, struct vector4 *r2){
  float dx = (r2 -> x - r1 -> x < 0) ? (r1 -> x - r2 -> x) : 
    (r2 -> x - r1 -> x);
  if((r1 -> x < r2 -> x) ? (dx <= r1 -> w) : (dx <= r2 -> w)){
    float dy = (r2 -> y - r1 -> y < 0) ? (r1 -> y - r2 -> y) : 
      (r2 -> y - r1 -> y);
    if((r1 -> y < r2 -> y) ? (dy <= r1 -> z) : (dy <= r2 -> z))
      return 1;
  }
  return 0;
}

// This detects collision between a rectangle and a circle
int collision_rectangle_circle(struct vector4 *rectangle, 
			       struct vector3 *circle){
  // They collide in the X-axis?
  if(!((circle -> x + circle -> z < rectangle -> x && circle -> x - 
	circle -> z < rectangle -> x &&
     circle -> x + circle -> z < rectangle -> x + rectangle -> w &&
      circle -> x - circle -> z < rectangle -> x + rectangle -> w) ||
     (circle -> x + circle -> z > rectangle -> x && circle -> x - 
      circle -> z > rectangle -> x &&
     circle -> x + circle -> z > rectangle -> x + rectangle -> w &&
      circle -> x - circle -> z > rectangle -> x + rectangle -> w)))
    if(!((circle -> y + circle -> z < rectangle -> y && circle -> y - 
	  circle -> z < rectangle -> y &&
	  circle -> y + circle -> z < rectangle -> y + rectangle -> z &&
	  circle -> y - circle -> z < rectangle -> y + rectangle -> z) ||
	 (circle -> y + circle -> z > rectangle -> y && circle -> y - 
	  circle -> z > rectangle -> y &&
	  circle -> y + circle -> z > rectangle -> y + rectangle -> z &&
	  circle -> y - circle -> z > rectangle -> y + rectangle -> z)))
      return 1;
  

  // By default, if no collision were detected, no collision happened
  return 0;
}



// This function detects collisions between two circles. If they are colliding,
// it returns 1. Else, it returns 0.
int collision_circle_circle(struct vector3 *circle1, struct vector3 *circle2){
  float dx = circle2 -> x - circle1 -> x;
  float dy = circle2 -> y - circle1 -> y;
  float distance = dx * dx + dy * dy;
  float minimum = circle1 -> z + circle2 -> z;
  
  minimum *= minimum;
  if(distance < minimum)
    return 1;
  else
    return 0;
}

// This determines if a rectangle and a polygon are colliding
int collision_rectangle_polygon(struct vector4 *r, struct vector2 *p){
  struct vector2 *current, *next, *first;
  float x1, x2, y1, y2;

  current = first = p;
  next = current -> next;

  if(next == current){ // We have a degenerated polygon with a single vertex
    if(p -> x >= r -> x && p -> x <= r -> x + r -> w)
      if(p -> y >= r -> y && p -> y <= r -> y + r -> z)
	return 1;
    return 0;
  }

  do{ // Loops in all the polygon's vertices
    x1 = MIN(current -> x, next -> x);
    x2 = MAX(current -> x, next -> x);
    if(!(x1 > r -> x + r -> w || x2 < r -> x)){
      y1 = MIN(current -> y, next -> y);
      y2 = MAX(current -> y, next -> y);
      if(!(y1 > r -> y + r -> z || y2 < r -> y)){ // The edge is next. Perhaps
	                                          //it's colliding...
	if(x1 == x2 || y1 == y2){ // We have a vertical or horizontal edge
	  return 1; // It certainly collides in this case
	}
	else{ // We have a non-vertical edge
	  float a = (next -> y - current -> y) / (next -> x - current -> x);
	  float b = current -> y - a * current -> x; 
	  // Now we have an ax+b=y rect equation
	  if((a * r -> x + b >= r -> y) && (a * r -> x + b <= r -> y + r -> z))
	    return 1; // Collided with the left side
	  if((a * (r -> x + r -> w) + b >= r -> y) && 
	     (a * (r -> x + r -> w) + b <= r -> y + r -> z))
	    return 1; // Collided with the right side
	  if(((r -> y - b)/a >= r -> x) && ((r -> y - b)/a <= r -> x + r -> w))
	    return 1; // Collided with the top side
	  if(((r -> y + r -> z - b)/a >= r -> x) &&
	     ((r -> y + r -> z - b)/a <= r -> x + r -> w))
	    return 1; // Collided with the down side
	}
      }
    }

    current = next;
    next = current -> next;
  }while(current != first);
  
  return 0;
}

// This determines if a circle and a polygon are colliding.
int collision_circle_polygon(struct vector3 *circle, struct vector2 *polygon){
  struct vector2 *current, *next, *first;
  float a, b, dist;
  
  current = first = polygon;
  next = current -> next;
  
  if(next == current){ // We have a degenerate polygon with only one vertex
    dist = (current -> x - circle -> x);
    dist *= dist; // dx * dx
    dist += (current -> y - circle -> y) * (current -> y - circle -> y);
    if(sqrt(dist) < circle -> z)
      return 1; // A collision happenned
    else
      return 0; // No collision happening
  }
 
  // This loops a number of times equal the number of polygon's sides
  // when the polygon isn's degenerated.
  do{
    // If it's not a vertical segment:
    if(current -> x != next -> x){
      a = (next -> y - current -> y) / (next -> x - current -> x);
      b = current -> y - a * current -> x;
      // Rect equation: ax + b = y
      dist = circle -> x * a - circle -> y + b;
      if(dist < 0.0)
        dist *= -1;
      dist /= sqrt(a * a + 1);
      // distance between rect and  point computed
      if(dist < circle -> z){
	// A collision happened or not
	if(!((circle -> x + circle -> z < current -> x && circle -> x + 
	      circle -> z < next-> x &&
              circle -> x - circle -> z < current -> x && circle -> x - 
	      circle -> z < next-> x) ||
             (circle -> x + circle -> z > current -> x && circle -> x + 
	      circle -> z > next-> x &&
              circle -> x - circle -> z > current -> x && circle -> x - 
	      circle -> z > next-> x))){
	  if(!((circle -> y + circle -> z < current -> y && circle -> y + 
		circle -> z < next-> y &&
		circle -> y - circle -> z < current -> y && circle -> y - 
		circle -> z < next-> y) ||
	       (circle -> y + circle -> z > current -> y && circle -> y + 
		circle -> z > next-> y &&
		circle -> y - circle -> z > current -> y && circle -> y - 
		circle -> z > next-> y))){
	    return 1;
	  }
        }
      }
    }
    else{ // We have a vertical segment
      if(!((circle -> x - circle -> z < current -> x && circle -> x + 
	    circle -> z < current -> x) ||
           (circle -> x - circle -> z > current -> x && circle -> x + 
	    circle -> z > current -> x))){
	// A collision happened... or not?
	if(!((circle -> y + circle -> z < current -> y && circle -> y + 
	      circle -> z < next-> y &&
              circle -> y - circle -> z < current -> y && circle -> y - 
	      circle -> z < next-> y) ||
             (circle -> y + circle -> z > current -> y && circle -> y + 
	      circle -> z > next-> y &&
	      circle -> y - circle -> z > current -> y && circle -> y - 
	      circle -> z > next-> y))){
          return 1; // COLLISION!
	}
      }
    }
    current = next;
    next = current -> next;
  }while(current != first);
  
  return 0;
}

// This determines collisions between 2 polygons
int collision_polygon_polygon(struct vector2 *poly1, struct vector2 *poly2){
  struct vector2 *first, *current, *next;
  struct vector2 *first2, *current2, *next2;

  current = first = poly1;
  next = current -> next;
  current2 = first2 = poly2;
  next2 = current2 -> next;

  if(current == next){ // This polygon is a henagon
    if(current2 == next2){ // We have 2 henagons!
      if(current -> x == current2 -> x &&
         current -> y == current2 -> y)
        return 1;
      else
        return 0;
    }
    else{ // The second polygon is not a henagon
      float a, b; // ax + b = y
      do{
        a = (next2 -> y - current2 -> y) / (next2 -> x - current2 -> x);
        b = current2 -> y - a * current2 -> x;
        if(a * current -> x + b == current -> y)
          return 1; // Detected a collision (unlikely to happen with henagons).
        current2 = next2;
        next2 = current2 -> next;
      }while(current2 != first2);
    }
  }
  else{ // The first polygon is not a henagon
    if(current2 == next2){ // But in this case, the second is
      float a, b;
      do{
        a = (next -> y - current -> y) / (next -> x - current -> x);
        b = current -> y - a * current -> x;
        if(a * current2 -> x + b == current2 -> y)
          return 1; // Collision detected.
        current = next;
        next = current -> next;
      }while(current != first);
    }
    else{ // The most common case. No henagons
      float a1, a2, b1, b2, x, y;
      a1 = a2 = b1 = b2 = y = 0.0;

      do{
        do{
          if(next -> x != current -> x){ // The first line is not vertical
            if(next2 -> x != current2 -> x){ // The second line is not vertical
              a1 = (next -> y - current -> y) / (next -> x - current -> x);
              b1 = current -> y - a1 * current -> x;
              a2 = (next2 -> y - current2 -> y) / (next2 -> x - current2 -> x);
              b2 = current2 -> y - a2 * current2 -> x;
              if(a1 != a2){ // Not paralells
                x = (b2 - b1) / (a1 - a2);
                if(!((x < current -> x && x < next -> x) ||
                     (x > current -> x && x > next -> x)))
                  return 1; // Collision !
              }
              else if(b1 == b2)
                return 1;
            }
            else{ // Only the second line is vertical
              y = a1 * current2 -> x + b1;
              if(!((y < current -> y && y < next -> y) ||
                   (y > current -> y && y > next -> y)))
                return 1; // Collision !
            }
          }
          else{ // The first line is vertical
            if(next2 -> x != current2 -> x){ // But the second is not
              y = a2 * current -> x + b2;
              if(!((y < current2 -> y && y < next2 -> y) ||
                   (y > current2 -> y && y > next2 -> y)))
                return 1; // Other collision case.
            }
            else{ // The 2 lines are vertical
              if(current -> y == current2 -> y)
                return 1;
            }
          }
        }while(current2 != first2);
        current = next;
        next = current -> next;
      }while(current != first);
    }
  }
  return 0;
}

