/*
 * Copyright (c) 2016 Bilibili
 * copyright (c) 2016 Zhang Rui <bbcallen@gmail.com>
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

#ifndef __APPLE__

#include "ijksdl_egl.h"

#include <stdlib.h>
#include <stdbool.h>
#include "ijksdl/ijksdl_gles2.h"
#include "ijksdl/ijksdl_log.h"
#include "ijksdl/ijksdl_vout.h"
#include "ijksdl/gles2/internal.h"

#define IJK_EGL_RENDER_BUFFER 0

typedef struct IJK_EGL_Opaque {
    IJK_GLES2_Renderer *renderer;
} IJK_EGL_Opaque;

static EGLBoolean IJK_EGL_isValid(IJK_EGL* egl)
{
    if (egl &&
        egl->window &&
        egl->display &&
        egl->surface &&
        egl->context) {
        return EGL_TRUE;
    }

    return EGL_FALSE;
}

#ifdef _WIN32

typedef struct SDL_EGL_VideoData
{
    void *egl_dll_handle, *dll_handle;
    EGLDisplay egl_display;
    EGLConfig egl_config;
    int egl_swapinterval;
    int egl_surfacetype;
    
    EGLDisplay(__stdcall *eglGetDisplay) (NativeDisplayType display);
    EGLDisplay(__stdcall *eglGetPlatformDisplay) (EGLenum platform,
                                void *native_display,
                                const EGLint *attrib_list);
    EGLDisplay(__stdcall *eglGetPlatformDisplayEXT) (EGLenum platform,
                                void *native_display,
                                const EGLint *attrib_list);
    EGLBoolean(__stdcall *eglInitialize) (EGLDisplay dpy, EGLint * major,
                                EGLint * minor);
    EGLBoolean(__stdcall  *eglTerminate) (EGLDisplay dpy);
    
    void *(__stdcall *eglGetProcAddress) (const char * procName);
    
    EGLBoolean(__stdcall *eglChooseConfig) (EGLDisplay dpy,
                                  const EGLint * attrib_list,
                                  EGLConfig * configs,
                                  EGLint config_size, EGLint * num_config);
    
    EGLContext(__stdcall *eglCreateContext) (EGLDisplay dpy,
                                   EGLConfig config,
                                   EGLContext share_list,
                                   const EGLint * attrib_list);
    
    EGLBoolean(__stdcall *eglDestroyContext) (EGLDisplay dpy, EGLContext ctx);
    
    EGLSurface(__stdcall *eglCreatePbufferSurface)(EGLDisplay dpy, EGLConfig config,
                                                     EGLint const* attrib_list);

    EGLSurface(__stdcall *eglCreateWindowSurface) (EGLDisplay dpy,
                                         EGLConfig config,
                                         NativeWindowType window,
                                         const EGLint * attrib_list);
    EGLBoolean(__stdcall *eglDestroySurface) (EGLDisplay dpy, EGLSurface surface);
    
    EGLBoolean(__stdcall *eglMakeCurrent) (EGLDisplay dpy, EGLSurface draw,
                                 EGLSurface read, EGLContext ctx);
    
    EGLBoolean(__stdcall *eglSwapBuffers) (EGLDisplay dpy, EGLSurface draw);
    
    EGLBoolean(__stdcall *eglSwapInterval) (EGLDisplay dpy, EGLint interval);
    
    const char *(__stdcall *eglQueryString) (EGLDisplay dpy, EGLint name);
    
    EGLBoolean(__stdcall  *eglGetConfigAttrib) (EGLDisplay dpy, EGLConfig config,
                                     EGLint attribute, EGLint * value);
    
    EGLBoolean(__stdcall *eglWaitNative) (EGLint  engine);

    EGLBoolean(__stdcall *eglWaitGL)(void);
    
    EGLBoolean(__stdcall *eglBindAPI)(EGLenum);

    EGLint(__stdcall *eglGetError)(void);

    EGLBoolean(__stdcall *eglReleaseThread) (void);

    EGLBoolean(__stdcall *eglQuerySurface) (EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value);

} SDL_EGL_VideoData;






static void IJK_EGL_loadLibrary(IJK_EGL* egl)
{


    egl->egl_data->dll_handle = SDL_LoadObject("libEGL.dll");
    /* Load new function pointers */
    LOAD_FUNC(eglGetDisplay);
	LOAD_FUNC(eglGetPlatformDisplayEXT);
    LOAD_FUNC(eglInitialize);
    LOAD_FUNC(eglTerminate);
    LOAD_FUNC(eglGetProcAddress);
    LOAD_FUNC(eglChooseConfig);
    LOAD_FUNC(eglGetConfigAttrib);
    LOAD_FUNC(eglCreateContext);
    LOAD_FUNC(eglDestroyContext);
    LOAD_FUNC(eglCreatePbufferSurface);
    LOAD_FUNC(eglCreateWindowSurface);
    LOAD_FUNC(eglDestroySurface);
    LOAD_FUNC(eglMakeCurrent);
    LOAD_FUNC(eglSwapBuffers);
    LOAD_FUNC(eglSwapInterval);
    LOAD_FUNC(eglWaitNative);
    LOAD_FUNC(eglWaitGL);
    LOAD_FUNC(eglBindAPI);
    LOAD_FUNC(eglQueryString);
    LOAD_FUNC(eglGetError);
	LOAD_FUNC(eglReleaseThread);
}



