#*******************************************************************************
#
#        Yuri V. Krugloff. 2013-2015. http://www.tver-soft.org
#
#   This is free and unencumbered software released into the public domain.
#
#   Anyone is free to copy, modify, publish, use, compile, sell, or
#   distribute this software, either in source code form or as a compiled
#   binary, for any purpose, commercial or non-commercial, and by any
#   means.
#
#   In jurisdictions that recognize copyright laws, the author or authors
#   of this software dedicate any and all copyright interest in the
#   software to the public domain. We make this dedication for the benefit
#   of the public at large and to the detriment of our heirs and
#   successors. We intend this dedication to be an overt act of
#   relinquishment in perpetuity of all present and future rights to this
#   software under copyright law.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
#   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#   OTHER DEALINGS IN THE SOFTWARE.
#
#   For more information, please refer to <http://unlicense.org/>
#
#*******************************************************************************

project(qtbinpatcher)
cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Possible type of build: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

set(SRC_LIST
    CommonTypes.cpp    CommonTypes.hpp
    Logger.cpp         Logger.hpp
    Functions.cpp      Functions.hpp
                       CmdLineOptions.hpp
    CmdLineParser.cpp  CmdLineParser.hpp
    CmdLineChecker.cpp CmdLineChecker.hpp
    QMake.cpp          QMake.hpp
    Backup.cpp         Backup.hpp
    QtBinPatcher.cpp   QtBinPatcher.hpp
    main.cpp
)

message("Current operating system: ${CMAKE_SYSTEM_NAME}")

set(QTBINPATCHER_GENERIC_UNIXLIKE_SYSTEM "[(Linux)(FreeBSD)]")

if(WIN32)
    add_definitions(-DOS_WINDOWS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # MSVC generate manifest file itself.
    if(NOT MSVC)
        set(SRC_LIST ${SRC_LIST} QtBinPatcher.win.rc)
    endif()

    if(MINGW)
        set(CMAKE_CXX_FLAGS_RELEASE "-static -static-libgcc -static-libstdc++ -Wl,-s ${CMAKE_CXX_FLAGS_RELEASE}")
    endif()

    if(MSVC)
        # Changing flags for static linking.
        foreach(flag_var CMAKE_CXX_FLAGS
                         CMAKE_CXX_FLAGS_RELEASE
                         CMAKE_CXX_FLAGS_MINSIZEREL)
            if(${flag_var} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
            endif()
        endforeach()
    endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_definitions(-DOS_MACOS)
else()
    # We treat all other OS as Linux. But....
    # Not all OSs are compatible on file names with Linux, so we should add a warning here
    if(NOT ${CMAKE_SYSTEM_NAME} MATCHES ${QTBINPATCHER_GENERIC_UNIXLIKE_SYSTEM})
        message(FATAL_ERROR "QtBinPatcher has not tested with your OS, please modify CMakeLists.txt if you are sure with your configuration.")
    endif()

    # Do what we should do on Linux.
    add_definitions(-DOS_LINUX)
    # Uncomment lines below for static linking.
    #if(CMAKE_COMPILER_IS_GNUCC)
    #    set(CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-s ${CMAKE_CXX_FLAGS}")
    #endif()
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DNDEBUG)
endif()

if(QTBINPATCHER_QTCROSS)
    add_definitions(-DQTCROSS)
endif()

add_executable(${PROJECT_NAME} ${SRC_LIST})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
