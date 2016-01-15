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

#ifndef _SHOT_H_
#define _SHOT_H_
#include "ai.h"
#include "star.h"
#include "ships.h"
#include "game.h"
#include "dust.h"
#include "camera.h"
#include "weaver/weaver.h"

// How much microseconds a shot exists
#define SHOT_LIFE 2500000
// The shot size (radius)
#define SHOT_SIZE 3.0
// How much time it takes to produce a shot (microseconds)
#define SHOT_PRODUCTION 500000
// The shot speed:
#define SHOT_SPEED 150.0

struct shot{
  circle *body;
  float dx, dy;
  struct timeval time;
  struct shot *next, *previous;
  unsigned color;
  int owner;
};

struct shot *list_of_shot;

void initialize_shot(void);
void destroy_shot(void);
void new_shot(float, float, float, float, unsigned, int);
int shot_hits(struct shot *);
void update_shot(void);
void film_shot(void);
void erase_shot(void);

#endif
