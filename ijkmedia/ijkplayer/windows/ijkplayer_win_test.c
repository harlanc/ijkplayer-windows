#include "ijkplayer_win.h"
#include <windows.h>
#include "../../ijksdl/windows/ijksdl_vout_win_surface.h"

static bool tag = true;
HANDLE msgEvent;

static int message_loop(void *mpp)
{
   
    IjkMediaPlayer *mp = (IjkMediaPlayer*)mpp;
    while (1) {
        AVMessage msg;

	    int retval = ijkmp_get_msg(mp, &msg, 1);
        if (retval < 0)
            break;

        // block-get should never return 0
        assert(retval > 0);

        switch (msg.what) {
        case FFP_MSG_FLUSH:
            printf("FFP_MSG_FLUSH:\n");
		    break;
        case FFP_MSG_ERROR:
            printf("FFP_MSG_ERROR: %d\n", msg.arg1);
	        break;
        case FFP_MSG_PREPARED:
            printf("FFP_MSG_PREPARED:\n");
			ijkmp_start(mp);
		    break;
        case FFP_MSG_COMPLETED:
            printf("FFP_MSG_COMPLETED:\n");
			break;
        case FFP_MSG_VIDEO_SIZE_CHANGED:
            printf("FFP_MSG_VIDEO_SIZE_CHANGED: %d, %d\n", msg.arg1, msg.arg2);
			break;
        case FFP_MSG_SAR_CHANGED:
            printf("FFP_MSG_SAR_CHANGED: %d, %d\n", msg.arg1, msg.arg2);
			break;
        case FFP_MSG_VIDEO_RENDERING_START:
            printf("FFP_MSG_VIDEO_RENDERING_START:\n");
			break;
        case FFP_MSG_AUDIO_RENDERING_START:
            printf("FFP_MSG_AUDIO_RENDERING_START:\n");
			break;
        case FFP_MSG_VIDEO_ROTATION_CHANGED:
            printf("FFP_MSG_VIDEO_ROTATION_CHANGED: %d\n", msg.arg1);
			break;
        case FFP_MSG_AUDIO_DECODED_START:
            printf("FFP_MSG_AUDIO_DECODED_START:\n");
			break;
        case FFP_MSG_VIDEO_DECODED_START:
            printf("FFP_MSG_VIDEO_DECODED_START:\n");
			break;
        case FFP_MSG_OPEN_INPUT:
            printf("FFP_MSG_OPEN_INPUT:\n");
			break;
        case FFP_MSG_FIND_STREAM_INFO:
            printf("FFP_MSG_FIND_STREAM_INFO:\n");
			break;
        case FFP_MSG_COMPONENT_OPEN:
            printf("FFP_MSG_COMPONENT_OPEN:\n");
			break;
        case FFP_MSG_BUFFERING_START:
            printf("FFP_MSG_BUFFERING_START:%d\n",msg.arg1);
			break;
        case FFP_MSG_BUFFERING_END:
            printf("FFP_MSG_BUFFERING_END:%d\n",msg.arg1);
			break;
        case FFP_MSG_BUFFERING_UPDATE:
            printf("FFP_MSG_BUFFERING_UPDATE: %d, %d", msg.arg1, msg.arg2);
			break;
        case FFP_MSG_BUFFERING_BYTES_UPDATE:
			break;
        case FFP_MSG_BUFFERING_TIME_UPDATE:
			break;
        case FFP_MSG_SEEK_COMPLETE:
            printf("FFP_MSG_SEEK_COMPLETE:\n");
			break;
        case FFP_MSG_ACCURATE_SEEK_COMPLETE:
            printf("FFP_MSG_ACCURATE_SEEK_COMPLETE:\n");
			break;
        case FFP_MSG_PLAYBACK_STATE_CHANGED:
            break;
        case FFP_MSG_TIMED_TEXT:
			break;
        case FFP_MSG_GET_IMG_STATE:
			break;
        case FFP_MSG_VIDEO_SEEK_RENDERING_START:
            printf("FFP_MSG_VIDEO_SEEK_RENDERING_START:\n");
			break;
        case FFP_MSG_AUDIO_SEEK_RENDERING_START:
            printf("FFP_MSG_AUDIO_SEEK_RENDERING_START:\n");
			break;
        default:
            ALOGE("unknown FFP_MSG_xxx(%d)\n", msg.what);
            break;
        }
        msg_free_res(&msg);
    }

LABEL_RETURN:
    ;
}


// NOTE: support to be called from read_thread
static int
inject_callback(void *opaque, int what, void *data, size_t data_size)
{
    int ret = 0;
    return ret;
}

LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND CreateWindowNative(int w, int h)
{
    WNDCLASSEXW windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEXW);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.hIcon = NULL;
    windowClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName ="test GL ES";
    if (!RegisterClassExW(&windowClass))
    {
        return false;
    }

    DWORD style = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU;
    DWORD extendedStyle = WS_EX_APPWINDOW;

    RECT sizeRect = { 0, 0, w, h };
    AdjustWindowRectEx(&sizeRect, style, false, extendedStyle);

    HWND mNativeWindow = CreateWindowExW(extendedStyle, "test GL ES", "test GL ES", style, CW_USEDEFAULT, CW_USEDEFAULT,
        sizeRect.right - sizeRect.left, sizeRect.bottom - sizeRect.top, NULL, NULL,
        GetModuleHandle(NULL), NULL);

    ShowWindow(mNativeWindow, SW_SHOW);
	return mNativeWindow;
}

int main(int argc,char *argv[])
{
    msgEvent = CreateEvent(NULL,TRUE,FALSE,TEXT("WriteEvent"));
	if(msgEvent == NULL){
		printf("create event error.\n");
		return -1;
		}
	printf("parameter num %d",argc);
	printf("path %s\n",argv[0]);
	printf("path %s\n",argv[1]);
	
	ijkmp_global_init();
    ijkmp_global_set_inject_callback(inject_callback);
	
	IjkMediaPlayer *mp = ijkmp_windows_create(&message_loop);
	
    ijkmp_set_option_int(mp,4,"opensles",0);
	ijkmp_set_option(mp,4,"overlay-format","fcc-_es2");
	ijkmp_set_option_int(mp,4,"framedrop",1);
	ijkmp_set_option_int(mp,4,"start-on-prepared",1);

    ijkmp_set_inject_opaque(mp, NULL);
    ijkmp_set_ijkio_inject_opaque(mp, NULL);
    ijkmp_set_option_int(mp,1,"http-detect-range-support",0);
	

	HWND hwnd = CreateWindowNative(320,240);

	SDL_VoutWindows_SetWindowsSurface(mp->ffplayer->vout, hwnd);
	ijkmp_set_data_source(mp,argv[1]);
	ijkmp_prepare_async(mp);

    BOOL bRet;
    MSG msg;
    while(bRet = GetMessage(&msg,NULL,0,0)){
	    if(bRet != -1){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
}
}