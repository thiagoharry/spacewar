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

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <X11/Xlib.h>
#include <X11/extensions/Xdbe.h>

// Defining colors...
#define NOT_IMPORTANT       0x000000
#define BLACK               0x000000
#define BLUE                0x0000ff
#define GREEN               0x00ff00
#define CYAN                0x00ffff
#define BROWN               0x964b00
#define BEIGE               0xf5f5dc
#define RED                 0xff0000
#define YELLOW              0xffff00
#define WHITE               0xffffff

// Some useful macros
#define fill_screen(x) fill_rectangle(0, 0, window_width, window_height, x)
#define draw_surface(a, b, x, y) blit_surface(a, b, 0, 0, a -> width, a -> height, x, y)
#define erase_screen() draw_surface(background, window, 0, 0)

typedef struct surface{
  Pixmap pix;
  int width;
  int height;
  Pixmap mask;
} surface;

Display *_dpy;
Window _w;
XdbeBackBuffer _b;
int window_width, window_height;
GC _gc, _mask_gc;
int _screen, _depth;
Visual *_visual;
struct surface *window;
struct surface *background;
unsigned long transparent_color;

struct surface *new_surface(int, int);
void destroy_surface(struct surface *);
void blit_surface(struct surface *, struct surface *, int, int, int, int, int, int);
void fill_surface(struct surface *, unsigned);

void _initialize_screen(void);
void hide_cursor(void);
void draw_point(unsigned, unsigned, unsigned);
void draw_line(unsigned, unsigned, unsigned, unsigned, unsigned);
void draw_circle(unsigned, unsigned, unsigned, unsigned);
void fill_circle(unsigned, unsigned, unsigned, unsigned);
void draw_rectangle(unsigned, unsigned, unsigned, unsigned, unsigned);
void fill_rectangle(unsigned, unsigned, unsigned, unsigned, unsigned);
void draw_ellipse(unsigned, unsigned, unsigned, unsigned, unsigned);
void fill_ellipse(unsigned, unsigned, unsigned, unsigned, unsigned);

void draw_rectangle_mask(struct surface *, int , int , int , int );

void blit_masked_pixmap(Pixmap, Pixmap, struct surface *, int , 
			int, int, int, int, 
			int, int, int);
void apply_texture(struct surface *, struct surface *);
void flush(void);

#endif
