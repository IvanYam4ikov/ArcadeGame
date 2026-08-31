#CC specifies which compiler we're using
CXX = clang++

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -g -Wall -Wextra -Wpedantic

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I. -I../SDL_files/include/ -I../GUI_files/headers

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L../SDL_files/lib

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2main -lSDL2-2.0.0 -lSDL2_image-2.0.0 -lSDL2_ttf-2.0.0 -lSDL2_mixer-2.0.0

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = arcadeSystem

SRCS := $(wildcard *.cpp) 
OBJS := $(patsubst %.cpp,%.o,$(SRCS)) 
# 1 make all depends on ArcadeSystem 
all: $(OBJ_NAME)

# 2 ArcadeSystem depends on the .o files, g++ -o ArcadeSystem *.o
# the .o files come from the wilcard statements
$(OBJ_NAME): $(OBJS) ../GUI_files/cpp_files/*.o
	$(CXX) $^ $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $@ 

# 3 all the .o files rely on the .cpp files
# make typically does this implicitly but we
# only want each .o to rely only on its .cpp
DEPS := $(OBJS:.o=.d)

-include $(DEPS)

$(OBJS): %.o: %.cpp
	$(CXX) -MMD -c -std=c++11 $(INCLUDE_PATHS) $(COMPILER_FLAGS) $< -o $@
clean:
	rm -f $(OBJS) $(DEPS) $(OBJ_NAME)
