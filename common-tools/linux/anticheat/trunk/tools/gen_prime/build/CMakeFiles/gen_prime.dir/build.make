# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/anticheater/anticheat/tools/gen_prime

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/anticheater/anticheat/tools/gen_prime/build

# Include any dependencies generated for this target.
include CMakeFiles/gen_prime.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/gen_prime.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/gen_prime.dir/flags.make

CMakeFiles/gen_prime.dir/gen_prime.c.o: CMakeFiles/gen_prime.dir/flags.make
CMakeFiles/gen_prime.dir/gen_prime.c.o: ../gen_prime.c
	$(CMAKE_COMMAND) -E cmake_progress_report /root/anticheater/anticheat/tools/gen_prime/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object CMakeFiles/gen_prime.dir/gen_prime.c.o"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/gen_prime.dir/gen_prime.c.o   -c ../gen_prime.c

CMakeFiles/gen_prime.dir/gen_prime.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/gen_prime.dir/gen_prime.c.i"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E ../gen_prime.c > CMakeFiles/gen_prime.dir/gen_prime.c.i

CMakeFiles/gen_prime.dir/gen_prime.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/gen_prime.dir/gen_prime.c.s"
	/usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S ../gen_prime.c -o CMakeFiles/gen_prime.dir/gen_prime.c.s

CMakeFiles/gen_prime.dir/gen_prime.c.o.requires:
.PHONY : CMakeFiles/gen_prime.dir/gen_prime.c.o.requires

CMakeFiles/gen_prime.dir/gen_prime.c.o.provides: CMakeFiles/gen_prime.dir/gen_prime.c.o.requires
	$(MAKE) -f CMakeFiles/gen_prime.dir/build.make CMakeFiles/gen_prime.dir/gen_prime.c.o.provides.build
.PHONY : CMakeFiles/gen_prime.dir/gen_prime.c.o.provides

CMakeFiles/gen_prime.dir/gen_prime.c.o.provides.build: CMakeFiles/gen_prime.dir/gen_prime.c.o
.PHONY : CMakeFiles/gen_prime.dir/gen_prime.c.o.provides.build

# Object files for target gen_prime
gen_prime_OBJECTS = \
"CMakeFiles/gen_prime.dir/gen_prime.c.o"

# External object files for target gen_prime
gen_prime_EXTERNAL_OBJECTS =

gen_prime: CMakeFiles/gen_prime.dir/gen_prime.c.o
gen_prime: CMakeFiles/gen_prime.dir/build.make
gen_prime: CMakeFiles/gen_prime.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable gen_prime"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gen_prime.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/gen_prime.dir/build: gen_prime
.PHONY : CMakeFiles/gen_prime.dir/build

CMakeFiles/gen_prime.dir/requires: CMakeFiles/gen_prime.dir/gen_prime.c.o.requires
.PHONY : CMakeFiles/gen_prime.dir/requires

CMakeFiles/gen_prime.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/gen_prime.dir/cmake_clean.cmake
.PHONY : CMakeFiles/gen_prime.dir/clean

CMakeFiles/gen_prime.dir/depend:
	cd /root/anticheater/anticheat/tools/gen_prime/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/anticheater/anticheat/tools/gen_prime /root/anticheater/anticheat/tools/gen_prime /root/anticheater/anticheat/tools/gen_prime/build /root/anticheater/anticheat/tools/gen_prime/build /root/anticheater/anticheat/tools/gen_prime/build/CMakeFiles/gen_prime.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/gen_prime.dir/depend

