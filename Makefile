CC = C:/MinGW/bin/gcc.exe
CFLAGS = -Wall -Wextra -O2 -Iinclude
TARGET = route_optimizer.exe

SRCS = main.c src/graph.c src/mst.c src/knapsack.c src/tsp.c src/benchmark.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q *.o src\*.o $(TARGET) 2>nul || true

run: all
	./$(TARGET)
