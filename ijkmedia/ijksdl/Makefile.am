lib_LTLIBRARIES=libijksdl.la
libijksdl_la_SOURCES=ijksdl_aout.c ijksdl_audio.c ijksdl_egl.c ijksdl_error.c ijksdl_mutex.c ijksdl_stdinc.c ijksdl_thread.c ijksdl_timer.c ijksdl_vout.c windows/ijksdl_aout_win_openal.c windows/ijksdl_utils_win.c windows/ijksdl_vout_win_nativewindows.c windows/ijksdl_vout_win_surface.c gles2/fsh/rgb.fsh.c gles2/fsh/yuv420p.fsh.c gles2/fsh/yuv420sp.fsh.c gles2/fsh/yuv444p10le.fsh.c gles2/vsh/mvp.vsh.c gles2/color.c gles2/common.c gles2/renderer.c gles2/renderer_rgb.c gles2/renderer_yuv420p.c gles2/renderer_yuv420sp.c gles2/renderer_yuv444p10le.c gles2/shader.c ffmpeg/abi_all/image_convert.c ffmpeg/ijksdl_vout_overlay_ffmpeg.c 
libijksdl_la_LDFLAGS     = -no-undefined -version-info 0:0:0 
libijksdl_la_CFLAGS = -g -O0
AM_CPPFLAGS=-I../../../ijkplayer-ffmpeg/ -I../../../ijkplayer-ffmpeg/build -I./include
libijksdl_la_LIBADD=-L/local/lib -lEGL -lGLESv2 -lpthread -lswscale -lavutil  -L/local/bin -lOpenAL32