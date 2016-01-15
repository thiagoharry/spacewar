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
#include "shot.h"

void initialize_shot(void){
  list_of_shot = NULL;
}

// Free memory allocated for the shots
void destroy_shot(void){
  struct shot *pointer = list_of_shot;
  while(pointer != NULL){
    if(pointer -> next != NULL){
      pointer = pointer -> next;
      free(pointer -> previous);
    }
    else{
      free(pointer);
      pointer = list_of_shot = NULL;
    }
  }
}

// Creates a new shot at coordinates (x, y) with the given
// slope (dx, dy), the given color and the given owner
void new_shot(float x, float y, float dx, float dy, unsigned color, int owner){
  struct shot *new_shot = (struct shot *) malloc(sizeof(struct shot));
  // Initializing some values
  new_shot -> body = new_circle(x, y, SHOT_SIZE);
  new_shot -> dx = dx;
  new_shot -> dy = dy;
  new_shot -> color = color;
  gettimeofday(&(new_shot -> time), NULL);
  new_shot -> previous = new_shot -> next = NULL;
  new_shot -> owner = owner;

  // Putting the new shot in list_of_shot:
  if(list_of_shot == NULL){
    list_of_shot = new_shot;
  }
  else{
    struct shot *pointer = list_of_shot;
    while(pointer -> next != NULL)
      pointer = pointer -> next;
    pointer -> next = new_shot;
    new_shot -> previous = pointer;
  }
}

// Determines if a shot hits the enemy
int shot_hits(struct shot *bullet){
  int i, j;
  for(i = 0; i < NUMBER_OF_SHIPS; i ++){
    if(i == bullet -> owner)
      continue;
    for(j = 0; ship[i].body[j] != NULL; j ++){
      if(collision_circle_polygon(bullet -> body, ship[i].body[j])){
	blow_up(i);
	return 1;
      }
    }
  }
  return 0;
}

// Move each shot particle and destroy the ancient ones and the
// particles that touch the star in the center of screen
void update_shot(void){
  struct shot *pointer = list_of_shot;
  struct shot *temp;
  struct timeval now;
  unsigned long elapsed_microseconds;

  while(pointer != NULL){
    // Moving
    move_circle(pointer -> body, pointer -> dx / fps, pointer -> dy / fps);
    if(pointer -> body -> x < 0.0) pointer -> body -> x += 800.0;
    if(pointer -> body -> x > 800.0) pointer -> body -> x -= 800.0;
    if(pointer -> body -> y < 0.0) pointer -> body -> y += 800.0;
    if(pointer -> body -> y > 800.0) pointer -> body -> y -= 800.0;

    shot_hits(pointer);
        
    // Erasing if the shot is too old
    gettimeofday(&now, NULL);
    elapsed_microseconds = (now.tv_usec - pointer -> time.tv_usec) +
      1000000 * (now.tv_sec - pointer  -> time.tv_sec);
    if(elapsed_microseconds > SHOT_LIFE || ! gravity_to_shot(pointer)){
      if(pointer -> previous == NULL){ // The first shot
	if(pointer -> next == NULL){ // It's the only shot
	  free(pointer);
	  list_of_shot = NULL;
	  return;
	}
	else{ // We are in the first, but there's others
	  pointer = pointer -> next;
	  free(pointer -> previous);
	  pointer -> previous = NULL;
	  list_of_shot = pointer;
	  continue;
	}
      }
      else{ // We aren't in the first shot
	if(pointer -> next == NULL){ // But we are in the last
	  pointer -> previous -> next = NULL;
	  free(pointer);
	  return;
	}
	else{ // Not the first, nor the last
	  pointer -> previous -> next = pointer -> next;
	  pointer -> next -> previous = pointer -> previous;
	  temp = pointer;
	  pointer = pointer -> previous;
	  free(temp);
	  continue;
	}
      }
    }
    pointer = pointer -> next;
  }
}

void film_shot(void){
  int i;
  struct shot *p;
  for(i = 0; i < 9; i++){
    p = list_of_shot;
    while(p != NULL){
      film_fullcircle(cam[i], p -> body, p -> color);
      p = p -> next;
    }
  }
}

void erase_shot(void){
  int i;
  struct shot *p = list_of_shot;
  for(i = 0; i < 9; i++){
    p = list_of_shot;
    while(p != NULL){
      erase_fullcircle(cam[i], p -> body);
      p = p -> next;
    }
  }
}


