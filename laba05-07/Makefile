CC = g++
CFLAGS = -lzmq -w

all: main child

main: parent.cpp
	$(CC) parent.cpp $(CFLAGS) -o main

child: child.cpp
	$(CC) child.cpp $(CFLAGS) -o child

clean:
	rm -f main child
