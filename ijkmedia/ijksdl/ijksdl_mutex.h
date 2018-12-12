/*****************************************************************************
 * ijksdl_mutex.h
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

#ifndef IJKSDL__IJKSDL_MUTEX_H
#define IJKSDL__IJKSDL_MUTEX_H

#ifndef _begin_code_h
#include "thirdparty/SDL2/begin_code.h"
#endif

#include <stdint.h>
#include <pthread.h>

#define SDL_MUTEX_TIMEDOUT  1
#define SDL_MUTEX_MAXWAIT   (~(uint32_t)0)

typedef struct SDL_mutex {
    pthread_mutex_t id;
} SDL_mutex;

extern DECLSPEC SDL_mutex * SDLCALL SDL_CreateMutex(void);
extern DECLSPEC void     SDLCALL   SDL_DestroyMutex(SDL_mutex *mutex);
extern DECLSPEC void     SDLCALL  SDL_DestroyMutexP(SDL_mutex **mutex);
extern DECLSPEC int      SDLCALL  SDL_LockMutex(SDL_mutex *mutex);
extern DECLSPEC int      SDLCALL  SDL_UnlockMutex(SDL_mutex *mutex);

typedef struct SDL_cond {
    pthread_cond_t id;
} SDL_cond;

extern DECLSPEC SDL_cond * SDLCALL SDL_CreateCond(void);
extern DECLSPEC void     SDLCALL   SDL_DestroyCond(SDL_cond *cond);
extern DECLSPEC void     SDLCALL   SDL_DestroyCondP(SDL_cond **mutex);
extern DECLSPEC int      SDLCALL   SDL_CondSignal(SDL_cond *cond);
extern DECLSPEC int      SDLCALL   SDL_CondBroadcast(SDL_cond *cond);
extern DECLSPEC int      SDLCALL   SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, uint32_t ms);
extern DECLSPEC int      SDLCALL   SDL_CondWait(SDL_cond *cond, SDL_mutex *mutex);

#endif

