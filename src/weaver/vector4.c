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

#include <stdlib.h>
#include "vector4.h"

struct vector4 *new_vector4(float w, float x, float y, float z){
  struct vector4 *vector;
  vector = (struct vector4 *) malloc(sizeof(struct vector4));
  if(vector != NULL){
    vector -> previous = vector -> next = vector -> top = 
      vector -> down = NULL;
    vector -> w = w;
    vector -> x = x;
    vector -> y = y;
    vector -> z = z;
  }
  return vector;
}
