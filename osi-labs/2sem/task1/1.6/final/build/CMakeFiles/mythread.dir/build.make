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
CMAKE_SOURCE_DIR = /home/durachok/workspace/osi-labs/task1/1.6/final

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/durachok/workspace/osi-labs/task1/1.6/final/build

# Include any dependencies generated for this target.
include CMakeFiles/mythread.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/mythread.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/mythread.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/mythread.dir/flags.make

CMakeFiles/mythread.dir/mythread.c.o: CMakeFiles/mythread.dir/flags.make
CMakeFiles/mythread.dir/mythread.c.o: ../mythread.c
CMakeFiles/mythread.dir/mythread.c.o: CMakeFiles/mythread.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/durachok/workspace/osi-labs/task1/1.6/final/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/mythread.dir/mythread.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/mythread.dir/mythread.c.o -MF CMakeFiles/mythread.dir/mythread.c.o.d -o CMakeFiles/mythread.dir/mythread.c.o -c /home/durachok/workspace/osi-labs/task1/1.6/final/mythread.c

CMakeFiles/mythread.dir/mythread.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/mythread.dir/mythread.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/durachok/workspace/osi-labs/task1/1.6/final/mythread.c > CMakeFiles/mythread.dir/mythread.c.i

CMakeFiles/mythread.dir/mythread.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/mythread.dir/mythread.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/durachok/workspace/osi-labs/task1/1.6/final/mythread.c -o CMakeFiles/mythread.dir/mythread.c.s

# Object files for target mythread
mythread_OBJECTS = \
"CMakeFiles/mythread.dir/mythread.c.o"

# External object files for target mythread
mythread_EXTERNAL_OBJECTS =

libmythread.so: CMakeFiles/mythread.dir/mythread.c.o
libmythread.so: CMakeFiles/mythread.dir/build.make
libmythread.so: CMakeFiles/mythread.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/durachok/workspace/osi-labs/task1/1.6/final/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C shared library libmythread.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mythread.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/mythread.dir/build: libmythread.so
.PHONY : CMakeFiles/mythread.dir/build

CMakeFiles/mythread.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/mythread.dir/cmake_clean.cmake
.PHONY : CMakeFiles/mythread.dir/clean

CMakeFiles/mythread.dir/depend:
	cd /home/durachok/workspace/osi-labs/task1/1.6/final/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/durachok/workspace/osi-labs/task1/1.6/final /home/durachok/workspace/osi-labs/task1/1.6/final /home/durachok/workspace/osi-labs/task1/1.6/final/build /home/durachok/workspace/osi-labs/task1/1.6/final/build /home/durachok/workspace/osi-labs/task1/1.6/final/build/CMakeFiles/mythread.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/mythread.dir/depend

