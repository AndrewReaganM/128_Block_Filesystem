# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/andrewm/dev/GitHub/4

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/andrewm/dev/GitHub/4

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/andrewm/dev/GitHub/4/CMakeFiles /home/andrewm/dev/GitHub/4/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/andrewm/dev/GitHub/4/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named zformat

# Build rule for target.
zformat: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 zformat
.PHONY : zformat

# fast build rule for target.
zformat/fast:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/build
.PHONY : zformat/fast

#=============================================================================
# Target rules for targets named zfilez

# Build rule for target.
zfilez: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 zfilez
.PHONY : zfilez

# fast build rule for target.
zfilez/fast:
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/build
.PHONY : zfilez/fast

#=============================================================================
# Target rules for targets named zinspect

# Build rule for target.
zinspect: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 zinspect
.PHONY : zinspect

# fast build rule for target.
zinspect/fast:
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/build
.PHONY : zinspect/fast

#=============================================================================
# Target rules for targets named zrmdir

# Build rule for target.
zrmdir: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 zrmdir
.PHONY : zrmdir

# fast build rule for target.
zrmdir/fast:
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/build
.PHONY : zrmdir/fast

#=============================================================================
# Target rules for targets named zmkdir

# Build rule for target.
zmkdir: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 zmkdir
.PHONY : zmkdir

# fast build rule for target.
zmkdir/fast:
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/build
.PHONY : zmkdir/fast

#=============================================================================
# Target rules for targets named ztouch

# Build rule for target.
ztouch: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 ztouch
.PHONY : ztouch

# fast build rule for target.
ztouch/fast:
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/build
.PHONY : ztouch/fast

oufs_lib.o: oufs_lib.c.o

.PHONY : oufs_lib.o

# target to build an object file
oufs_lib.c.o:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/oufs_lib.c.o
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/oufs_lib.c.o
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/oufs_lib.c.o
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/oufs_lib.c.o
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/oufs_lib.c.o
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/oufs_lib.c.o
.PHONY : oufs_lib.c.o

oufs_lib.i: oufs_lib.c.i

.PHONY : oufs_lib.i

# target to preprocess a source file
oufs_lib.c.i:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/oufs_lib.c.i
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/oufs_lib.c.i
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/oufs_lib.c.i
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/oufs_lib.c.i
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/oufs_lib.c.i
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/oufs_lib.c.i
.PHONY : oufs_lib.c.i

oufs_lib.s: oufs_lib.c.s

.PHONY : oufs_lib.s

# target to generate assembly for a file
oufs_lib.c.s:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/oufs_lib.c.s
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/oufs_lib.c.s
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/oufs_lib.c.s
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/oufs_lib.c.s
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/oufs_lib.c.s
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/oufs_lib.c.s
.PHONY : oufs_lib.c.s

oufs_lib_support.o: oufs_lib_support.c.o

.PHONY : oufs_lib_support.o

# target to build an object file
oufs_lib_support.c.o:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/oufs_lib_support.c.o
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/oufs_lib_support.c.o
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/oufs_lib_support.c.o
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/oufs_lib_support.c.o
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/oufs_lib_support.c.o
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/oufs_lib_support.c.o
.PHONY : oufs_lib_support.c.o

oufs_lib_support.i: oufs_lib_support.c.i

.PHONY : oufs_lib_support.i

# target to preprocess a source file
oufs_lib_support.c.i:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/oufs_lib_support.c.i
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/oufs_lib_support.c.i
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/oufs_lib_support.c.i
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/oufs_lib_support.c.i
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/oufs_lib_support.c.i
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/oufs_lib_support.c.i
.PHONY : oufs_lib_support.c.i

oufs_lib_support.s: oufs_lib_support.c.s

.PHONY : oufs_lib_support.s

# target to generate assembly for a file
oufs_lib_support.c.s:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/oufs_lib_support.c.s
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/oufs_lib_support.c.s
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/oufs_lib_support.c.s
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/oufs_lib_support.c.s
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/oufs_lib_support.c.s
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/oufs_lib_support.c.s
.PHONY : oufs_lib_support.c.s

vdisk.o: vdisk.c.o

.PHONY : vdisk.o

# target to build an object file
vdisk.c.o:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/vdisk.c.o
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/vdisk.c.o
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/vdisk.c.o
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/vdisk.c.o
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/vdisk.c.o
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/vdisk.c.o
.PHONY : vdisk.c.o

vdisk.i: vdisk.c.i

.PHONY : vdisk.i

# target to preprocess a source file
vdisk.c.i:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/vdisk.c.i
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/vdisk.c.i
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/vdisk.c.i
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/vdisk.c.i
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/vdisk.c.i
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/vdisk.c.i
.PHONY : vdisk.c.i

vdisk.s: vdisk.c.s

.PHONY : vdisk.s

# target to generate assembly for a file
vdisk.c.s:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/vdisk.c.s
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/vdisk.c.s
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/vdisk.c.s
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/vdisk.c.s
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/vdisk.c.s
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/vdisk.c.s
.PHONY : vdisk.c.s

zfilez.o: zfilez.c.o

.PHONY : zfilez.o

# target to build an object file
zfilez.c.o:
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/zfilez.c.o
.PHONY : zfilez.c.o