#endif

void IJK_EGL_terminate(IJK_EGL* egl)
{
    if (!IJK_EGL_isValid(egl))
        return;

    if (egl->opaque)
        IJK_GLES2_Renderer_freeP(&egl->opaque->renderer);
#ifdef _WIN32
    if (egl->display) {
        egl->egl_data->eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl->context)
            egl->egl_data->eglDestroyContext(egl->display, egl->context);
        if (egl->surface)
            egl->egl_data->eglDestroySurface(egl->display, egl->surface);
        egl->egl_data->eglTerminate(egl->display);
        egl->egl_data->eglReleaseThread(); // FIXME: call at thread exit
    }
#else
    if (egl->display) {
        eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl->context)
            eglDestroyContext(egl->display, egl->context);
        if (egl->surface)
            eglDestroySurface(egl->display, egl->surface);
        eglTerminate(egl->display);
        eglReleaseThread(); // FIXME: call at thread exit
    }
#endif
    egl->context = EGL_NO_CONTEXT;
    egl->surface = EGL_NO_SURFACE;
    egl->display = EGL_NO_DISPLAY;
}

static int IJK_EGL_getSurfaceWidth(IJK_EGL* egl)
{
    EGLint width = 0;
#ifdef _WIN32
    if (!egl->egl_data->eglQuerySurface(egl->display, egl->surface, EGL_WIDTH, &width)) {
		ALOGE("[EGL] eglQuerySurface(EGL_WIDTH) returned error %d", egl->egl_data->eglGetError());
#else
    if (!eglQuerySurface(egl->display, egl->surface, EGL_WIDTH, &width)) {
		ALOGE("[EGL] eglQuerySurface(EGL_WIDTH) returned error %d", eglGetError());
#endif
    return 0;
    }

    return width;
}

static int IJK_EGL_getSurfaceHeight(IJK_EGL* egl)
{
    EGLint height = 0;
#ifdef _WIN32
    if (!egl->egl_data->eglQuerySurface(egl->display, egl->surface, EGL_HEIGHT, &height)) {
		ALOGE("[EGL] eglQuerySurface(EGL_HEIGHT) returned error %d", egl->egl_data->eglGetError());
#else
    if (!eglQuerySurface(egl->display, egl->surface, EGL_HEIGHT, &height)) {
        ALOGE("[EGL] eglQuerySurface(EGL_HEIGHT) returned error %d", eglGetError());
#endif
 
        return 0;
    }

    return height;
}

static EGLBoolean IJK_EGL_setSurfaceSize(IJK_EGL* egl, int width, int height)
{
    if (!IJK_EGL_isValid(egl))
        return EGL_FALSE;

#ifdef __ANDROID__
    egl->width  = IJK_EGL_getSurfaceWidth(egl);
    egl->height = IJK_EGL_getSurfaceHeight(egl);

    if (width != egl->width || height != egl->height) {
        int format = ANativeWindow_getFormat(egl->window);
        ALOGI("ANativeWindow_setBuffersGeometry(w=%d,h=%d) -> (w=%d,h=%d);",
            egl->width, egl->height,
            width, height);
        int ret = ANativeWindow_setBuffersGeometry(egl->window, width, height, format);
        if (ret) {
            ALOGE("[EGL] ANativeWindow_setBuffersGeometry() returned error %d", ret);
            return EGL_FALSE;
        }

        egl->width  = IJK_EGL_getSurfaceWidth(egl);
        egl->height = IJK_EGL_getSurfaceHeight(egl);
        return (egl->width && egl->height) ? EGL_TRUE : EGL_FALSE;
    }

    return EGL_TRUE;
#elif _WIN32
	egl->width	= 320;
	egl->height = 240;

    return EGL_TRUE;

#else
    // FIXME: other platform?
#endif
    return EGL_FALSE;
}

static EGLBoolean IJK_EGL_makeCurrent(IJK_EGL* egl, EGLNativeWindowType window)
{
    if (window && window == egl->window &&
        egl->display &&
        egl->surface &&
        egl->context) {
#ifdef _WIN32
        if (!egl->egl_data->eglMakeCurrent(egl->display, egl->surface, egl->surface, egl->context)) {
#else
        if (!eglMakeCurrent(egl->display, egl->surface, egl->surface, egl->context)) {
#endif
            ALOGE("[EGL] elgMakeCurrent() failed (cached)\n");
            return EGL_FALSE;
        }

        return EGL_TRUE;
    }

    IJK_EGL_terminate(egl);
    egl->window = window;

    if (!window)
        return EGL_FALSE;
#ifdef _WIN32
    const EGLint displayAttributes[] =
    {
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_NONE,
    };

	HDC mHDC = GetDC(window);

    EGLDisplay display = egl->egl_data->eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, mHDC, displayAttributes);//eglGetDisplay(EGL_DEFAULT_DISPLAY);
#else
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
 
    if (display == EGL_NO_DISPLAY) {
        ALOGE("[EGL] eglGetDisplay failed\n");
        return EGL_FALSE;
    }


    EGLint major, minor;
#ifdef _WIN32
    if (!egl->egl_data->eglInitialize(display, &major, &minor)) {
#else
    if (!eglInitialize(display, &major, &minor)) {
#endif
        ALOGE("[EGL] eglInitialize failed\n");
        return EGL_FALSE;   
    }
    ALOGI("[EGL] eglInitialize %d.%d\n", (int)major, (int)minor);


    static const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfig;
#ifdef _WIN32
    const EGLint configAttribs[] =
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };

    if (!egl->egl_data->eglChooseConfig(display, configAttribs, &config, 1, &numConfig)) {
		ALOGE("[EGL] eglChooseConfig failed\n");
        egl->egl_data->eglTerminate(display);
#else

    static const EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,
        EGL_BLUE_SIZE,          8,
        EGL_GREEN_SIZE,         8,
        EGL_RED_SIZE,           8,
        EGL_NONE
    };

    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfig)) {
        ALOGE("[EGL] eglChooseConfig failed\n");
        eglTerminate(display);
#endif

        return EGL_FALSE;
    }

#ifdef __ANDROID__
    {
        EGLint native_visual_id = 0;
        if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &native_visual_id)) {
            ALOGE("[EGL] eglGetConfigAttrib() returned error %d", eglGetError());
            eglTerminate(display);
            return EGL_FALSE;
        }

        int32_t width  = ANativeWindow_getWidth(window);
        int32_t height = ANativeWindow_getWidth(window);
        ALOGI("[EGL] ANativeWindow_setBuffersGeometry(f=%d);", native_visual_id);
        int ret = ANativeWindow_setBuffersGeometry(window, width, height, native_visual_id);
        if (ret) {
            ALOGE("[EGL] ANativeWindow_setBuffersGeometry(format) returned error %d", ret);
            eglTerminate(display);
            return EGL_FALSE;
        }
    }
