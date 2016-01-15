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
#include "ships.h"

// Initializes the number of ships passed as argument.
void initialize_ships(int number){
  tank.empty = new_image("empty_tank.png");
  tank.full  = new_image("full_tank.png");
  if(number < NUMBER_OF_SHIPS){
    int i;
    for(i = NUMBER_OF_SHIPS - 1; i > number - 1; i --)
      ship[i].body =  NULL;
  }
  if(number < 1)
    return;
  // The first ship have 3 polygons:
  ship[0].body = (polygon **) malloc(4*sizeof(polygon *));
  ship[0].body[0] = new_polygon(4, 
				130.0, 159.0,
				125.0, 164.0, // The fuel tank
				119.0, 159.0,
				125.0, 153.0);
  ship[0].body[1] = new_polygon(7,
				133.0, 145.0,
				150.0, 184.0,
				111.0, 167.0,
				122.0, 162.0, // The ship body
				125.0, 164.0,
				130.0, 159.0,
				128.0, 156.0);
  ship[0].body[2] = new_polygon(2,
				144.0, 173.0, // The cockpit
				139.0, 178.0);
  ship[0].propulsor[0] = ship[0].body[0];
  ship[0].propulsor[1] = NULL; // This ship has only 1 propulsor
  ship[0].body[3] = NULL; // The array must end with a NULL pointer. 
  ship[0].front = ship[0].body[1] -> next;
  ship[0].color = BLUE;
  ship[0].x = 136.0; ship[0].y = 170.0;
  ship[0].dx = ship[0].dy = 0.0;
  ship[0].status = ALIVE;
  gettimeofday(&(ship[0].time), NULL);
  ship[0].fuel = FUEL;
  if(number < 2)
    return;
  // The second ship have 5 polygons
  ship[1].body = (polygon **) malloc(6*sizeof(polygon *));
  ship[1].body[0] = new_polygon(12,
				649.0, 628.0,
				650.0, 614.0, // The cockpit
				657.0, 620.0,
				650.0, 614.0,
				664.0, 613.0,
				649.0, 628.0,
				652.0, 629.0,
				662.0, 630.0,
				675.0, 638.0,
				666.0, 626.0,
				665.0, 616.0,
				664.0, 613.0);
  ship[1].body[1] = new_polygon(2,
				668.0, 636.0, // Left wing
				668.0, 645.0);
  ship[1].body[2] = new_polygon(2,
				673.0, 632.0, // Right wing
				681.0, 632.0);
  ship[1].body[3] = new_polygon(2,
				674.0, 625.0, // Right propulsor
				691.0, 642.0);
  ship[1].body[4] = new_polygon(2,
				662.0, 638.0,  // Left propulsor
				678.0, 654.0);
  ship[1].body[5] = NULL; // The NULL pointer marks the end of array
  ship[1].propulsor[0] = ship[1].body[3];
  ship[1].propulsor[1] = ship[1].body[4];
  ship[1].front = ship[1].body[0] -> next;
  ship[1].color = RED;
  ship[1].x = 665.0; ship[1].y = 630.0;
  ship[1].dx = ship[1].dy = 0.0;
  ship[1].status = ALIVE;
  gettimeofday(&(ship[1].time), NULL);
  ship[1].fuel = FUEL;
}

// Film initialized ships
void film_ships(void){
  int p, i, j;
  for(j = 0; j < 9; j++){
    for(i = 0; i < NUMBER_OF_SHIPS; i ++){
      if(ship[i].body != NULL && ship[i].status == ALIVE){
	for(p = 0; ship[i].body[p] != NULL; p ++){
	  film_fullpolygon(cam[j], ship[i].body[p], BLACK); // 3-5
	  film_polygon(cam[j], ship[i].body[p], ship[i].color); // 4-5
	}
      }
    }
  }
}
    
// Erase initialized ships
void erase_ships(void){
  int p, i, j;
  for(j = 0; j < 9; j ++){
    for(i = 0; i < NUMBER_OF_SHIPS; i ++){
      if(ship[i].body != NULL && ship[i].status == ALIVE){
	for(p = 0; ship[i].body[p] != NULL; p ++){
	  erase_fullpolygon(cam[j], ship[i].body[p]);
	  erase_polygon(cam[j], ship[i].body[p]);
	}
      }
    }
  }
}



