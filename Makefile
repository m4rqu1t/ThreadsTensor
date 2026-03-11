CC = gcc
CFLAGS = -Wall -Wextra -g -pthread -fopenmp
TARGET = tensor

SRCS = main.c tensor.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET) tensor_maolf.out
