CC=g++
CFLAGS=-c -Wall
INCLUDE += -I./include/opencv4
LDFLAGS += -L./lib `pkg-config --libs opencv`

SOURCES=dct.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=dct

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS) 

.cpp.o:
	$(CC) $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	rm -rf *o $(EXECUTABLE)