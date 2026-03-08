CC = gcc
CFLAGS = -Wall -Wextra -pthread -I.
TARGET = thread_pool_app
SOURCES = main.c src/thread_pool.c src/task_queue.c src/future.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
