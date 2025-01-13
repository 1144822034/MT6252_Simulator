CC := gcc

CFLAGS := -g -lkernel32 -Wall -DNETWORK_SUPPORT 
OBJS := 

UNICORN = Lib/unicorn-2.0.1-min/unicorn-import.lib

SDL2 = Lib/sdl2-2.0.10

# -Wl,-subsystem,windows gets rid of the console window
# gcc  -o main.exe main.c -lmingw32 -Wl,-subsystem,windows -L./lib -lSDL2main -lSDL2
# -mwindows 关闭控制台窗口
# -lwinhttp http通信库
build:
	$(CC) $(CFLAGS) \
	src/main.c -o bin/main.exe \
	$^ $(UNICORN) -lpthread -lm -lws2_32 -lmingw32 -L$(SDL2)/lib -lSDL2main -lSDL2
