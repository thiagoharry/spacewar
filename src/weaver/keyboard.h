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

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <X11/keysym.h>

// Macro functions
#define clean_keyboard() _initialize_keyboard()

// Number of keys watched
#define KEYS 0xffff+0x4

// Position of each key
#define UP          XK_Up
#define RIGHT       XK_Right
#define DOWN        XK_Down
#define LEFT        XK_Left
#define PLUS        XK_KP_Add
#define MINUS       XK_KP_Subtract
#define SHIFT       KEYS-1
#define CTRL        KEYS-2
#define ESC         XK_Escape
#define A           XK_a
#define S           XK_s
#define D           XK_d
#define W           XK_w
#define ENTER       XK_Return
#define LEFT_CTRL   XK_Control_L
#define RIGHT_CTRL  XK_Control_R
#define F1          XK_F1
#define ANY         KEYS-3
#define ALT         KEYS-4
#define F2          XK_F2
#define F3          XK_F3
#define F4          XK_F4
#define F5          XK_F5
#define F6          XK_F6
#define F7          XK_F7
#define F8          XK_F8
#define F9          XK_F9
#define F10         XK_F10
#define F11         XK_F11
#define F12         XK_F12
#define BACKSPACE   XK_BackSpace
#define TAB         XK_Tab
#define PAUSE       XK_Pause
#define DELETE      XK_Delete
#define SCROLL_LOCK XK_Scroll_Lock
#define HOME        XK_Home
#define PAGE_UP     XK_Page_Up
#define PAGE_DOWN   XK_Page_Down
#define END         XK_End
#define INSERT      XK_Insert
#define NUM_LOCK    XK_Num_Lock
#define ZERO        XK_KP_0
#define ONE         XK_KP_1
#define TWO         XK_KP_2
#define THREE       XK_KP_3
#define FOUR        XK_KP_4
#define FIVE        XK_KP_5
#define SIX         XK_KP_6
#define SEVEN       XK_KP_7
#define EIGHT       XK_KP_8
#define NINE        XK_KP_9
#define LEFT_SHIFT  XK_Shift_L
#define RIGHT_SHIFT XK_Shift_R
#define CAPS_LOCK   XK_Caps_Lock
#define LEFT_ALT    XK_Alt_L
#define RIGHT_ALT   XK_Alt_R
#define Q           XK_q
#define E           XK_e
#define R           XK_r
#define T           XK_t
#define Y           XK_y
#define U           XK_u
#define I           XK_i
#define O           XK_o
#define P           XK_p
#define F           XK_f
#define G           XK_g
#define H           XK_h
#define J           XK_j
#define K           XK_k
#define L           XK_l
#define Z           XK_z
#define X           XK_x
#define C           XK_c
#define V           XK_v
#define B           XK_b
#define N           XK_n
#define M           XK_m

int keyboard[KEYS];

struct{
  int pressed;
  int changed;
  int x, y;
} mouse;

void _initialize_keyboard(void);
void get_input(void);
void _initialize_mouse(void);

#endif
