#include "ijkplayer_win.h"
#include <assert.h>
#include "../ff_fferror.h"
#include "../ff_ffplay.h"
#include "../ijkplayer_internal.h"
#include "../pipeline/ffpipeline_ffplay.h"

IjkMediaPlayer *ijkmp_windows_create(int(*msg_loop)(void*))
{
    IjkMediaPlayer *mp = ijkmp_create(msg_loop);
    if (!mp)
        goto fail;

    mp->ffplayer->vout = SDL_VoutWindows_CreateForWindowsSurface();
    if (!mp->ffplayer->vout)
        goto fail;

    mp->ffplayer->pipeline = ffpipeline_create_from_windows(mp->ffplayer);
    if (!mp->ffplayer->pipeline)
        goto fail;

	return mp;

fail:
    ijkmp_dec_ref_p(&mp);
    return NULL;
}