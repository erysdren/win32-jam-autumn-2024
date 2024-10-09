ARCH?=$(shell uname -m)

TARGET?=bisected.$(ARCH)
RM?=rm -f
PKGCONFIG?=pkg-config
PKGS=sdl gl glu

override CFLAGS += $(shell $(PKGCONFIG) $(PKGS) --cflags)
override LDFLAGS += $(shell $(PKGCONFIG) $(PKGS) --libs) -lm

OBJS = config.o main.o maths.o

all: clean $(TARGET)

clean:
	$(RM) $(OBJS) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
