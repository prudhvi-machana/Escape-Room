CXX = g++
CXXFLAGS = -Wall
LIBS = -lGL -lGLU -lglut

SRCS = main.cpp app/render.cpp room/room.cpp room/room_skybox.cpp \
	room/room_features.cpp room/room_codebox.cpp camera/camera.cpp \
	utils/state.cpp utils/drawing.cpp utils/interactions.cpp utils/hud.cpp \
	objects/object_utils.cpp objects/table.cpp objects/chair.cpp \
	objects/dustbin.cpp objects/bookshelf.cpp objects/bed.cpp \
	objects/painting.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRCS) -o escape_room $(LIBS)

clean:
	rm -f escape_room
