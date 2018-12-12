
#ifndef IJKSDL_WINDOWS__IJKSDL_VOUT_WINDOWS_SURFACE_H
#define IJKSDL_WINDOWS__IJKSDL_VOUT_WINDOWS_SURFACE_H


#include "../ijksdl_stdinc.h"
#include "../ijksdl_vout.h"

extern DECLSPEC SDL_Vout * SDLCALL SDL_VoutWindows_CreateForWindowsSurface();
extern DECLSPEC void SDLCALL SDL_VoutWindows_SetWindowsSurface(SDL_Vout *vout, HWND hwnd);

#endif