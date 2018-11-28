cmake_minimum_required(VERSION 3.10)
project(4 C)

set(CMAKE_C_STANDARD 11)



add_executable(zinspect oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zinspect.c oufs_lib.c)
add_executable(zformat oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h zformat.c)
add_executable(zmkdir zmkdir.c oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h)
add_executable(zrmdir zrmdir.c oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h)
add_executable(zfilez zfilez.c oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h)
add_executable(ztouch ztouch.c oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h)
add_executable(zcreate zcreate.c oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h)
add_executable(zappend zappend.c oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h)
add_executable(zmore zmore.c oufs_lib.h oufs_lib_support.c oufs.h vdisk.c oufs_lib.c zformat.h)



target_compile_options(zformat PRIVATE -fstack-protector)