CC = g++
FLAGS = -Wall -Wextra -O3

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

all: $(OBJS)
	$(CC) $(FLAGS) -o $@ $^
	rm -f *.o

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@


