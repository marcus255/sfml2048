CC=g++
SFMLLIBDIR=/usr/lib/i386-linux-gnu/
LIBS=-L$(SFMLLIBDIR) -lsfml-graphics -lsfml-window -lsfml-system
CFLAGS=-Werror -std=c++17
OBJECTS=main.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o sfml2048 $(LIBS)
.cpp.o:
	$(CC) $(CFLAGS) -c $*.cpp
clean:
	rm -rf *.o
