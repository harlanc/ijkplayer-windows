noinst_PROGRAMS=winplayer
winplayer_SOURCES=ijkplayer_win_test.c 

winplayer_LDFLAGS=-D_GNU_SOURCE -no-undefined -version-info 0:0:0 
AM_CPPFLAGS=-I../../ijksdl/include -I../../../../ijkplayer-ffmpeg/ -I../../../../ijkplayer-ffmpeg/build
winplayer_LDADD=-L/local/lib -lijkplayer -lijksdl 
