CC     = gcc
RAYINC = C:/Users/gagan/raylib/raylib-5.0_win32_mingw-w64/include
RAYLIB = C:/Users/gagan/raylib/raylib-5.0_win32_mingw-w64/lib

CFLAGS  = -Wall -Wextra -O2 -Iinclude -I$(RAYINC)
LDFLAGS = -L$(RAYLIB) -lraylib -lopengl32 -lgdi32 -lwinmm

TARGET = route_optimizer.exe
SRCS   = main.c src/graph.c src/mst.c src/knapsack.c \
         src/tsp.c src/benchmark.c src/visualizer.c
OBJS   = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q *.o src\*.o $(TARGET) 2>nul || true

run: all
	./$(TARGET)
