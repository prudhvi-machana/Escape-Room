CXX = g++
CXXFLAGS = -Wall
LIBS = -lGL -lGLU -lglut

SRCS = main.cpp room/room.cpp camera/camera.cpp utils/utils.cpp \
	objects/object_utils.cpp objects/table.cpp objects/chair.cpp \
	objects/dustbin.cpp objects/bookshelf.cpp objects/bed.cpp \
	objects/painting.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o escape_room $(LIBS)

clean:
	rm -f escape_room
