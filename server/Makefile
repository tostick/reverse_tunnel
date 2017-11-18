OBJ	:=$(patsubst %.c, %.o, $(wildcard *.c))
CC := gcc
CFLAGS := -g -Wall 
LIBS := -pthread -levent -lcrypt ../libs/cli-lib/libzebra.a
TARGET := server

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
clean:
	@rm -rf *.o $(TARGET)
