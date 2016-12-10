import qbs 1.0

Project {
    minimumQbsVersion: 1.6
    
    CppApplication {
        name: "qtbinpatcher"
        cpp.minimumWindowsVersion: "6.1"
        
        Group {
            files: [
                "main.cpp",
                "Backup.cpp",
                "CmdLineChecker.cpp",
                "CmdLineParser.cpp",
                "CommonTypes.cpp",
                "Functions.cpp",
                "Logger.cpp",
                "QMake.cpp",
                "QtBinPatcher.cpp",

                "Backup.hpp",
                "CmdLineChecker.hpp",
                "CmdLineOptions.hpp",
                "CmdLineParser.hpp",
                "CommonTypes.hpp",
                "Functions.hpp",
                "Logger.hpp",
                "QMake.hpp",
                "QtBinPatcher.hpp"
            ]
        }

        cpp.cxxLanguageVersion: "c++11"

        Group {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain && qbs.toolchain.contains("mingw")
            files: [
                "QtBinPatcher.win.rc"
            ]
        }

        Properties {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain && qbs.toolchain.contains("mingw")
            cpp.generateManifestFile: false
            cpp.linkerFlags: [
                "-static",
                "-s"
            ]
        }

        Properties {
            condition: qbs.targetOS.contains("windows") && qbs.toolchain && qbs.toolchain.contains("msvc") && cpp.compilerVersionMajor == 19 && cpp.compilerVersionMinor == 0 && cpp.compilerVersionPatch >= 24000
            cpp.cxxFlags: [
                "/d2SSAOptimizer-"
            ]
        }

        Properties {
            condition: qbs.targetOS.contains("windows")
            cpp.defines: [
                "OS_WINDOWS",
                "_CRT_SECURE_NO_WARNINGS"
            ]
            cpp.runtimeLibrary: "static"
        }

        Properties {
            condition: qbs.targetOS.contains("macos") || qbs.targetOS.contains("osx") || qbs.targetOS.contains("darwin")
            cpp.defines: [
                "OS_MACOS"
            ]
        }

        Properties {
            condition: qbs.targetOS.contains("linux") || qbs.targetOS.contains("freebsd")
            cpp.defines: [
                "OS_LINUX"
            ]
            cpp.linkerFlags: [
                "-s"
            ]
        }
    }
}