#endif
#ifdef _WIN32
    EGLSurface surface = egl->egl_data->eglCreateWindowSurface(display, config, window, NULL);
#else
    EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
#endif
    if (surface == EGL_NO_SURFACE) {
        ALOGE("[EGL] eglCreateWindowSurface failed\n");
#ifdef _WIN32
        egl->egl_data->eglTerminate(display);
#else
        eglTerminate(display);
#endif
        return EGL_FALSE;
    }

#ifdef _WIN32
    EGLSurface context = egl->egl_data->eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        ALOGE("[EGL] eglCreateContext failed\n");
        egl->egl_data->eglDestroySurface(display, surface);
        egl->egl_data->eglTerminate(display);
        return EGL_FALSE;
    }

    if (!egl->egl_data->eglMakeCurrent(display, surface, surface, context)) {
        ALOGE("[EGL] elgMakeCurrent() failed (new)\n");
        egl->egl_data->eglDestroyContext(display, context);
        egl->egl_data->eglDestroySurface(display, surface);
        egl->egl_data->eglTerminate(display);
        return EGL_FALSE;
    }
#else
    EGLSurface context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (context == EGL_NO_CONTEXT) {
        ALOGE("[EGL] eglCreateContext failed\n");
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return EGL_FALSE;
    }

    if (!eglMakeCurrent(display, surface, surface, context)) {
        ALOGE("[EGL] elgMakeCurrent() failed (new)\n");
        eglDestroyContext(display, context);
        eglDestroySurface(display, surface);
        eglTerminate(display);
        return EGL_FALSE;
    }
