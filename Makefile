COMPILER=g++
FLAGS=-Wall -O2
SOURCES=./src/*.cpp
EXEC=router
RTABLE="rtable.txt"

build:
	$(COMPILER) $(FLAGS) -o $(EXEC) $(SOURCES)

run: build
	./$(EXEC) $(RTABLE)

clean:
	rm -f $(EXEC) *.o
