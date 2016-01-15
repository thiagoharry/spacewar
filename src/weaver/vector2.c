/*
  Copyright (C) 2009 Thiago Leucz Astrizi

 This file is part of WeaverAPI.

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

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "vector2.h"

// This function creates a new vector2
struct vector2 *new_vector2(float x, float y){
  struct vector2 *vector;
  vector = (struct vector2 *) malloc(sizeof(struct vector2));
  if(vector != NULL){
    vector -> x = x;
    vector -> y = y;
    vector -> previous = vector -> next = NULL;
  }
  return vector;
}

// And this connects two vectors
void connect_vector2(struct vector2 *a, struct vector2 *b){
  a -> next = b;
  b -> previous = a;
}

// Now a function to create arbitrary polygons for us
struct vector2 *new_polygon(int number_of_vertices, ...){
  struct vector2 *new_vector = NULL;
  struct vector2 *first_vector = NULL, *previous_vector;
  va_list args;
  float x, y;
  int vertices = number_of_vertices;

  previous_vector = NULL;

  va_start(args, number_of_vertices);
  for(;number_of_vertices > 0; number_of_vertices --){
    x = (float) va_arg(args, double);
    y = (float) va_arg(args, double);
    new_vector = new_vector2(x, y);
    if(new_vector == NULL){
      if(previous_vector != NULL){
        previous_vector -> next = first_vector;
        first_vector -> previous = previous_vector;
        destroy_polygon(first_vector);
      }
      return new_vector;
    }
    if(previous_vector != NULL)
      connect_vector2(previous_vector, new_vector);
    else
      first_vector = new_vector;
    previous_vector = new_vector;
  }
  if(vertices > 0){
    new_vector -> next = first_vector;
    first_vector -> previous = new_vector;
  }

  return first_vector;
}

// Free the memory allocated by a polygon
void destroy_polygon(struct vector2 *poly){
  struct vector2 *first_vector = poly;
  struct vector2 *current_vector, *temp;

  current_vector = first_vector;
  do{
    temp = current_vector;
    current_vector = current_vector -> next;
    free(temp);
  }while(current_vector != first_vector);
}

// This function rotates a polygon rad radians in the axis (x, y)
void rotate_polygon(struct vector2 *poly, float x, float y, float rad){
  long double new_x, new_y;
  struct vector2 *first_vector, *current_vector;
  first_vector = current_vector = poly;
  do{
    // Changing base...
    new_x = (long double) current_vector -> x - (long double) x;
    new_y = (long double) current_vector -> y - (long double) y;
    // Rotating
    new_x =  new_x * cosl((long double) rad) - new_y * sinl((long double) rad);
    new_y = ((long double) current_vector -> x - (long double) x) * 
      sinl((long double) rad) + new_y * cosl((long double) rad); 
    // Changing base again...
    current_vector -> x = (float) (new_x + (long double) x);
    current_vector -> y = (float) (new_y + (long double) y);
    current_vector = current_vector -> next;
  }while(current_vector != first_vector);
}

// This moves all the vertices in a polygon
void move_polygon(struct vector2 *poly, float x, float y){
  struct vector2 *first_vertex, *current_vertex;
  first_vertex = current_vertex = poly;
  do{
    current_vertex -> x += x;
    current_vertex -> y += y;
    current_vertex = current_vertex -> next;
  }while(current_vertex != first_vertex);
}