#endif

#if 0
#if defined(__ANDROID__)
    {
        const char *extensions = (const char *) glGetString(GL_EXTENSIONS);
        if (extensions) {
            char *dup_extensions = strdup(extensions);
            if (dup_extensions) {
                char *brk = NULL;
                char *ext = strtok_r(dup_extensions, " ", &brk);
                while (ext) {
                    if (0 == strcmp(ext, "GL_EXT_texture_rg"))
                        egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg] = 1;

                    ext = strtok_r(NULL, " ", &brk);
                }

                free(dup_extensions);
            }
        }
    }
#elif defined(__APPLE__)
    egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg] = 1;
#endif

    ALOGI("[EGL] GLES2 extensions begin:\n");
    ALOGI("[EGL]     GL_EXT_texture_rg: %d\n", egl->gles2_extensions[IJK_GLES2__GL_EXT_texture_rg]);
    ALOGI("[EGL] GLES2 extensions end.\n");
#endif


    egl->context = context;
    egl->surface = surface;
    egl->display = display;
    return EGL_TRUE;
}

static EGLBoolean IJK_EGL_prepareRenderer(IJK_EGL* egl, SDL_VoutOverlay *overlay)
{
    assert(egl);
    assert(egl->opaque);

    IJK_EGL_Opaque *opaque = egl->opaque;

    if (!IJK_GLES2_Renderer_isValid(opaque->renderer) ||
        !IJK_GLES2_Renderer_isFormat(opaque->renderer, overlay->format)) {

        IJK_GLES2_Renderer_reset(opaque->renderer);
        IJK_GLES2_Renderer_freeP(&opaque->renderer);

        opaque->renderer = IJK_GLES2_Renderer_create(overlay);
        if (!opaque->renderer) {
            ALOGE("[EGL] Could not create render.");
            return EGL_FALSE;
        }

        if (!IJK_GLES2_Renderer_use(opaque->renderer)) {
            ALOGE("[EGL] Could not use render.");
            IJK_GLES2_Renderer_freeP(&opaque->renderer);
            return EGL_FALSE;
        }
    }

    IJK_GLES2_Renderer_setupGLES();

    if (!IJK_EGL_setSurfaceSize(egl, overlay->w, overlay->h)) {
        ALOGE("[EGL] IJK_EGL_setSurfaceSize(%d, %d) failed\n", overlay->w, overlay->h);
        return EGL_FALSE;
    }
#ifdef _WIN32
    global_render_data->glViewport(0, 0, egl->width, egl->height);  IJK_GLES2_checkError_TRACE("glViewport");
#else
    glViewport(0, 0, egl->width, egl->height);  IJK_GLES2_checkError_TRACE("glViewport");
#endif
    return EGL_TRUE;
}

