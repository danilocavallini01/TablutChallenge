# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/danilo/Documents/Github/TablutChallenge

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/danilo/Documents/Github/TablutChallenge

# Include any dependencies generated for this target.
include CMakeFiles/tablutChallange.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tablutChallange.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tablutChallange.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tablutChallange.dir/flags.make

CMakeFiles/tablutChallange.dir/main.cpp.o: CMakeFiles/tablutChallange.dir/flags.make
CMakeFiles/tablutChallange.dir/main.cpp.o: main.cpp
CMakeFiles/tablutChallange.dir/main.cpp.o: CMakeFiles/tablutChallange.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/danilo/Documents/Github/TablutChallenge/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tablutChallange.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tablutChallange.dir/main.cpp.o -MF CMakeFiles/tablutChallange.dir/main.cpp.o.d -o CMakeFiles/tablutChallange.dir/main.cpp.o -c /home/danilo/Documents/Github/TablutChallenge/main.cpp

CMakeFiles/tablutChallange.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/tablutChallange.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/danilo/Documents/Github/TablutChallenge/main.cpp > CMakeFiles/tablutChallange.dir/main.cpp.i

CMakeFiles/tablutChallange.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/tablutChallange.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/danilo/Documents/Github/TablutChallenge/main.cpp -o CMakeFiles/tablutChallange.dir/main.cpp.s

# Object files for target tablutChallange
tablutChallange_OBJECTS = \
"CMakeFiles/tablutChallange.dir/main.cpp.o"

# External object files for target tablutChallange
tablutChallange_EXTERNAL_OBJECTS =

tablutChallange: CMakeFiles/tablutChallange.dir/main.cpp.o
tablutChallange: CMakeFiles/tablutChallange.dir/build.make
tablutChallange: /home/danilo/libtorch/lib/libtorch.so
tablutChallange: /home/danilo/libtorch/lib/libc10.so
tablutChallange: /home/danilo/libtorch/lib/libkineto.a
tablutChallange: /home/danilo/libtorch/lib/libc10.so
tablutChallange: CMakeFiles/tablutChallange.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/danilo/Documents/Github/TablutChallenge/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable tablutChallange"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tablutChallange.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tablutChallange.dir/build: tablutChallange
.PHONY : CMakeFiles/tablutChallange.dir/build

CMakeFiles/tablutChallange.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tablutChallange.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tablutChallange.dir/clean

CMakeFiles/tablutChallange.dir/depend:
	cd /home/danilo/Documents/Github/TablutChallenge && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/danilo/Documents/Github/TablutChallenge /home/danilo/Documents/Github/TablutChallenge /home/danilo/Documents/Github/TablutChallenge /home/danilo/Documents/Github/TablutChallenge /home/danilo/Documents/Github/TablutChallenge/CMakeFiles/tablutChallange.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tablutChallange.dir/depend

