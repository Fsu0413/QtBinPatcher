# QtBinPatcher
Tool for patching paths in binary and text files of Qt Framework

## Building
CMake or qmake is used to build this project.

A compiler that supports C++11 is required.

Instructions for CMake:


| Unix-like           | Windows (in VS command prompt)            | Note                                                              |
|---------------------|-------------------------------------------|-------------------------------------------------------------------|
| cd /path/to/patcher | cd -d X:\path\to\patcher                  |                                                                   |
| mkdir ../build      | md ..\build                               |                                                                   |
| cd ../build         | cd ..\build                               |                                                                   |
| cmake ../patcher    | cmake ..\patcher -G "NMake Makefiles JOM" | Windows: or -G "MinGW Makefiles" if MinGW is used                 |
| make                | nmake                                     | Windows: jom can also be used. Use mingw32-make if MinGW is used  |

and the qtbinpatcher(.exe) executable is generated in the build directory.

Instructions for qmake:

| Unix-like           | Windows (in VS command prompt)        | Note                                                               |
|---------------------|---------------------------------------|--------------------------------------------------------------------|
| cd /path/to/patcher | cd -d X:\path\to\patcher              |                                                                    |
| mkdir ../build      | md ..\build                           |                                                                    |
| cd ../build         | cd ..\build                           |                                                                    |
| qmake ../patcher    | qmake ..\patcher -spec win32-msvc2015 | Windows: change "win32-msvc2015" according to your qt installation |
| make                | nmake                                 | Windows: jom can also be used. Use mingw32-make if MinGW is used   |

## How To Use

| Unix-like                   | Windows                                 |
|-----------------------------|-----------------------------------------|
| cp qtbinpatcher /path/to/qt | copy /y qtbinpatcher.exe X:\path\to\qt  |
| cd /path/to/qt              | X:\ && cd \path\to\qt                   |
| ./qtbinpatcher              | qtbinpatcher                            |

## Notes for a cross-compiled Qt

To use QtBinPatcher for a cross-compiled Qt, note that the cross-compiled target libs should not be patched by this software.  
We use precompiler-macro to deal with this condition.

Add a parameter to CMake/qmake executable to compile a version of QtBinPatcher for cross-compiled Qt, e.g.:  
cmake ../patcher -G "NMake Makefiles JOM" -DQTBINPATCHER_QTCROSS=True  
qmake ../patcher -spec win32-msvc2015 CONFIG+=qtcross  

When build succeed, using "./qtbinpatcher --help"(or "qtbinpatcher.exe --help" for windows) to check if it is built for cross compiled Qt.
