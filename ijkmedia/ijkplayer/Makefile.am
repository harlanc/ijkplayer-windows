lib_LTLIBRARIES=libijkplayer.la
libijkplayer_la_SOURCES=ff_cmdutils.c ff_ffpipeline.c ff_ffpipenode.c ff_ffplay.c ijkmeta.c ijkplayer.c ijkavformat/allformats.c ijkavformat/ijkasync.c ijkavformat/ijkio.c ijkavformat/ijkioapplication.c ijkavformat/ijkiocache.c ijkavformat/ijkioffio.c ijkavformat/ijkiomanager.c ijkavformat/ijkioprotocol.c ijkavformat/ijkiourlhook.c ijkavformat/ijklivehook.c ijkavformat/ijklongurl.c ijkavformat/ijkmediadatasource.c ijkavformat/ijksegment.c ijkavformat/ijkurlhook.c ijkavutil/ijkdict.c ijkavutil/ijkstl.cpp ijkavutil/ijkfifo.c ijkavutil/ijkthreadpool.c ijkavutil/ijktree.c ijkavutil/ijkutils.c pipeline/ffpipeline_ffplay.c pipeline/ffpipenode_ffplay_vdec.c windows/pipeline/ffpipeline_windows.c windows/ijkplayer_win.c
libijkplayer_la_LDFLAGS     = -no-undefined -version-info 0:0:0 
libijkplayer_la_CFLAGS = -g -O0
AM_CPPFLAGS=-I../../../ijkplayer-ffmpeg/ -I../../../ijkplayer-ffmpeg/build
libijkplayer_la_LIBADD=-L/local/lib -lavformat -lpthread -lavdevice  -lavcodec  -lavutil -lswscale -lswresample -lijksdl






