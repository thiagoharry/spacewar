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
#include "star.h"

// Mallocs all the data needed by the star and also draw the first
// star image in the screen
void initialize_star(void){
  star.state = 0;
  star.size = 26.0;
  // Loading all the sprites for our animated background
  star.img[0] = new_image("star_tiny.png");
  star.img[1] = new_image("star_small.png");
  star.img[2] = new_image("star_medium.png");
  star.img[3] = new_image("star_big.png");
  star.img[4] = new_image("star_huge.png");
  // Drawing the first image
  draw_star();
  // When the star was created:
  gettimeofday(&(star.time), NULL);
}

// Draws the starfield in the screen
void draw_star(void){
  draw_surface(star.img[star.state], background, 
	       (window_width - star.img[star.state] -> width) / 2,
	       (window_height - star.img[star.state] -> height) / 2);
  draw_rectangle((window_width - star.img[star.state] -> width) / 2 - 1,
		 (window_height - star.img[star.state] -> height) / 2 - 1,
		 star.img[star.state] -> width + 1, 
		 star.img[star.state] -> height + 1, YELLOW);
  blit_surface(background, window, 
	       (window_width - star.img[star.state] -> width) / 2,
	       (window_height - star.img[star.state] -> height) / 2, 
	       star.img[star.state] -> width, star.img[star.state] -> height,
	       (window_width - star.img[star.state] -> width) / 2,
	       (window_height - star.img[star.state] -> height) / 2);
}

// Updates the star state, making it grows or shrink
void update_star(void){
  struct timeval now;
  unsigned long elapsed_microseconds;
  gettimeofday(&now, NULL);
  elapsed_microseconds = (now.tv_usec - star.time.tv_usec) + 
    1000000 * (now.tv_sec - star.time.tv_sec);
  if(elapsed_microseconds > STAR_TIME){
    gettimeofday(&(star.time), NULL);
    switch(star.state){
    case 0:
      star.size += 3.0;
      star.state ++;
      break;
    case 4:
      star.size -= 3.0;
      star.state--;
      break;
    default:
      if(rand()%2){
	star.state++; 
	star.size += 3.0;
      }
      else{
	star.state--; 
	star.size -= 3.0;
      }
      break;
    }
    draw_star();
  }
}

// Free all the memory allocated by the star
void destroy_star(void){
  int i;
  for(i=0; i < 5; i ++)
    destroy_surface(star.img[i]);
}

// Uses gravitational attraction in the given ship
int gravity_to_ship(int i){
  float strenght;
  // Computing the distance:
  float distance = ship[i].x - 400.0;
  distance *= distance;
  distance += (ship[i].y - 400.0) * (ship[i].y - 400.0);
  distance = sqrtf(distance);
  if(distance < star.size + 10.0){
    blow_up(i);
    return 0;
  }  
  // Applying the gravity:
  strenght = STAR_GRAV / (fps * (distance * distance));
  ship[i].dx += ((400.0 - ship[i].x) / distance) * strenght;
  ship[i].dy += ((400.0 - ship[i].y) / distance) * strenght;
  return 1;
}

// Uses gravitational attraction in the given dust
int gravity_to_dust(struct dust *p){
  float strenght;
  // Computing the distance:
  float distance = p -> body -> x - 400.0;
  distance *= distance;
  distance += (p -> body -> y - 400.0) * (p -> body -> y - 400.0);
  distance = sqrtf(distance);
  if(distance < star.size)
    return 0;
  // Applying the gravity:
  strenght = STAR_GRAV / (fps * (distance * distance));
  p -> dx += ((400.0 - p -> body -> x) / distance) * strenght;
  p -> dy += ((400.0 - p -> body -> y) / distance) * strenght;
  return 1;
}

// Uses gravitational attraction in the given shot
int gravity_to_shot(struct shot *p){
  float strenght;
  // Computing the distance:
  float distance = p -> body -> x - 400.0;
  distance *= distance;
  distance += (p -> body -> y - 400.0) * (p -> body -> y - 400.0);
  distance = sqrtf(distance);
  if(distance < star.size)
    return 0;
  // Applying the gravity:
  strenght = STAR_GRAV / (fps * (distance * distance));
  p -> dx += ((400.0 - p -> body -> x) / distance) * strenght;
  p -> dy += ((400.0 - p -> body -> y) / distance) * strenght;
  return 1;
}
