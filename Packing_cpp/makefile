CC = g++
FLAGS = -Wall -O3

# SRCS = $(wildcard *.cpp)
# OBJS = $(SRCS:.cpp=.o)

# all: $(OBJS)
# 	$(CC) $(FLAGS) -o $@ $^
# 	rm -f *.o	

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

test: test.o
	$(CC) $(FLAGS) test.o -o $@ 

clean:
	rm *.o