static EGLBoolean IJK_EGL_display_internal(IJK_EGL* egl, EGLNativeWindowType window, SDL_VoutOverlay *overlay)
{
    IJK_EGL_Opaque *opaque = egl->opaque;

    if (!IJK_EGL_prepareRenderer(egl, overlay)) {
        ALOGE("[EGL] IJK_EGL_prepareRenderer failed\n");
        return EGL_FALSE;
    }

    if (!IJK_GLES2_Renderer_renderOverlay(opaque->renderer, overlay)) {
        ALOGE("[EGL] IJK_GLES2_render failed\n");
        return EGL_FALSE; 
    }
#ifdef _WIN32
    egl->egl_data->eglSwapBuffers(egl->display, egl->surface);
#else
    eglSwapBuffers(egl->display, egl->surface);
#endif

    return EGL_TRUE;
}

EGLBoolean IJK_EGL_display(IJK_EGL* egl, EGLNativeWindowType window, SDL_VoutOverlay *overlay)
{
    EGLBoolean ret = EGL_FALSE;
    if (!egl)
        return EGL_FALSE;

    IJK_EGL_Opaque *opaque = egl->opaque;
    if (!opaque)
        return EGL_FALSE;

    if (!IJK_EGL_makeCurrent(egl, window))
        return EGL_FALSE;

    ret = IJK_EGL_display_internal(egl, window, overlay);

#ifdef _WIN32
    egl->egl_data->eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    egl->egl_data->eglReleaseThread(); // FIXME: call at thread exit
#else
    eglMakeCurrent(egl->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglReleaseThread(); // FIXME: call at thread exit
#endif
    return ret;
}

void IJK_EGL_releaseWindow(IJK_EGL* egl)
{
    if (!egl || !egl->opaque || !egl->opaque->renderer)
        return;

    IJK_EGL_terminate(egl);
}

void IJK_EGL_free(IJK_EGL *egl)
{
    if (!egl)
        return;

    IJK_EGL_terminate(egl);

    memset(egl, 0, sizeof(IJK_EGL));
    free(egl);
}

void IJK_EGL_freep(IJK_EGL **egl)
{
    if (!egl || !*egl)
        return;

    IJK_EGL_free(*egl);
    *egl = NULL;
}

static SDL_Class g_class = {
    .name = "EGL",
};

IJK_EGL *IJK_EGL_create()
{
    IJK_EGL *egl = (IJK_EGL*) mallocz(sizeof(IJK_EGL));
    if (!egl)
        return NULL;

    egl->opaque_class = &g_class;
    egl->opaque = mallocz(sizeof(IJK_EGL_Opaque));
    if (!egl->opaque) {
        free(egl);
        return NULL;
    }

	#ifdef _WIN32
    egl->egl_data = (SDL_EGL_VideoData*)mallocz(sizeof(SDL_EGL_VideoData));
    if(!egl->egl_data)
        return NULL;
    IJK_EGL_loadLibrary(egl);
    #endif

    return egl;
}

#endif
