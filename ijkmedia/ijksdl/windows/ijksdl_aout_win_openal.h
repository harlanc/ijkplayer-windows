#ifndef IJKSDL_WINDOWS__IJKSDL_AOUT_WINDOWS_OPENAL_H
#define IJKSDL_WINDOWS__IJKSDL_AOUT_WINDOWS_OPENAL_H

#ifndef _begin_code_h
#include "../thirdparty/SDL2/begin_code.h"
#endif

#include "ijksdl_aout.h"

extern DECLSPEC SDL_Aout * SDLCALL SDL_AoutWindows_CreateForOpenAL();
extern DECLSPEC bool SDLCALL  SDL_AoutWindows_IsObjectOfOpenAL(SDL_Aout *aout);

#endif