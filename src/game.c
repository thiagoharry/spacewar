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

#include "weaver/weaver.h"
#include "game.h"

#define CPU_X_CPU 0
#define PLR_X_CPU 1
#define CPU_X_PLR 2
#define PLR_X_PLR 3

int main(int argc, char **argv){
  unsigned type = PLR_X_CPU;
  int key_pressed = 0;
  awake_the_weaver(); // Initializing Weaver API
  hide_cursor();
  copyleft();
  intro();
  surface *title = new_image("title.png");
  for(;;){
    int esc_pressed = 0;
    int x = (window_width - title -> width) / 2; 
    int y = (window_height - title -> height) / 2;
    fill_screen(BLACK); 
    draw_surface(title, window, x, y);
    draw_gametype(type % 4, x + 120, y + 60);
    key_pressed = 0;
    for(;;){
      get_input();
      if(keyboard[ESC]){
	esc_pressed = 1;
	break;
      }
      if(keyboard[ENTER]){
	fill_screen(BLACK);
	flush();
	game(type % 4);
	break;
      }
      else if(keyboard[LEFT]){
	if(!key_pressed){
	  type --;
	  draw_gametype(type % 4, x + 120, y + 60);
	}
      }
      else if(keyboard[RIGHT]){
	if(!key_pressed){
	  type ++;
	  draw_gametype(type % 4, x + 120, y + 60);
	}
      }
      key_pressed = keyboard[ANY];
      weaver_rest(10000000);
    }
    if(esc_pressed)
      break;
  }
  destroy_surface(title);
  may_the_weaver_sleep();
  return 0;
}

void copyleft(void){
  int width, height;
  surface *c = new_image("copyleft.png");
  width = c -> width;
  height = c -> height;
  fill_screen(BLACK);
  draw_surface(c, window, (window_width - width)/2, 
	       (window_height - height)/2);
  weaver_rest(1);
  sleep(1);
  destroy_surface(c);
  return;
}

void intro(void){
  int i, sound = 0;
  time_t begin, now;
  float speed[6];
  circle *planet[6], *moon1;
  camera *cam = new_camera(0.0, 0.0, 800.0, 800.0);
  surface *dark_weaver = new_image("dark_weaver.png");
  surface *credits = new_image("credits.png");
  for(i = 0; i < 6; i ++){
    speed[i] = ((rand() % 20) + 10) / 1000.0;
    planet[i] = new_circle(450.0 + i*50, 450.0 + i*50, (rand() % 20) + 10.0);
  }
  moon1 = new_circle(planet[2] -> x + 40.0, planet[2] -> y + 40.0, 10.0);
  limit_camera(cam, window_width / 2 - 400, window_height / 2 - 400,
	       800, 800);
  begin = time(NULL);
  initialize_star();
  for(;;){
    now = time(NULL);
    if(now - begin > 3){
      draw_surface(dark_weaver, window, 
		   (window_width - dark_weaver -> width) / 2, 
		   (window_height - dark_weaver -> height) / 2);
      draw_surface(credits, window, (window_width - credits -> width) / 2, 0);
      if(sound == 0){
	sound = 1;
	play_sound("weaver.ogg");
      }
      if(now - begin > 5)
	break;
    }
    for(i = 0; i < 6; i ++)
      erase_fullcircle(cam, planet[i]);
    erase_fullcircle(cam, moon1);
    for(i = 0; i < 6; i ++)
      rotate_circle(planet[i], 400.0, 400.0, speed[i]);
    rotate_circle(moon1, planet[2] -> x, planet[2] -> y, speed[2]*10.0);
    film_fullcircle(cam, planet[0], 0x220000);
    film_fullcircle(cam, planet[1], 0x002200);
    film_fullcircle(cam, planet[2], 0x000022);
    film_fullcircle(cam, planet[3], 0x002222);
    film_fullcircle(cam, planet[4], 0x220022);
    film_fullcircle(cam, planet[5], 0x222222);
    film_fullcircle(cam, moon1, 0x444400);
    update_star();
    weaver_rest(10000000);
  }
  destroy_surface(dark_weaver);
  destroy_surface(credits);
  destroy_star();
}

void draw_gametype(unsigned type, int x, int y){
  surface *select = new_image("select.png");
  surface *cpu = new_image("cpu.png");
  surface *human = new_image("human.png");

  draw_surface(select, window, x, y);
  if(type % 2)
    draw_surface(human, window, x + select -> width / 2 - cpu -> width, y);
  else
    draw_surface(cpu, window, x + select -> width / 2 - cpu -> width, y);
  if((type / 2) % 2)
    draw_surface(human, window, x + select -> width / 2, y);
  else
    draw_surface(cpu, window, x + select -> width / 2, y);
  destroy_surface(human);
  destroy_surface(cpu);
  destroy_surface(select);
}


int game(int type){
  int game_over = 0;
  {
    surface *background = new_image("background.png");
    apply_texture(background, window);
    destroy_surface(background);
  }
  initialize_cameras();initialize_star();initialize_ships(2);initialize_dust();
  initialize_shot();initialize_ai();
  play_music("music.ogg");
  draw_tank(0, (window_width / 2 - 421), window_height / 2 - 150);
  draw_tank(1, window_width / 2 + 401, window_height / 2 - 150);
    // Main loop
  for(;;){
    int i;
    get_input();
    if(keyboard[ESC] || game_over){
      break;
    }
    
    for(i = 0; i < 2; i ++)
      if(ship[i].status == DEAD){
	struct timeval now;
	gettimeofday(&now, NULL);
	if((int) (now.tv_sec - ship[i].time.tv_sec) > 2)
	  game_over = 1;
      }



    erase_ships();    
    erase_shot();     
    
    if(type != CPU_X_CPU){ // Rotation: 10 Propulse: 20 Fire: 20 Hyper: 180 
      int my_ship;
      // Player 1 moves
      if(type == CPU_X_PLR){
	ai_play(0);
	my_ship = 1;
      }
      else if(type == PLR_X_CPU){
	my_ship = 0;
	ai_play(1);
      }
      else
	my_ship = 0;
      if(keyboard[LEFT])
	rotate_ship(my_ship, LEFT); 
      if(keyboard[RIGHT])
	rotate_ship(my_ship, RIGHT);
      if(keyboard[UP])
	propulse_ship(my_ship);
      if(keyboard[DOWN])
	ship_fire(my_ship);
      if(keyboard[RIGHT_CTRL])
	goto_hyperspace(my_ship);
    }
    else{
      ai_play(0);
      ai_play(1);
    }

    if(type == PLR_X_PLR){ 
      // Player 2 moves
      if(keyboard[A])
	rotate_ship(1, LEFT);
      if(keyboard[D])
	rotate_ship(1, RIGHT);
      if(keyboard[W])
	propulse_ship(1);
      if(keyboard[L])
	blow_up(1);
      if(keyboard[S])
	ship_fire(1);
      if(keyboard[LEFT_CTRL])
	goto_hyperspace(1);
    }
    
    update_star(); // 6
    update_ships(); // 2
    update_dust(); // 145
    update_shot(); // 16
    film_ships(); // Até 16
    film_dust(); // 20
    film_shot(); // Até 20
    weaver_rest(10000000);
  }
  stop_music();
  destroy_star();
  destroy_ships();
  destroy_cameras();
  destroy_dust();
  destroy_shot();
  clean_keyboard();
  return 0;
}
