CC = g++
FLAGS = -Wall

# SRCS = $(wildcard *.cpp)
# OBJS = $(SRCS:.cpp=.o)

# all: $(OBJS)
# 	$(CC) $(FLAGS) -o $@ $^
# 	rm -f *.o	

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

test: test.o
	$(CC) $(FLAGS) test.o -o $@ 

all: main.o simulation.o io.o
	$(CC) $(FLAGS) main.o simulation.o io.o -o pack.exe

opt:
	$(CC) -Wall -O3 -mstackrealign -static -static-libgcc -static-libstdc++ main.cpp simulation.cpp io.cpp

clean:
	rm *.o