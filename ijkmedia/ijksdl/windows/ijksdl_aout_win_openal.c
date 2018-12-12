#include "al.h"
#include "alc.h"
#include "ijksdl_inc_internal.h"
#include "../ijksdl_aout_internal.h"

#include "ijksdl_aout_win_openal.h"
#include "ijksdl_thread.h"
#define OPENAL_BUFFERS_NUM 1 


static SDL_Class g_openal_class = {
    .name = "OpenAL",
};


typedef struct SDL_Aout_Opaque {
    SDL_cond   *wakeup_cond;
    SDL_mutex  *wakeup_mutex;

    SDL_Thread *audio_tid;
    SDL_Thread _audio_tid;

    SDL_AudioSpec    spec;


    int              bytes_per_frame;
    int              milli_per_buffer;
    int              frames_per_buffer;
    int              bytes_per_buffer;

    volatile bool  need_set_volume;
    volatile float left_volume;
    volatile float right_volume;

    volatile bool  abort_request;
    volatile bool  pause_on;
    volatile bool  need_flush;
    volatile bool  is_running;

    ALuint al_source_id;
    ALint al_state;
    ALuint al_buffers[OPENAL_BUFFERS_NUM];

    ALenum format;

    uint8_t       *buffer;
    size_t         buffer_capacity;
} SDL_Aout_Opaque;


#define CHECK_OPENAL_ERROR(ret__, ...) \
    do { \
    	if ((ret__) != AL_NO_ERROR) \
    	{ \
    		ALOGE(__VA_ARGS__); \
    		goto fail; \
    	} \
    } while (0)


static int clear_buffer(SDL_Aout *aout)
{

    SDL_Aout_Opaque               *opaque           = aout->opaque;
    ALuint alsourceid = opaque->al_source_id;
    int numqueued = 0;
    alGetSourcei(alsourceid, AL_BUFFERS_QUEUED, &numqueued);

    while(numqueued --)
    {
        ALuint buff;
        alSourceUnqueueBuffers(alsourceid, 1, &buff);
        alDeleteBuffers(1, &buff);
    }
}

static void update_queue_buffer(SDL_Aout *aout)
{
    SDL_Aout_Opaque               *opaque           = aout->opaque;
    ALuint alsourceid = opaque->al_source_id;
    int numprocessed;
    alGetSourcei(alsourceid, AL_BUFFERS_PROCESSED, &numprocessed);
    while(numprocessed--)
    {
        ALuint buff;
        alSourceUnqueueBuffers(alsourceid, 1, &buff);
        alDeleteBuffers(1, &buff);
    }
}


static int aout_thread_n(SDL_Aout *aout)
{
    SDL_Aout_Opaque               *opaque           = aout->opaque;
    ALuint alsourceid = opaque->al_source_id;
    

    SDL_AudioCallback              audio_cblk       = opaque->spec.callback;
    void                          *userdata         = opaque->spec.userdata;
    uint8_t                       *next_buffer      = NULL;
    int                            next_buffer_index = 0;
    size_t                         bytes_per_buffer = opaque->bytes_per_buffer;

    SDL_SetThreadPriority(SDL_THREAD_PRIORITY_HIGH);

    if (!opaque->abort_request && !opaque->pause_on)
        alSourcePlay(alsourceid);

    while (!opaque->abort_request) {
      
        SDL_LockMutex(opaque->wakeup_mutex);
        if (!opaque->abort_request && (opaque->pause_on )) {
			alSourcePause(alsourceid);
            while (!opaque->abort_request && opaque->pause_on) {
                SDL_CondWaitTimeout(opaque->wakeup_cond, opaque->wakeup_mutex, 1000);
            }
            if (!opaque->abort_request && !opaque->pause_on) {
                if (opaque->need_flush) {
                    opaque->need_flush = 0;
                    clear_buffer(aout);
                }
                alSourcePlay(alsourceid);
            }
        }
        if (opaque->need_flush) {
            opaque->need_flush = 0;
            clear_buffer(aout);
        }
#if 0
        if (opaque->need_set_volume) {
            opaque->need_set_volume = 0;
            // FIXME: set volume here
        }
#endif
        if (opaque->need_set_volume) {
            opaque->need_set_volume = 0;
            alSourcef(alsourceid,AL_GAIN,(opaque->left_volume + opaque->right_volume) / 2);
            
        }
        SDL_UnlockMutex(opaque->wakeup_mutex);

        next_buffer = opaque->buffer;

        audio_cblk(userdata, next_buffer, bytes_per_buffer);
        if (opaque->need_flush) {
			opaque->need_flush = false;
            clear_buffer(aout);
           
        }

        if (opaque->need_flush) {
            ALOGE("flush");
            opaque->need_flush = 0;
           clear_buffer(aout);
        } else {
            ALuint bufferID = 0;
           int numprocessed;
            alGetSourcei(alsourceid, AL_BUFFERS_PROCESSED, &numprocessed);
            if(numprocessed > 0)
            {
                ALuint buff;
                alSourceUnqueueBuffers(alsourceid, 1, &bufferID);
                alBufferData(bufferID, AL_FORMAT_STEREO16, next_buffer, bytes_per_buffer,44100);
                alSourceQueueBuffers(alsourceid, 1, &bufferID);
            }else{
			
				alGenBuffers(1,&bufferID);
				 alBufferData(bufferID, AL_FORMAT_STEREO16, next_buffer, bytes_per_buffer,44100);
			 alSourceQueueBuffers(alsourceid, 1, &bufferID);
           	}
        	}
	}
		 update_queue_buffer(aout);

        // TODO: 1 if callback return -1 or 0
   // }

    return 0;
}

