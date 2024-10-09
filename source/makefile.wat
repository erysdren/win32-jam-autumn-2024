
TARGET=bisected.exe
RM=rm -f

SDL_DIR=./SDL

CC=wcc386
CFLAGS=&
	-bt=nt&
	-omaxnet -bm -ei -wx -zp4 -5r -fp5 -zw&
	-I$(SDL_DIR)&

OBJS=&
	config.obj&
	main.obj&
	maths.obj&
	vid_gl.obj&

LIBS=&
	$(SDL_DIR)/SDL.lib&
	$(SDL_DIR)/SDLmain.lib&
	opengl32.lib&
	glu32.lib&

.c.obj:
	$(CC) $(CFLAGS) -fo=$*.obj $*.c

all: clean $(TARGET) .SYMBOLIC

clean: .SYMBOLIC
	$(RM) $(TARGET) $(OBJS) wlink.rsp

$(TARGET): $(OBJS) wlink.rsp
	wlink $(LDFLAGS) @wlink.rsp

wlink.rsp: makefile.wat
	echo name $(TARGET) >wlink.rsp
	echo system nt_win >>wlink.rsp
	for %i in ($(OBJS)) do echo file %i >>wlink.rsp
	for %i in ($(LIBS)) do echo libfile %i >>wlink.rsp
