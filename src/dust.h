/*
  Copyright (C) 2011,2012 Thiago "Harry" Leucz Astrizi

 This file is part of spacewar.

 spacewar is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 spacewar is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with spacewar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _DUST_H_
#define _DUST_H_
#include "ai.h"
#include "shot.h"
#include "star.h"
#include "ships.h"
#include "game.h"
#include "camera.h"
#include "weaver/weaver.h"

// How many microseconds a dust particle exists in the screen
#define DUST_LIFE 1000000

struct dust{
  polygon *body;
  float dx, dy;
  unsigned color;
  struct timeval time;
  struct dust *next, *previous;
} *list_of_dust;

void initialize_dust(void);
void destroy_dust(void);
void new_dust(float, float, float, float, unsigned);
void update_dust(void);
void film_dust(void);
void erase_dust(void);

#endif