static int aout_thread(void *arg)
{
    return aout_thread_n(arg);
}

static void aout_free_l(SDL_Aout *aout)
{
    SDLTRACE("%s\n", __func__);
    if (!aout)
        return;

    aout_close_audio(aout);

    SDL_Aout_Opaque *opaque = aout->opaque;
	ALuint alsourceid = opaque->al_source_id;
	ALuint *al_buffers = opaque->al_buffers;

    //delete our source  
    alDeleteSources(1,&alsourceid);  
    //delete our buffer  
    alDeleteBuffers(1,al_buffers);  
    //Get active context  
    ALCcontext *Context=alcGetCurrentContext();  
    //Get device for active context  
    ALCdevice *Device=alcGetContextsDevice(Context);  
    //Disable context  
    alcMakeContextCurrent(NULL);  
    //Release context(s)  
    alcDestroyContext(Context);  
    //Close device  
    alcCloseDevice(Device);

    SDL_DestroyCondP(&opaque->wakeup_cond);
    SDL_DestroyMutexP(&opaque->wakeup_mutex);

    SDL_Aout_FreeInternal(aout);
}




static int aout_open_audio(SDL_Aout *aout, const SDL_AudioSpec *desired, SDL_AudioSpec *obtained)
{

    SDL_Aout_Opaque *opaque = aout->opaque;
    opaque->spec = *desired;
	
    ALuint *al_buffers = opaque->al_buffers;
	int ret = 0;

    ALCcontext *context;  
    ALCdevice *device;  
    device = alcOpenDevice((ALubyte*)"DirectSound3D");  
   
    if (device == NULL)  
        exit(-1);  
   
    //Create context(s)  
    context = alcCreateContext(device, NULL);  
    //Set active context  
    alcMakeContextCurrent(context);  
    // Clear Error Code  
    ALenum rv = alGetError();

	if(rv != AL_NO_ERROR)
	{
		ALOGE("alcMakeContextCurrent failed."); 
	}

    alGenSources(1, &opaque->al_source_id);
    alGenBuffers(OPENAL_BUFFERS_NUM, al_buffers);
  
    alSpeedOfSound(1.0);
    alDopplerVelocity(1.0);
    alDopplerFactor(1.0);
    alSourcef(opaque->al_source_id, AL_PITCH, 1.0f);
    alSourcef(opaque->al_source_id, AL_GAIN, 1.0f);
    alSourcei(opaque->al_source_id, AL_LOOPING, AL_FALSE);
    alSourcef(opaque->al_source_id, AL_SOURCE_TYPE, AL_STREAMING);

    opaque->bytes_per_frame   = desired->channels * 16 / 8;
   // opaque->milli_per_buffer  = 1.0 / (double)desired->freq * 1000;
    opaque->frames_per_buffer = 512;//opaque->milli_per_buffer/1000 * desired->freq; // samplesPerSec is in milli
    opaque->bytes_per_buffer  = opaque->bytes_per_frame * opaque->frames_per_buffer;
    opaque->buffer_capacity   = OPENAL_BUFFERS_NUM * opaque->bytes_per_buffer;

    opaque->buffer          = malloc(opaque->buffer_capacity);
   
    memset(opaque->buffer, 0, opaque->buffer_capacity);

    opaque->pause_on = 1;
    opaque->abort_request = 0;
    opaque->audio_tid = SDL_CreateThreadEx(&opaque->_audio_tid, aout_thread, aout, "ff_aout_openal");
    
    if (obtained) {
        *obtained      = *desired;
        obtained->size = opaque->buffer_capacity;
    }

    return opaque->buffer_capacity;
fail:
    aout_close_audio(aout);
    return -1;
}

 void aout_close_audio(SDL_Aout *aout)
{
    SDLTRACE("aout_close_audio()\n");
    SDL_Aout_Opaque *opaque = aout->opaque;
    if (!opaque)
        return;

    SDL_LockMutex(opaque->wakeup_mutex);
    opaque->abort_request = true;
    SDL_CondSignal(opaque->wakeup_cond);
    SDL_UnlockMutex(opaque->wakeup_mutex);

    SDL_WaitThread(opaque->audio_tid, NULL);
    opaque->audio_tid = NULL;
	freep((void **)&opaque->buffer);
}


