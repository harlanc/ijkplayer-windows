
#include "ffpipeline_windows.h"

typedef struct IJKFF_Pipeline_Opaque {
    FFPlayer      *ffp;
    SDL_mutex     *surface_mutex;
	volatile bool  is_surface_need_reconfigure;
	SDL_Vout      *weak_vout;
	float          left_volume;
    float          right_volume;
} IJKFF_Pipeline_Opaque;

static SDL_Class g_pipeline_class = {
    .name = "ffpipeline_windows_media",
};

static IJKFF_Pipenode *func_open_video_decoder(IJKFF_Pipeline *pipeline, FFPlayer *ffp)
{
    IJKFF_Pipeline_Opaque *opaque = pipeline->opaque;
    IJKFF_Pipenode        *node = NULL;
	node = ffpipenode_create_video_decoder_from_ffplay(ffp);
	return node;
}

static SDL_Aout *func_open_audio_output(IJKFF_Pipeline *pipeline, FFPlayer *ffp)
{
    SDL_Aout *aout = NULL;
	aout = SDL_AoutWindows_CreateForOpenAL();
	return aout;
}

static void func_destroy(IJKFF_Pipeline *pipeline)
{
    IJKFF_Pipeline_Opaque *opaque = pipeline->opaque;
	SDL_DestroyMutexP(&opaque->surface_mutex);
fail:
    return;
}

IJKFF_Pipeline *ffpipeline_create_from_windows(FFPlayer *ffp)
{
    ALOGD("ffpipeline_create_from_windows()\n");
    IJKFF_Pipeline *pipeline = ffpipeline_alloc(&g_pipeline_class, sizeof(IJKFF_Pipeline_Opaque));
    if (!pipeline)
        return pipeline;

    IJKFF_Pipeline_Opaque *opaque = pipeline->opaque;
    opaque->ffp                   = ffp;
    opaque->surface_mutex         = SDL_CreateMutex();
    opaque->left_volume           = 1.0f;
    opaque->right_volume          = 1.0f;
    if (!opaque->surface_mutex) {
        ALOGE("ffpipeline-android:create SDL_CreateMutex failed\n");
        goto fail;
    }

    pipeline->func_destroy              = func_destroy;
    pipeline->func_open_video_decoder   = func_open_video_decoder;
    pipeline->func_open_audio_output    = func_open_audio_output;

	return pipeline;
fail:
    ffpipeline_free_p(&pipeline);
    return NULL;
}



