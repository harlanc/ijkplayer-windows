/*****************************************************************************
 * ijksdl_thread.h
 *****************************************************************************
 *
 * Copyright (c) 2013 Bilibili
 * copyright (c) 2013 Zhang Rui <bbcallen@gmail.com>
 *
 * This file is part of ijkPlayer.
 *
 * ijkPlayer is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * ijkPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with ijkPlayer; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef IJKSDL__IJKSDL_TIMER_H
#define IJKSDL__IJKSDL_TIMER_H

#ifndef _begin_code_h
#include "thirdparty/SDL2/begin_code.h"
#endif

#include "ijksdl_stdinc.h"

extern DECLSPEC void SDLCALL SDL_Delay(Uint32 ms);

extern DECLSPEC Uint64 SDLCALL SDL_GetTickHR(void);


typedef struct SDL_Profiler
{
    int64_t total_elapsed;
    int     total_counter;

    int64_t sample_elapsed;
    int     sample_counter;
    float   sample_per_seconds;
    int64_t average_elapsed;

    int64_t begin_time;

    int     max_sample;
} SDL_Profiler;

extern DECLSPEC void  SDLCALL  SDL_ProfilerReset(SDL_Profiler* profiler, int max_sample);
extern DECLSPEC void  SDLCALL  SDL_ProfilerBegin(SDL_Profiler* profiler);
extern DECLSPEC int64_t SDLCALL SDL_ProfilerEnd(SDL_Profiler* profiler);

typedef struct SDL_SpeedSampler
{
    Uint64  samples[10];

    int     capacity;
    int     count;
    int     first_index;
    int     next_index;

    Uint64  last_log_time;
} SDL_SpeedSampler;

extern DECLSPEC void SDLCALL SDL_SpeedSamplerReset(SDL_SpeedSampler *sampler);
// return samples per seconds
extern DECLSPEC float SDLCALL SDL_SpeedSamplerAdd(SDL_SpeedSampler *sampler, int enable_log, const char *log_tag);



typedef struct SDL_SpeedSampler2
{
    int64_t sample_range;
    int64_t last_profile_tick;
    int64_t last_profile_duration;
    int64_t last_profile_quantity;
    int64_t last_profile_speed;
} SDL_SpeedSampler2;

extern DECLSPEC  void  SDLCALL  SDL_SpeedSampler2Reset(SDL_SpeedSampler2 *sampler, int sample_range);
extern DECLSPEC  int64_t SDLCALL SDL_SpeedSampler2Add(SDL_SpeedSampler2 *sampler, int quantity);
extern DECLSPEC  int64_t SDLCALL SDL_SpeedSampler2GetSpeed(SDL_SpeedSampler2 *sampler);

#endif
