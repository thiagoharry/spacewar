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
#include "dust.h"

void initialize_dust(void){
  list_of_dust = NULL;
}

// Free memory allocated for the dust particles
void destroy_dust(void){
  struct dust *pointer = list_of_dust;
  while(pointer != NULL){
    if(pointer -> next != NULL){
      pointer = pointer -> next;
      free(pointer -> previous);
    }
    else{
      free(pointer);
      pointer = list_of_dust = NULL;
    }
  }
}

// Creates a new dust particle at coordinates (x, y) with the given
// slope (dx, dy) and with the given color
void new_dust(float x, float y, float dx, float dy, unsigned color){
  struct dust *new_dust = (struct dust *) malloc(sizeof(struct dust));
  // Initializing some values
  new_dust -> body = new_polygon(1, x, y);
  new_dust -> dx = dx;
  new_dust -> dy = dy;
  new_dust -> color = color;
  gettimeofday(&(new_dust -> time), NULL);
  new_dust -> previous = new_dust -> next = NULL;

  // Putting the new dust in list_of_dust:
  if(list_of_dust == NULL){
    list_of_dust = new_dust;
  }
  else{
    struct dust *pointer = list_of_dust;
    while(pointer -> next != NULL)
      pointer = pointer -> next;
    pointer -> next = new_dust;
    new_dust -> previous = pointer;
  }
}

// Move each dust particle and destroy the ancient ones and the
// particles that touch the star in the center of screen
void update_dust(void){
  struct dust *pointer = list_of_dust;
  struct dust *temp;
  struct timeval now;
  unsigned long elapsed_microseconds;
  float distance;

  while(pointer != NULL){
    // Moving
    move_polygon(pointer -> body, pointer -> dx / fps, pointer -> dy / fps);
    // Computing the distance:
    distance = pointer -> dx - 400.0;
    distance *= distance;
    distance += (pointer -> dy - 400.0) * (pointer -> dy - 400.0);
    distance = sqrtf(distance);
    
    // Erasing if the dust is too old
    gettimeofday(&now, NULL);
    elapsed_microseconds = (now.tv_usec - pointer -> time.tv_usec) +
      1000000 * (now.tv_sec - pointer  -> time.tv_sec);
    if(elapsed_microseconds > DUST_LIFE || ! gravity_to_dust(pointer)){
      if(pointer -> previous == NULL){ // The first dust
	if(pointer -> next == NULL){ // It's the only dust
	  free(pointer);
	  list_of_dust = NULL;
	  return;
	}
	else{ // We are in the first, but there's others
	  pointer = pointer -> next;
	  free(pointer -> previous);
	  pointer -> previous = NULL;
	  list_of_dust = pointer;
	  continue;
	}
      }
      else{ // We aren't in the first dust
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

void film_dust(void){
  int i;
  struct dust *p;
  for(i = 0; i < 9; i++){
    p = list_of_dust;
    while(p != NULL){
      film_polygon(cam[i], p -> body, p -> color);
      p = p -> next;
    }
  }
}

void erase_dust(void){
  int i;
  struct dust *p = list_of_dust;
  for(i = 0; i < 9; i++){
    p = list_of_dust;
    while(p != NULL){
      erase_polygon(cam[i], p -> body);
      p = p -> next;
    }
  }
}


