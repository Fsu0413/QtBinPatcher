TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = qtbinpatcher

SOURCES += main.cpp \
    Backup.cpp \
    CmdLineChecker.cpp \
    CmdLineParser.cpp \
    CommonTypes.cpp \
    Functions.cpp \
    Logger.cpp \
    QMake.cpp \
    QtBinPatcher.cpp

HEADERS += \
    Backup.hpp \
    CmdLineChecker.hpp \
    CmdLineOptions.hpp \
    CmdLineParser.hpp \
    CommonTypes.hpp \
    Functions.hpp \
    Logger.hpp \
    QMake.hpp \
    QtBinPatcher.hpp

win32 {
    !win32-msvc*: RESOURCES += QtBinPatcher.win.rc

    win32-g++ {
        QMAKE_LFLAGS_RELEASE += -static -static-libgcc -static-libstdc++
    }
    win32-msvc* {
        QMAKE_CXXFLAGS -= -MD
        QMAKE_CXXFLAGS += -MT
        QMAKE_CXXFLAGS_RELEASE -= -MD
        QMAKE_CXXFLAGS_RELEASE += -MT
    }

    # win32-clang-msvc2015: ???
}

win32: DEFINES += OS_WINDOWS _CRT_SECURE_NO_WARNINGS
linux: DEFINES += OS_LINUX
osx:   DEFINES += OS_MACOS

!win32: !linux: !osx: error("Unsupported operating system!")

!CONFIG(debug): DEFINES += NDEBUG
CONFIG(qtcross): DEFINES += QTCROSS