zfilez.i: zfilez.c.i

.PHONY : zfilez.i

# target to preprocess a source file
zfilez.c.i:
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/zfilez.c.i
.PHONY : zfilez.c.i

zfilez.s: zfilez.c.s

.PHONY : zfilez.s

# target to generate assembly for a file
zfilez.c.s:
	$(MAKE) -f CMakeFiles/zfilez.dir/build.make CMakeFiles/zfilez.dir/zfilez.c.s
.PHONY : zfilez.c.s

zformat.o: zformat.c.o

.PHONY : zformat.o

# target to build an object file
zformat.c.o:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/zformat.c.o
.PHONY : zformat.c.o

zformat.i: zformat.c.i

.PHONY : zformat.i

# target to preprocess a source file
zformat.c.i:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/zformat.c.i
.PHONY : zformat.c.i

zformat.s: zformat.c.s

.PHONY : zformat.s

# target to generate assembly for a file
zformat.c.s:
	$(MAKE) -f CMakeFiles/zformat.dir/build.make CMakeFiles/zformat.dir/zformat.c.s
.PHONY : zformat.c.s

zinspect.o: zinspect.c.o

.PHONY : zinspect.o

# target to build an object file
zinspect.c.o:
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/zinspect.c.o
.PHONY : zinspect.c.o

zinspect.i: zinspect.c.i

.PHONY : zinspect.i

# target to preprocess a source file
zinspect.c.i:
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/zinspect.c.i
.PHONY : zinspect.c.i

zinspect.s: zinspect.c.s

.PHONY : zinspect.s

# target to generate assembly for a file
zinspect.c.s:
	$(MAKE) -f CMakeFiles/zinspect.dir/build.make CMakeFiles/zinspect.dir/zinspect.c.s
.PHONY : zinspect.c.s

zmkdir.o: zmkdir.c.o

.PHONY : zmkdir.o

# target to build an object file
zmkdir.c.o:
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/zmkdir.c.o
.PHONY : zmkdir.c.o

zmkdir.i: zmkdir.c.i

.PHONY : zmkdir.i

# target to preprocess a source file
zmkdir.c.i:
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/zmkdir.c.i
.PHONY : zmkdir.c.i

zmkdir.s: zmkdir.c.s

.PHONY : zmkdir.s

# target to generate assembly for a file
zmkdir.c.s:
	$(MAKE) -f CMakeFiles/zmkdir.dir/build.make CMakeFiles/zmkdir.dir/zmkdir.c.s
.PHONY : zmkdir.c.s

zrmdir.o: zrmdir.c.o

.PHONY : zrmdir.o

# target to build an object file
zrmdir.c.o:
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/zrmdir.c.o
.PHONY : zrmdir.c.o

zrmdir.i: zrmdir.c.i

.PHONY : zrmdir.i

# target to preprocess a source file
zrmdir.c.i:
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/zrmdir.c.i
.PHONY : zrmdir.c.i

zrmdir.s: zrmdir.c.s

.PHONY : zrmdir.s

# target to generate assembly for a file
zrmdir.c.s:
	$(MAKE) -f CMakeFiles/zrmdir.dir/build.make CMakeFiles/zrmdir.dir/zrmdir.c.s
.PHONY : zrmdir.c.s

ztouch.o: ztouch.c.o

.PHONY : ztouch.o

# target to build an object file
ztouch.c.o:
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/ztouch.c.o
.PHONY : ztouch.c.o

ztouch.i: ztouch.c.i

.PHONY : ztouch.i

# target to preprocess a source file
ztouch.c.i:
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/ztouch.c.i
.PHONY : ztouch.c.i

ztouch.s: ztouch.c.s

.PHONY : ztouch.s

# target to generate assembly for a file
ztouch.c.s:
	$(MAKE) -f CMakeFiles/ztouch.dir/build.make CMakeFiles/ztouch.dir/ztouch.c.s
.PHONY : ztouch.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... rebuild_cache"
	@echo "... zformat"
	@echo "... zfilez"
	@echo "... zinspect"
	@echo "... edit_cache"
	@echo "... zrmdir"
	@echo "... zmkdir"
	@echo "... ztouch"
	@echo "... oufs_lib.o"
	@echo "... oufs_lib.i"
	@echo "... oufs_lib.s"
	@echo "... oufs_lib_support.o"
	@echo "... oufs_lib_support.i"
	@echo "... oufs_lib_support.s"
	@echo "... vdisk.o"
	@echo "... vdisk.i"
	@echo "... vdisk.s"
	@echo "... zfilez.o"
	@echo "... zfilez.i"
	@echo "... zfilez.s"
	@echo "... zformat.o"
	@echo "... zformat.i"
	@echo "... zformat.s"
	@echo "... zinspect.o"
	@echo "... zinspect.i"
	@echo "... zinspect.s"
	@echo "... zmkdir.o"
	@echo "... zmkdir.i"
	@echo "... zmkdir.s"
	@echo "... zrmdir.o"
	@echo "... zrmdir.i"
	@echo "... zrmdir.s"
	@echo "... ztouch.o"
	@echo "... ztouch.i"
	@echo "... ztouch.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

