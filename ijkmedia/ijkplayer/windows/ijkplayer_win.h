#ifndef IJKPLAYER_WINDOWS__IJKPLAYER_WINDOWS_H
#define IJKPLAYER_WINDOWS__IJKPLAYER_WINDOWS_H

#include "../ijkplayer.h"
#include "../ijkplayer_internal.h"

IjkMediaPlayer *ijkmp_windows_create(int(*msg_loop)(void*));

#endif