#CC specifies which compiler we're using
CXX = clang++

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -g -Wall -Wextra -Wpedantic

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I. -I../../SDL_files/include -I../headers -I../../source

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L../../SDL_files/lib

#SRCS is all the .cpp files in the current directory
SRCS := $(wildcard *.cpp) 

#OBJS is all the .o files that will be created from SRCS
OBJS := $(patsubst %.cpp,%.o,$(SRCS)) 

# The name of the target we want relies on the object files
all: $(OBJS) 

# make a list of dependency files from the .o files
DEPS := $(OBJS:.o=.d)

-include $(DEPS)

# This specifies a static pattern rule because we have multiple targets that 
# should each be compiled separately and rely only on the according .o file
# -MMD is a compiler flag that generates rules for make for dependent include files
$(OBJS): %.o: %.cpp
	$(CXX) -MMD -c -std=c++11 $(INCLUDE_PATHS) $(COMPILER_FLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS)
