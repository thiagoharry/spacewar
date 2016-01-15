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

#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include "display.h"
#include "keyboard.h"

// This function initializes and cleans our keyboard buffer
void _initialize_keyboard(void){
  int i;
  for(i = 0; i < KEYS; i++)
    keyboard[i] = 0;
}

// And this initialize the mouse info
void _initialize_mouse(void){
  mouse.pressed = 0;
  mouse.x = mouse.y = 0;
}

// This functions watch for input events. If we get one,
// we stores the information in our buffers
void get_input(void){
  XEvent event;
  KeySym pressed_key, released_key;
  mouse.changed = 0;
  
  while(XPending(_dpy)){
    XNextEvent(_dpy, &event);
    
    if(event.type == KeyPress){ // KEYBOARD EVENT
      keyboard[ANY] += 1;
      pressed_key = XLookupKeysym(&event.xkey, 0);
      
      // Treating special cases first 
      if(pressed_key == LEFT_SHIFT || pressed_key == RIGHT_SHIFT)
	keyboard[SHIFT] += 1;
      if(pressed_key == LEFT_CTRL || pressed_key == RIGHT_CTRL)
	keyboard[CTRL] += 1;
      if(pressed_key == LEFT_ALT || pressed_key == RIGHT_ALT)
	keyboard[ALT] += 1;

      keyboard[pressed_key] += 1; //Default behaviour
    }
    else if(event.type == KeyRelease){
      released_key = XLookupKeysym(&event.xkey, 0);
      keyboard[ANY] = 0;
      
      // Some special cases
      if(released_key == LEFT_SHIFT || released_key == RIGHT_SHIFT)
	keyboard[SHIFT] = 0;
      if(released_key == LEFT_CTRL || released_key == RIGHT_CTRL)
	keyboard[CTRL] = 0;
      if(released_key == LEFT_ALT || released_key == RIGHT_ALT)
	keyboard[ALT] = 0;

      
      keyboard[released_key] = 0; // Default behaviour  
    }
    else if(event.type == ButtonPress){ //MOUSE EVENT
      if(! mouse.pressed){
	mouse.pressed = 1;
	mouse.changed = 1;
      }
      mouse.x = event.xbutton.x;
      mouse.y = event.xbutton.y;
    }
    else if(event.type == ButtonRelease){
      if(mouse.pressed){
	mouse.pressed = 0;
	mouse.changed = 1;
      }
      mouse.x = event.xbutton.x;
      mouse.y = event.xbutton.y;
    }
    else if(event.type == MotionNotify){
      mouse.x = event.xmotion.x;
      mouse.y = event.xmotion.y;
    }
  }
}
