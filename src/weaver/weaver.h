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

#ifndef _WEAVER_H_
#define _WEAVER_H_

#include <math.h>
#include <sys/time.h>
#include "display.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "keyboard.h"
#include "sound.h"
#include "image.h"
#include "font.h"

// Macro functions
#define new_camera(x, y, width, height) new_vector4(width, x, y, height)
#define new_camera_w(x, y, width) new_vector4(width, x, y, width *      \
                                              ((float) window_height /  \
                                               (float) window_width))
#define new_camera_h(x, y, height) new_vector4(height * ((float) window_width / (float) window_height), x, y, height)
#define new_circle(x, y, z) new_vector3(x, y, z)
#define destroy_circle(a) free(a)
#define destroy_rectangle(a) free(a)
#define destroy_camera(a) free(a)
#define new_rectangle(x, y, w, z) new_vector4(w, x, y, z)
#define collision_circle_rectangle(a, b) collision_rectangle_circle(b, a)
#define collision_polygon_circle(a, b) collision_circle_polygon(b, a)
#define collision_polygon_rectangle(a, b) collision_rectangle_polygon(b, a)
#define erase_rectangle(a, b) _erase_rectangle(a, b, 0)
#define erase_fullrectangle(a, b) _erase_rectangle(a, b, 1)
#define film_fullpolygon(a, b, c) _film_fullpolygon(a, b, c, 0)
#define erase_fullpolygon(a, b) _film_fullpolygon(a, b, NOT_IMPORTANT, 1)
#define film_polygon(a, b, c) _film_polygon(a, b, c, 0)
#define erase_polygon(a, b) _film_polygon(a, b, NOT_IMPORTANT, 1)
#define MAX(x, y) (x) > (y) ? (x) : (y)
#define MIN(x, y) (x) > (y) ? (y) : (x)
#define DEBUG_TIMER_START() { struct timeval _begin, _end;	\
  XFlush(_dpy);                                                 \
  gettimeofday(&_begin, NULL);
#define DEBUG_TIMER_END() gettimeofday(&_end, NULL);		\
  printf("%ld\n", (1000000 * (_end.tv_sec - _begin.tv_sec) +	\
		   _end.tv_usec - _begin.tv_usec));		\
  }


// Functions
void awake_the_weaver(void);
void may_the_weaver_sleep(void);
void weaver_rest(long);

void center_camera(struct vector4 *, float, float);
void limit_camera(struct vector4 *, int, int, int, int);
void zoom_camera(struct vector4 *, float); 

void film_circle(struct vector4 *, struct vector3 *, unsigned);
void erase_circle(struct vector4 *, struct vector3 *);
void film_fullcircle(struct vector4 *, struct vector3 *, unsigned);
void erase_fullcircle(struct vector4 *, struct vector3 *);
void film_rectangle(struct vector4 *, struct vector4 *, unsigned);
void _erase_rectangle(struct vector4 *, struct vector4 *, int);
void film_fullrectangle(struct vector4 *, struct vector4 *,unsigned);


void _film_polygon(struct vector4 *, struct vector2 *, unsigned, int);
void _film_fullpolygon(struct vector4 *, struct vector2 *, unsigned, int);
void _film_henagon(struct vector4 *, struct vector2 *, unsigned, int);
void _film_limited_henagon(struct vector4 *, struct vector2 *, unsigned, int);
void _film_normal_polygon(struct vector4 *, struct vector2 *, unsigned);
void _erase_normal_polygon(struct vector4 *, struct vector2 *);
void _film_limited_polygon(struct vector4 *, struct vector2 *, unsigned);
void _erase_limited_polygon(struct vector4 *, struct vector2 *);

int collision_circle_circle(struct vector3 *, struct vector3 *);
int collision_circle_polygon(struct vector3 *, struct vector2 *);
int collision_polygon_polygon(struct vector2 *, struct vector2 *);
int collision_rectangle_circle(struct vector4 *, struct vector3 *);
int collision_rectangle_rectangle(struct vector4 *, struct vector4 *);
int collision_rectangle_polygon(struct vector4 *, struct vector2 *);

struct timeval _b_frame, current_time;
int fps;

typedef struct vector2 polygon;
typedef struct vector3 circle;
typedef struct vector4 camera;
typedef struct vector4 rectangle;

#endif
