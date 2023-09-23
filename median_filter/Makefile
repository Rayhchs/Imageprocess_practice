CC=g++
CFLAGS=-c -Wall
INCLUDE += -I./include/opencv4
LDFLAGS += -L./lib `pkg-config --libs opencv` -lpthread

SOURCES=median_filter.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=median_filter

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS) 

.cpp.o:
	$(CC) $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	rm -rf *o $(EXECUTABLE)