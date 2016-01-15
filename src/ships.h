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

#ifndef _SHIPS_H_
#define _SHIPS_H_
#include "ai.h"
#include "shot.h"
#include "dust.h"
#include "camera.h"
#include "star.h"
#include "game.h"
#include "weaver/weaver.h"

#define DEAD       0
#define ALIVE      1
#define HYPERSPACE 2

#define FUEL       1650.0 // How much fuel each ship have
#define NUMBER_OF_SHIPS 2
#define PROPULSORS      2 // Number of propulsors
#define SHIP_TURN (M_PI / 2) // Angular speed per second
#define SHIP_ACCEL 50.0 // Ship acceleration
#define BLOW_UP 10.0
#define HYPERSPACE_TIME 1000000lu
#define HYPERSPACE_FUEL 165.0

struct {
  polygon **body; // Ship's polygons
  unsigned color;
  float x, y;     // Position of the gravity center
  float dx, dy;   // slope
  int status;     // ALIVE, DEAD or HYPERSPACE
  struct timeval time; // Last time the status changed
  float fuel;
  polygon *front;
  polygon *propulsor[PROPULSORS];
} ship[NUMBER_OF_SHIPS];

struct {
  surface *full;
  surface *empty;
} tank;

void initialize_ships(int);
void film_ships();
void erase_ships();
void destroy_ships(void);

void update_ships(void);
void rotate_ship(int, int);
void propulse_ship(int);

void blow_up_vertex(int, float, float, float, float, float, float, unsigned);
void blow_up(int);

void ship_fire(int);
void goto_hyperspace(int);
void return_hyperspace(int);

void draw_tank(int, int, int);

#endif
