/*
  Copyright (C) 2009 Thiago Leucz Astrizi

 This file is part of Weaver API.

 Weaver API is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Weaver API is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
    
 You should have received a copy of the GNU General Public License
 along with Weaver API.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _VECTOR2_H_
#define _VECTOR2_H_

// Some macro functions
#define destroy_vector2(x) free(x)

struct vector2{
  float x, y;
  struct vector2 *previous, *next;
};

struct vector2 *new_vector2(float x, float y);
void connect_vector2(struct vector2 *, struct vector2 *);
struct vector2 *new_polygon(int, ...);
void destroy_polygon(struct vector2 *);
void rotate_polygon(struct vector2 *, float, float, float);
void move_polygon(struct vector2 *, float, float);

#endif
