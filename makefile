CXX = g++
CXXFLAGS = -Wall
LIBS = -lGL -lGLU -lglut

SRCS = main.cpp room/room.cpp camera/camera.cpp utils/utils.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o escape_room $(LIBS)

clean:
	rm -f escape_room