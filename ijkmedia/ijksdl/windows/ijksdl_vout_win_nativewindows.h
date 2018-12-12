#ifndef IJKSDL_WINDOWS__IJKSDL_VOUT_WINDOWS_NATIVEWINDOW_H
#define IJKSDL_WINDOWS__IJKSDL_VOUT_WINDOWS_NATIVEWINDOW_H
#ifndef _begin_code_h
#include "../thirdparty/SDL2/begin_code.h"
#endif
#include "../ijksdl_stdinc.h"
#include "../ijksdl_vout.h"

typedef struct ANativeWindow   ANativeWindow;


extern DECLSPEC SDL_Vout * SDLCALL SDL_VoutWindows_CreateForANativeWindow();
extern DECLSPEC void SDLCALL SDL_VoutWindows_SetNativeWindow(SDL_Vout *vout, ANativeWindow *native_window);

#endif