// Free the memory used by the initialized ships
void destroy_ships(void){
  int i;
  destroy_surface(tank.empty);
  destroy_surface(tank.full);
  for(i = 0; i < NUMBER_OF_SHIPS; i ++){
    if(ship[i].body != NULL){
      int j;
      for(j = 0; ship[i].body[j] != NULL; j ++)
	destroy_polygon(ship[i].body[j]);
    }
  } 
}

// Updates each initialized ship position, based in the ship's slope
void update_ships(void){
  int i;
  for(i = 0; i < NUMBER_OF_SHIPS; i ++){
    if(ship[i].body != NULL && ship[i].status == ALIVE){
      float dx = ship[i].dx / fps;
      float dy = ship[i].dy / fps;
      int j;
      for(j = 0; ship[i].body[j] != NULL; j ++)
	move_polygon(ship[i].body[j], dx, dy);
      ship[i].x += dx;
      ship[i].y += dy;
      if(ship[i].x < 0.0){
	for(j = 0; ship[i].body[j] != NULL; j ++)
	  move_polygon(ship[i].body[j], 800.0, 0.0);
	ship[i].x += 800.0;
      }
      else if(ship[i].x > 800.0){
	for(j = 0; ship[i].body[j] != NULL; j ++)
	  move_polygon(ship[i].body[j], -800.0, 0.0);
	ship[i].x -= 800.0;
      }
      if(ship[i].y < 0.0){
	for(j = 0; ship[i].body[j] != NULL; j ++)
	  move_polygon(ship[i].body[j], 0.0, 800.0);
	ship[i].y += 800.0;
      }
      else if(ship[i].y > 800.0){
	for(j = 0; ship[i].body[j] != NULL; j ++)
	  move_polygon(ship[i].body[j], 0.0, -800.0);
	ship[i].y -= 800.0;
      }
      gravity_to_ship(i);
    }
    else if(ship[i].body != NULL && ship[i].status == HYPERSPACE){
      /* If we spent too much time in hyperspace, came back. */
      struct timeval now;
      unsigned long elapsed_microseconds;
      gettimeofday(&now, NULL);
      elapsed_microseconds = (now.tv_usec - ship[i].time.tv_usec) +
	1000000 * (now.tv_sec - ship[i].time.tv_sec);
      if(elapsed_microseconds > HYPERSPACE_TIME){
	return_hyperspace(i);
      } 
    } 
  }
}

// Rotates a given ship to LEFT of RIGHT Pi/2 radians per second
void rotate_ship(int i, int direction){
  if(ship[i].body != NULL && ship[i].status == ALIVE){
    int j;
    float rotation = ((direction == LEFT)?(-SHIP_TURN/fps):(SHIP_TURN/fps));
    for(j = 0; ship[i].body[j] != NULL; j ++)
      rotate_polygon(ship[i].body[j], ship[i].x, ship[i].y, rotation);
  }
}

// Blows up a vertex creating 2**(n+1) particles. The vertex is limited by
// the edges (x1, y1) and (x2, y2). The coordinate (bx, by) is the
// center of the explosion:
void blow_up_vertex(int n, float x1, float y1, float x2, float y2, float bx,
		    float by, unsigned color){
  // The recursion end:
  if(n == 0){
    float distance, dx, dy;
    float x = (x1 + x2) / 2;
    float y = (y1 + y2) / 2;
    // Computing the point's distance to the ship gravity center:
    distance = bx - x;
    distance *= distance;
    distance += (by - y) * (by - y);
    distance = sqrtf(distance);
    // Computing the explosion srenght:
    dx = - ((bx - x) / distance * distance) * BLOW_UP;
    dy = - ((by - y) / distance * distance) * BLOW_UP;
    new_dust(x, y, dx, dy, color);
    new_dust(x, y, dx / 2, dy / 2, color);
  }
  else{
    blow_up_vertex(n-1, x1, y1, (x1 + x2)/2, (y1 + y2)/2, bx, by, color);
    blow_up_vertex(n-1, (x1 + x2)/2, (y1 + y2)/2, x2, y2, bx, by, color);
  }
}

// Blows up a given ship
void blow_up(int i){
  int j;
  polygon *point, *first;
  if(ship[i].body != NULL && ship[i].status == ALIVE){
    //erase_ships();
    ship[i].status = DEAD;
    ship[i].fuel = 1.0;
    gettimeofday(&(ship[i].time), NULL);
    for(j = 0; ship[i].body[j] != NULL; j ++){
      point = first = ship[i].body[j];
      do{
	blow_up_vertex(2, point -> x, point -> y, 
		       point -> next -> x, point -> next -> y,
		       ship[i].x, ship[i].y, ship[i].color);
	point = point -> next;
      }while(point != first);
    }
    play_sound("explosion.ogg");
    draw_tank(i, (i == 0)?(window_width / 2 - 421):(window_width / 2 + 401),
	      window_height / 2 - 150);
  }
}

