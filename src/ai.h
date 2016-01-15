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

#ifndef _AI_H_
#define _AI_H_
#include "star.h"
#include "shot.h"
#include "ships.h"
#include "game.h"
#include "dust.h"
#include "camera.h"
#include "weaver/weaver.h"

// The AI status:
#define START       0
#define START_MOVE1 1
#define START_MOVE2 2
#define START_MOVE3 3
#define FIGHTING    4

// A speedconsidered good for the AI
#define GOOD_SPEED 62.0

/* The status of each Artificial Intelligence */
int ai_status[2];

void initialize_ai(void);
void ai_play(int);

#endif
