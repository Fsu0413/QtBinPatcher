/*******************************************************************************

         Yuri V. Krugloff. 2013-2015. http://www.tver-soft.org

    This is free and unencumbered software released into the public domain.

    Anyone is free to copy, modify, publish, use, compile, sell, or
    distribute this software, either in source code form or as a compiled
    binary, for any purpose, commercial or non-commercial, and by any
    means.

    In jurisdictions that recognize copyright laws, the author or authors
    of this software dedicate any and all copyright interest in the
    software to the public domain. We make this dedication for the benefit
    of the public at large and to the detriment of our heirs and
    successors. We intend this dedication to be an overt act of
    relinquishment in perpetuity of all present and future rights to this
    software under copyright law.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

    For more information, please refer to <http://unlicense.org/>

*******************************************************************************/

#include <string>

#include "Logger.hpp"
#include "Functions.hpp"
#include "CmdLineOptions.hpp"
#include "CmdLineParser.hpp"
#include "CmdLineChecker.hpp"
#include "QtBinPatcher.hpp"

//------------------------------------------------------------------------------

void howToUseMessage()
{
    printf(
        //.......|.........|.........|.........|.........|.........|.........|.........|
        "\n"
        "Usage: qtbinpatcher [options]\n"
        "Options:\n"
        "  --version      Show program version and exit.\n"
        "  --help         Show this help and exit.\n"
        "  --verbose      Print extended runtime information.\n"
        "  --logfile=name Duplicate messages into logfile with name \"name\".\n"
        "  --backup       Create and save backup for files that will be patched.\n"
        "                 This option incompatible with option \"--nobackup\".\n"
        "  --nobackup     Don't create backup files in patch process.\n"
        "                 This option incompatible with option \"--backup\".\n"
        "                 WARNING: If an error occurs during patching, Qt library can be\n"
        "                          permanently damaged!\n"
        "  --force        Force patching (without old path actuality checking).\n"
        "  --qt-dir=path  Directory, where Qt or qmake is now located (may be relative).\n"
        "                 If not specified, will be used current directory. Patcher will\n"
        "                 search qmake first in directory \"path\", and then in its subdir\n"
        "                 \"bin\". Patcher is NEVER looking qmake in other directories.\n"
        "                 WARNING: If nonstandard directory for binary files is used\n"
        "                          (not \"bin\"), select directory where located qmake.\n"
        "  --new-dir=path Directory where Qt will be located (may be relative).\n"
        "                 If not specified, will be used the current location.\n"
        "  --old-dir=path Directory where Qt was located. This option can be specified\n"
        "                 more then once. This path will be replaced only in text files.\n"
        "  --dry-run      Output the procedure only, do not really process the jobs.\n"
        "\n"
        "Remark.\n"
        "  If missing \"--backup\" and \"--nobackup\" options, the backup files will be\n"
        "  created before patching and deleted after successful completion of the\n"
        "  patching or restored if an error occurs.\n"
        "\n"
        //.......|.........|.........|.........|.........|.........|.........|.........|
    );
}

//------------------------------------------------------------------------------

int main(int argc, const char* argv[])
{
    const char *description = "\n"
        "QtBinPatcher v2.3.0Beta. Tool for patching paths in Qt binaries.\n"
        "Yuri V. Krugloff, 2013-2015. http://www.tver-soft.org\n"
        "Frank Su, 2016. http://mogara.org\n"
        "This is free software released into the public domain.\n"
#ifdef QTCROSS
        "This binary is built for cross compiled Qt.\n"
#endif
        "\n"
    ;

    LOG(description);

    TCmdLineParser CmdLineParser(argc, argv);
    if (CmdLineParser.hasError()) {
        LOG("%s\n", CmdLineParser.errorString().c_str());
        howToUseMessage();
        return -1;
    }
    const TStringListMap& argsMap = CmdLineParser.argsMap();


    std::string ErrorString = TCmdLineChecker::check(argsMap);
    if (!ErrorString.empty()) {
        LOG("%s\n", ErrorString.c_str());
        howToUseMessage();
        return -1;
    }


    TLogger::setVerbose(argsMap.contains(OPT_VERBOSE));
    LOG_SET_FILENAME(argsMap.value(OPT_LOGFILE).c_str());
    LOG_V("%s\n", CmdLineParser.dump().c_str());
    LOG_V("Working directory: \"%s\".\n", Functions::currentDir().c_str());
    LOG_V("Binary file location: \"%s\".\n", argv[0]);

    if (argsMap.contains(OPT_HELP)) {
        howToUseMessage();
        return 0;
    }

    if (argsMap.contains(OPT_VERSION))
        return 0;

    return TQtBinPatcher::exec(argsMap) ? 0 : -1;
}

//------------------------------------------------------------------------------
