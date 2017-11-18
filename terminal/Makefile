OBJ	:=$(patsubst %.c, %.o, $(wildcard *.c))
CC := gcc
CFLAGS := -g -Wall 
LIBS := -pthread -levent -lreadline
TARGET := terminal

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
clean:
	@rm -rf *.o $(TARGET)
