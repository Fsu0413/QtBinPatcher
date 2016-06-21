# QtBinPatcher
Tool for patching paths in binary and text files of Qt Framework

## Building
CMake is used to build this project.

A compiler that supports C++11 is required.

Instructions: (note: ~ stands for the project directory.)


| Unix-like                   | Windows                                 | Note                                                              |
|-----------------------------|-----------------------------------------|-------------------------------------------------------------------|
| cd ~                        | cd ~                                    |                                                                   |
| mkdir build                 | md build                                |                                                                   |
| cd build                    | cd build                                |                                                                   |
| cmake ..                    | cmake .. -G "NMake Makefiles JOM"       | Windows: or -G "MinGW Makefiles" if MinGW is used                 |
| make                        | nmake                                   | Windows: jom can also be used. Use mingw32-make if MinGW is used  |

and the qtbinpatcher(.exe) executable is generated in the build directory.

## How To Use

| Unix-like                   | Windows                                 |
|-----------------------------|-----------------------------------------|
| cp qtbinpatcher /path/to/qt | copy /y qtbinpatcher.exe X:\path\to\qt  |
| cd /path/to/qt              | X:\ && cd \path\to\qt                   |
| ./qtbinpatcher              | qtbinpatcher                            |