static void aout_pause_audio(SDL_Aout *aout, int pause_on)
{
    SDL_Aout_Opaque *opaque = aout->opaque;

    SDL_LockMutex(opaque->wakeup_mutex);
    SDLTRACE("aout_pause_audio_openal(%d)\n", pause_on);
    opaque->pause_on = pause_on;
    if (!pause_on)
        SDL_CondSignal(opaque->wakeup_cond);
    SDL_UnlockMutex(opaque->wakeup_mutex);
}

static void aout_flush_audio(SDL_Aout *aout)
{
    SDL_Aout_Opaque *opaque = aout->opaque;
    SDL_LockMutex(opaque->wakeup_mutex);
    SDLTRACE("aout_flush_audio()");
    opaque->need_flush = 1;
    SDL_CondSignal(opaque->wakeup_cond);
    SDL_UnlockMutex(opaque->wakeup_mutex);
}

static void aout_set_volume(SDL_Aout *aout, float left_volume, float right_volume)
{
    SDL_Aout_Opaque *opaque = aout->opaque;
    SDL_LockMutex(opaque->wakeup_mutex);
    ALOGI("aout_set_volume(%f, %f)", left_volume, right_volume);
    opaque->left_volume = left_volume;
    opaque->right_volume = right_volume;
    opaque->need_set_volume = 1;
    SDL_CondSignal(opaque->wakeup_cond);
    SDL_UnlockMutex(opaque->wakeup_mutex);
}

/*static double aout_get_latency_seconds(SDL_Aout *aout)
{
    SDL_Aout_Opaque *opaque = aout->opaque;
    ALuint alsourceid = opaque->al_source_id;
    int numprocessed;
    alGetSourcei(alsourceid, AL_BUFFERS_PROCESSED, &numprocessed);

    return opaque->milli_per_buffer * (OPENAL_BUFFERS_NUM - numprocessed);

    // ALint sizeInBytes;
    // ALint channels;
    // ALint bits;

    // alGetBufferi(bufferID, AL_SIZE, &sizeInBytes);
    // alGetBufferi(bufferID, AL_CHANNELS, &channels);
    // alGetBufferi(bufferID, AL_BITS, &bits);

    // double lengthInSamples = sizeInBytes * 8 / (channels * bits);

    // ALint frequency;
    // alGetBufferi(bufferID, AL_FREQUENCY, &frequency);

    // double latency = (double)lengthInSamples / (double)frequency;
}*/


SDL_Aout *SDL_AoutWindows_CreateForOpenAL()
{
    SDLTRACE("%s\n", __func__);
    SDL_Aout *aout = SDL_Aout_CreateInternal(sizeof(SDL_Aout_Opaque));
	printf("SDL_AoutWindows_CreateForOpenAL05\n");
    if (!aout)
        return NULL;

    SDL_Aout_Opaque *opaque = aout->opaque;
    opaque->wakeup_cond = SDL_CreateCond();
    opaque->wakeup_mutex = SDL_CreateMutex();
	printf("SDL_AoutWindows_CreateForOpenAL06\n");

    int ret = 0;

    aout->free_l       = aout_free_l;
    aout->opaque_class = &g_openal_class;
    aout->open_audio   = aout_open_audio;
    aout->pause_audio  = aout_pause_audio;
    aout->flush_audio  = aout_flush_audio;
    aout->close_audio  = aout_close_audio;
    aout->set_volume   = aout_set_volume;
    //aout->func_get_latency_seconds = aout_get_latency_seconds;
	printf("SDL_AoutWindows_CreateForOpenAL07\n");
	if(aout!=NULL){
		printf("SDL_AoutWindows_CreateForOpenAL08\n");
		}else{
				printf("SDL_AoutWindows_CreateForOpenAL09\n");
			}

    return aout;
fail:
    aout_free_l(aout);
    return NULL;
}


