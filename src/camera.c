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
#include "camera.h"

void initialize_cameras(void){
  int i;
  for(i = 0; i < 9; i ++){
    cam[i] = new_camera((i < 3)?(-800.0):((i < 6)?(0.0):(800.0)),
			(i%3 == 0)?(-800.0):((i%3 == 2)?(0.0):(800.0)),
			800.0, 800.0);
    limit_camera(cam[i], window_width / 2 - 400, window_height / 2 - 400,
		 800, 800);
  }
}

void destroy_cameras(void){
  int i;
  for(i = 0; i < 9; i ++)
    destroy_camera(cam[i]);
}