// Makes the given spaceship fire
void ship_fire(int i){
  if(ship[i].status == ALIVE){
    unsigned long elapsed_microseconds;
    struct timeval now;

    gettimeofday(&now, NULL);
    elapsed_microseconds = (now.tv_usec - ship[i].time.tv_usec) +
      1000000 * (now.tv_sec - ship[i].time.tv_sec);
    if(elapsed_microseconds > SHOT_PRODUCTION){
      gettimeofday(&(ship[i].time), NULL);
      new_shot(ship[i].front -> x, ship[i].front  -> y,
	       ((ship[i].front -> x - ship[i].x) / 15.0) 
	       * SHOT_SPEED,
	       ((ship[i].front -> y - ship[i].y) / 15.0) 
	       * SHOT_SPEED, ship[i].color, i);
    }
  }
}

// Activate a given ships' propulsors
void propulse_ship(int i){
  int prop;
  if(ship[i].fuel > 0.0 && ship[i].status == ALIVE && ship[i].status == ALIVE){
    if(ship[i].body != NULL && ship[i].fuel > 0.0){
      ship[i].dx += ((ship[i].front -> x - ship[i].x)/20.0) * SHIP_ACCEL / 
	fps;
      ship[i].dy += ((ship[i].front -> y - ship[i].y)/20.0) * SHIP_ACCEL / 
	fps;
      ship[i].fuel -= 100.0 / (float) fps;
      // Creating the dust:
      for(prop = 0; prop < PROPULSORS; prop ++)
	if(ship[i].propulsor[prop] != NULL){
	  polygon *point, *start;
	  point = start = ship[i].propulsor[prop];
	  do{
	    if(rand()%3){
	      new_dust(point -> x, point -> y,
		       -((ship[i].front -> x - ship[i].x) / 15.0) 
		       * SHIP_ACCEL / fps,
		       -((ship[i].front -> y - ship[i].y) / 15.0) 
		       * SHIP_ACCEL / fps, WHITE);
	    }
	    if(rand()%3){
	      new_dust((point -> x + point -> next -> x)/2, 
		       (point -> y + point -> next -> y)/2,
		       -((ship[i].front -> x - ship[i].x) / 15.0) 
		       * SHIP_ACCEL / fps,
		       -((ship[i].front -> y - ship[i].y) / 15.0) 
		       * SHIP_ACCEL / fps, WHITE);
	    }
	    point = point -> next;
	  } while(point != start); 
	}
    }
    draw_tank(i, (i == 0)?(window_width / 2 - 421):(window_width / 2 + 401),
    	      window_height / 2 - 150);
  }
}

// Sends a given ship to hyperspace
void goto_hyperspace(int i){
  if(ship[i].status == ALIVE && ship[i].fuel >= HYPERSPACE_FUEL){
    int j;
    float dx = 400.0 - (float) (rand() % 800);
    float dy = 400.0 - (float) (rand() % 800);
    gettimeofday(&(ship[i].time), NULL);
    ship[i].status = HYPERSPACE;
    for(j = 0; ship[i].body[j] !=  NULL; j++){
      move_polygon(ship[i].body[j], dx, dy);
    }
    ship[i].x += dx;
    ship[i].y += dy;
    ship[i].fuel -= HYPERSPACE_FUEL;
    play_sound("hyper.ogg");
    draw_tank(i, (i == 0)?(window_width / 2 - 421):(window_width / 2 + 401),
	      window_height / 2 - 150);
  }
}

// Returns a given ship from hyperspace
void return_hyperspace(int i){
  if(ship[i].status == HYPERSPACE){
    gettimeofday(&(ship[i].time), NULL);
    ship[i].status = ALIVE;
  }
}

// This function draws the fuel tank associated with a given spaceship
void draw_tank(int i, int x, int y){
  if(ship[i].fuel <= 0)
    return;
  draw_surface(tank.empty, window, x, y);
  {
    int height = 300 - 3 * (int) (ship[i].fuel / (FUEL / 100.0));
    blit_surface(tank.full, window, 
		 0, height, tank.full -> width, tank.full -> height - height,
		 x, y + height);
  }
}
