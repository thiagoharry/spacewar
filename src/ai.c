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
#include "ai.h"

/* This function sets each AI status as "START" */
void initialize_ai(void){
  ai_status[0] = ai_status[1] = START;
}

/*
  This function makes the AI control the given spaceship
*/
void ai_play(int i){
  // Determining the initial strategy
  if(ai_status[i] == START)
    ai_status[i] = rand() % 4 + 1;
  if(sqrtf((ship[i].dx * ship[i].dx) + 
	   (ship[i].dy * ship[i].dy)) >= GOOD_SPEED){
    ai_status[i] = FIGHTING;
  }
  // Initial strategy 1:
  if(ai_status[i] == START_MOVE1){
    rotate_ship(i, LEFT);
    propulse_ship(i);
  }
  // Initial strategy 2:
  else if(ai_status[i] == START_MOVE2){
    rotate_ship(i, RIGHT);
    propulse_ship(i);
  }
  // Initial strategy 3:
  else if(ai_status[i] == START_MOVE2){
    propulse_ship(i);
  }
  else{
    struct shot *pointer = list_of_shot;
    // We are in the fighting stance
    // We are almost touching the star:
    if(sqrtf((ship[i].x - 400.0)*(ship[i].x - 400.0) + 
	     (ship[i].y - 400.0)*(ship[i].y - 400.0)) <= 80.0){
      goto_hyperspace(i);
      return;
    }
    // We are almost being hitten by a shot:
    while(pointer != NULL){
      if(pointer -> owner != i)
	if(sqrtf((ship[i].x - pointer -> body -> x) * 
		 (ship[i].x - pointer -> body -> x) + 
		 (ship[i].y - pointer -> body -> y) * 
		 (ship[i].y - pointer -> body -> y))
	   <= 35.0){
	  goto_hyperspace(i);
	  return;
	} 
      pointer = pointer -> next;
    }
    // If the enemy is not reachable, do nothing
    if(ship[!i].status == DEAD)
      return;
    ship_fire(i);
    if(ship[!i].status != ALIVE)
      return;
    {
      float real_x = ship[!i].x , real_y = ship[!i].y;
      // If we are here, we should correct our aim
      if(ship[i].front -> x - ship[!i].x > 400.0)
	real_x += 800.0;
      else if(ship[i].front -> x - ship[!i].x < -400.0)
	real_x -= 800.0;
      if(ship[i].front -> y - ship[!i].y > 400.0)
	real_y += 800.0;
      else if(ship[i].front -> y - ship[!i].y < -400.0)
	real_y -= 800.0;
      
      {
	// Now we check if it's better to turn left or right
	float distance_left, distance_right;
	rotate_ship(i, LEFT);
	distance_left = sqrtf((ship[i].front -> x - real_x) *
			      (ship[i].front -> x - real_x) +
			      (ship[i].front -> y - real_y) *
			      (ship[i].front -> y - real_y));
	rotate_ship(i, RIGHT);
	rotate_ship(i, RIGHT);
	distance_right = sqrtf((ship[i].front -> x - real_x) *
			       (ship[i].front -> x - real_x) +
			      (ship[i].front -> y - real_y) *
			      (ship[i].front -> y - real_y));
	if(distance_left < distance_right){
	  rotate_ship(i, LEFT);
	  rotate_ship(i, LEFT);
	}
      }
    }
    
  }

}
