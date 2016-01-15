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

#ifndef _GAME_H_
#define _GAME_H_
#include "ai.h"
#include "shot.h"
#include "dust.h"
#include "camera.h"
#include "ships.h"
#include "star.h"

int game(int);
void draw_gametype(unsigned, int, int);
void copyleft(void);
void intro(void);

#endif
