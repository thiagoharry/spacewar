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
#ifndef _SOUND_H_
#define _SOUND_H_

#include <alsa/asoundlib.h>
#include <unistd.h>

#define SND_BUFFER 4096 // Our sound buffer has 4KB

int _initialize_sound(void);
void play_sound(char *);
void _play_soundfile(char *, char *);
void play_music(char *);
void stop_music(void);

pid_t _music;
pid_t _sound;

#endif
