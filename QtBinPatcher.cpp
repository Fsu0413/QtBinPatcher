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

#include "QtBinPatcher.hpp"

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <vector>
#include <algorithm>
#include <fstream>

#include "Logger.hpp"
#include "Functions.hpp"
#include "CmdLineOptions.hpp"
#include "CmdLineParser.hpp"
#include "CmdLineChecker.hpp"
#include "QMake.hpp"
#include "Backup.hpp"

//------------------------------------------------------------------------------

using namespace std;
using namespace Functions;

//------------------------------------------------------------------------------

#define QT_PATH_MAX_LEN 450

//------------------------------------------------------------------------------

// Case-insensitive comparision (only for case-independent file systems).

bool caseInsensitiveComp(const char c1, const char c2)
{
    return tolower(c1) == tolower(c2);
}

//------------------------------------------------------------------------------
// String comparision. For OS Windows comparision is case-insensitive.

bool strneq(const string& s1, const string& s2)
{
#ifdef OS_WINDOWS
    string _s1 = s1, _s2 = s2;
    transform(_s1.begin(), _s1.end(), _s1.begin(), ::tolower);
    transform(_s2.begin(), _s2.end(), _s2.begin(), ::tolower);
    return _s1 != _s2;
#else
    return s1 != s2;
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

string TQtBinPatcher::getStartDir() const
{
    string Result = normalizeSeparators(m_ArgsMap.value(OPT_QT_DIR));
    if (!Result.empty())
        Result = absolutePath(Result);
    return Result;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::getQtDir()
{
    m_QtDir = m_QMake.qtPath();
    if (m_QtDir.empty()) {
        LOG_E("Can't determine path to Qt directory.\n");
        return false;
    }

    LOG_V("Path to Qt directory: \"%s\".\n", m_QtDir.c_str());
    return true;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::getNewQtDir()
{
    assert(hasOnlyNormalSeparators(m_QtDir));

    m_NewQtDir = normalizeSeparators(m_ArgsMap.value(OPT_NEW_DIR));
    if (!m_NewQtDir.empty())
        m_NewQtDir = absolutePath(m_NewQtDir);
    else
        m_NewQtDir = m_QtDir;
    LOG_V("Path to new Qt directory: \"%s\".\n", m_NewQtDir.c_str());

    if (m_NewQtDir.length() > QT_PATH_MAX_LEN) {
        LOG_E("Path to new Qt directory too long (%i symbols).\n"
            "Path must be not longer as %i symbols.",
            m_NewQtDir.length(), QT_PATH_MAX_LEN);
        return false;
    }
    return !m_NewQtDir.empty();
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::isPatchNeeded()
{
    assert(hasOnlyNormalSeparators(m_NewQtDir));

    string OldQtDir = normalizeSeparators(m_QMake.qtInstallPrefix());
    if (!OldQtDir.empty() && !m_NewQtDir.empty())
        return strneq(OldQtDir, m_NewQtDir);
    return false;
}

//------------------------------------------------------------------------------

void TQtBinPatcher::addTxtPatchValues(const string& oldPath)
{
    assert(hasOnlyNormalSeparators(oldPath));
    assert(hasOnlyNormalSeparators(m_NewQtDir));

    if (!oldPath.empty()) {
        m_TxtPatchValues[oldPath] = m_NewQtDir;

#ifdef OS_WINDOWS
        string Old = oldPath;
        string New = m_NewQtDir;
        replace(Old.begin(), Old.end(), '/', '\\');
        replace(New.begin(), New.end(), '/', '\\');
        m_TxtPatchValues[Old] = New;

        string NewQtDirDS = m_NewQtDir;
        replace(&NewQtDirDS, '/', "\\\\");
        Old = oldPath;
        replace(&Old, '/', "\\\\");
        m_TxtPatchValues[Old] = NewQtDirDS;
#endif
    }
}

//------------------------------------------------------------------------------

void TQtBinPatcher::createBinPatchValues()
{
    struct TParam
    {
        const char* const Name;
        const char* const Prefix;
    };

    static const TParam Params[] = {
        {"QT_INSTALL_PREFIX", "qt_epfxpath="},  // "QT_EXT_PREFIX"
        {"QT_INSTALL_PREFIX", "qt_prfxpath="},
        {"QT_INSTALL_ARCHDATA", "qt_adatpath="},
        {"QT_INSTALL_DATA", "qt_datapath="},
        {"QT_INSTALL_DOCS", "qt_docspath="},
        {"QT_INSTALL_HEADERS", "qt_hdrspath="},
        {"QT_INSTALL_LIBS", "qt_libspath="},
        {"QT_INSTALL_LIBEXECS", "qt_lbexpath="},
        {"QT_INSTALL_BINS", "qt_binspath="},
        {"QT_INSTALL_TESTS", "qt_tstspath="},
        {"QT_INSTALL_PLUGINS", "qt_plugpath="},
        {"QT_INSTALL_IMPORTS", "qt_impspath="},
        {"QT_INSTALL_QML", "qt_qml2path="},
        {"QT_INSTALL_TRANSLATIONS", "qt_trnspath="},
        {"QT_INSTALL_EXAMPLES", "qt_xmplpath="},
        {"QT_INSTALL_DEMOS", "qt_demopath="},
        {"QT_HOST_PREFIX", "qt_hpfxpath="},
        {"QT_HOST_DATA", "qt_hdatpath="},
        {"QT_HOST_BINS", "qt_hbinpath="},
        {"QT_HOST_LIBS", "qt_hlibpath="}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    string newQtDirNative = normalizeSeparators(m_NewQtDir);
    for (size_t i = 0; i < sizeof(Params) / sizeof(Params[0]); ++i) {
        const TParam& Param = Params[i];
        if (!m_QMake.value(Param.Name).empty()) {
            string NewValue = Param.Prefix;
            NewValue.append(newQtDirNative);
            const std::string suffix = m_QMake.suffix(Param.Name);
            if (!suffix.empty())
                NewValue += separator() + suffix;
            m_BinPatchValues[Param.Prefix] = NewValue;
        } else {
            LOG_V("Variable \"%s\" not found in qmake output.\n", Param.Name);
        }
    }
}

//------------------------------------------------------------------------------

void TQtBinPatcher::createPatchValues()
{
    m_TxtPatchValues.clear();
    m_BinPatchValues.clear();

    addTxtPatchValues(normalizeSeparators(m_QMake.qtInstallPrefix()));
    createBinPatchValues();

    const TStringList* pValues = m_ArgsMap.values(OPT_OLD_DIR);
    if (pValues != NULL)
        for (TStringList::const_iterator Iter = pValues->begin(); Iter != pValues->end(); ++Iter)
            addTxtPatchValues(normalizeSeparators(*Iter));

    LOG_V("\nPatch values for text files:\n%s",
        stringMapToStr(m_TxtPatchValues, "  \"", "\" -> \"", "\"\n").c_str());

    LOG_V("\nPatch values for binary files:\n%s",
        stringMapToStr(m_BinPatchValues, "  \"", "\" -> \"", "\"\n").c_str());
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::createTxtFilesForPatchList()
{
    struct TElement
    {
        const char* const Dir;
        const char* const Name;
        const bool        Recursive;
    };

    // Files for patching in Qt4.
    static const TElement Elements4[] = {
        {"/lib/", "*.pri", true},
        {"/demos/shared/", "libdemo_shared.prl", false},
        {"/lib/pkgconfig/", "Qt*.pc", false},
        {"/lib/pkgconfig/", "phonon*.pc", false},
        {"/mkspecs/default/", "qmake.conf", false},
        {"/", ".qmake.cache", false},
        {"/lib/pkgconfig/", "qt*.pc", false},
        {"/lib/", "*.la", false},
        {"/mkspecs/", "qconfig.pri", false}
    };

    // Files for patching in Qt5.
    static const TElement Elements5[] = {
        {"/", "*.la", true},
        {"/", "*.prl", true},
        {"/lib/pkgconfig/", "Qt5*.pc", true},
        {"/lib/pkgconfig/", "Enginio*.pc", true},
        {"/", "*.pri", true},
        {"/lib/cmake/Qt5LinguistTools/", "Qt5LinguistToolsConfig.cmake", false},
        {"/mkspecs/default-host/", "qmake.conf", false},
        {"/mkspecs/default/", "qmake.conf", false},
        {"/", ".qmake.cache", false},
        {"/lib/", "prl.txt", false}
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    m_TxtFilesForPatch.clear();

    const TElement* Elements;
    size_t Count;
    switch (m_QMake.qtVersion()) {
        case '4':
            Elements = Elements4;
            Count = sizeof(Elements4) / sizeof(Elements4[0]);
            break;
        case '5':
            Elements = Elements5;
            Count = sizeof(Elements5) / sizeof(Elements5[0]);
            break;
        default:
            LOG_E("Unsupported Qt version (%c).", m_QMake.qtVersion());
            return false;
    }
    for (size_t i = 0; i < Count; ++i) {
        if (Elements[i].Recursive)
            splice(&m_TxtFilesForPatch, findFilesRecursive(m_QtDir + Elements[i].Dir, Elements[i].Name));
        else
            splice(&m_TxtFilesForPatch, findFiles(m_QtDir + Elements[i].Dir, Elements[i].Name));
    }

    LOG_V("\nList of text files for patch:\n%s\n",
        stringListToStr(m_TxtFilesForPatch, "  ", "\n").c_str());

    return true;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::createBinFilesForPatchList()
{
    struct TElement
    {
        const char* const Dir;
        const char* const Name;
        const char *const xplatform;
    };

    // Files for patching in Qt4.
    static const TElement Elements4[] = {
#if defined(OS_WINDOWS)
        {"/bin/", "qmake.exe", NULL},
        {"/bin/", "lrelease.exe", NULL},
#else
        {"/bin/", "qmake", NULL},
        {"/bin/", "lrelease", NULL},
#endif
#ifndef QTCROSS
        {"/bin/", "QtCore*.dll", "win"},
        {"/lib/", "QtCore*.dll", "win"},
        {"/lib/", "libQtCore*.so", "linux"},
        {"/lib/", "libQtCore*.dylib", "macx"},
        {"/lib/QtCore.framework/", "QtCore", "macx"}
#endif
    };

    // Files for patching in Qt5.
    static const TElement Elements5[] = {
#ifdef OS_WINDOWS
        {"/bin/", "qmake.exe", NULL},
        {"/bin/", "lrelease.exe", NULL},
        {"/bin/", "qdoc.exe", NULL},
#else
        {"/bin/", "qmake", NULL},
        {"/bin/", "lrelease", NULL},
        {"/bin/", "qdoc", NULL},
#endif
#ifndef QTCROSS
        {"/bin/", "Qt5Core*.dll", "win"},
        {"/lib/", "Qt5Core*.dll", "win"},
        {"/lib/", "libQt5Core*.so", "linux"},
        {"/lib/", "libQt5Core*.dylib", "macx"},
        {"/lib/QtCore.framework/", "QtCore", "macx"}
#endif
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    m_BinFilesForPatch.clear();

    const TElement* Elements;
    size_t Count;
    switch (m_QMake.qtVersion()) {
        case '4':
            Elements = Elements4;
            Count = sizeof(Elements4) / sizeof(Elements4[0]);
            break;
        case '5':
            Elements = Elements5;
            Count = sizeof(Elements5) / sizeof(Elements5[0]);
            break;
        default:
            LOG_E("Unsupported Qt version (%c).\n", m_QMake.qtVersion());
            return false;
    }

    string xspec = m_QMake.xSpec();
    if (xspec.empty()) {
        LOG_E("Unable to get mkspec from Qt install.\n");
        return false;
    }

    for (size_t i = 0; i < Count; ++i) {
        bool flag = false;
        if (Elements[i].xplatform == NULL)
            flag = true;
        else {
            const char *const d = ",";
            char *copystr = new char[strlen(Elements[i].xplatform) + 2];
            strcpy(copystr, Elements[i].xplatform);
            char *c = strtok(copystr, d);
            while (c != NULL) {
                if (xspec.find(c) != string::npos) {
                    flag = true;
                    break;
                }
                c = strtok(NULL, d);
            }
            delete[] copystr;
        }
        if (flag)
            splice(&m_BinFilesForPatch, findFiles(m_QtDir + Elements[i].Dir, Elements[i].Name));
    }

    LOG_V("\nList of binary files for patch:\n%s\n",
        stringListToStr(m_BinFilesForPatch, "  ", "\n").c_str());

    return true;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::patchTxtFile(const string& fileName)
{
    LOG("Patching text file \"%s\".\n", fileName.c_str());

    if (m_ArgsMap.contains(OPT_DRY_RUN))
        return true;

    bool Result = false;

    if ((m_QMake.qtVersion() == '4') && (fileName.substr(fileName.length() - 26) == "mkspecs/default/qmake.conf")) // Workaround QTBUG-27593 && QTBUG-28792
        return patchQtbug27593(fileName);

    FILE* File = fopen(fileName.c_str(), "r+b");
    if (File != NULL) {
        vector<char> Buf;
        long FileLength = getFileSize(File);

        if (FileLength > 0) {
            Buf.resize(FileLength);
            if (fread(Buf.data(), FileLength, 1, File) == 1) {
                for (TStringMap::const_iterator Iter = m_TxtPatchValues.begin(); Iter != m_TxtPatchValues.end(); ++Iter) {
                    string::size_type Delta = 0;
                    vector<char>::iterator Found;
                    while ((Found = search(Buf.begin() + Delta, Buf.end(),
                        Iter->first.begin(), Iter->first.end()
#ifdef OS_WINDOWS
                        , caseInsensitiveComp
#endif
                        ))
                        != Buf.end()) {
                        Delta = Found - Buf.begin() + static_cast<int>(Iter->second.length());
                        Found = Buf.erase(Found, Found + Iter->first.length());
                        Buf.insert(Found, Iter->second.begin(), Iter->second.end());
                    }
                }
                zeroFile(File);
                if (fwrite(Buf.data(), Buf.size(), 1, File) == 1)
                    Result = true;
                else
                    LOG_E("Error writing to file \"%s\".\n", fileName.c_str());
            } else {
                LOG_E("Error reading from file \"%s\".\n", fileName.c_str());
            }
        } else {
            LOG_V("  File is empty. Skipping.\n");
            Result = true;
        }

        fclose(File);
    } else {
        LOG_E("Error opening file \"%s\". Error %i.\n", fileName.c_str(), errno);
    }

    return Result;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::patchQtbug27593(const string &fileName)
{
    try {
        string xspec = m_QMake.xSpec();
        ofstream ofs(fileName.c_str(), ios_base::out | ios_base::trunc);
        ofs << (string("QMAKESPEC_ORIGINAL=") + m_NewQtDir + string("/mkspecs/") + xspec) << endl << endl;
        ofs << (string("include(../") + xspec + string("/qmake.conf)")) << endl << endl;
    } catch(...) {
        return false;
    }

    return true;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::patchBinFile(const string& fileName)
{
    LOG("Patching binary file \"%s\".\n", fileName.c_str());

    if (m_ArgsMap.contains(OPT_DRY_RUN))
        return true;

    bool Result = false;

    FILE* File = fopen(fileName.c_str(), "r+b");
    if (File != NULL) {
        long BufSize = getFileSize(File);
        char* Buf = new char[BufSize];

        if (fread(Buf, BufSize, 1, File) == 1) {
            for (TStringMap::const_iterator Iter = m_BinPatchValues.begin(); Iter != m_BinPatchValues.end(); ++Iter) {
                char* First = Buf;
                while ((First = search(First, Buf + BufSize,
                    Iter->first.begin(), Iter->first.end()
#ifdef OS_WINDOWS
                    , caseInsensitiveComp
#endif
                    ))
                    != Buf + BufSize
                    ) {
                    strcpy(First, Iter->second.c_str());
                    First += Iter->second.length();
                }
            }
            rewind(File);
            if (fwrite(Buf, BufSize, 1, File) == 1)
                Result = true;
            else
                LOG_E("Error writing to file \"%s\".\n", fileName.c_str());
        } else {
            LOG_E("Error reading from file \"%s\".\n", fileName.c_str());
        }

        delete[] Buf;
        fclose(File);
    } else {
        LOG_E("Error opening file \"%s\". Error %i.\n", fileName.c_str(), errno);
    }

    return Result;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::patchTxtFiles()
{
    for (TStringList::const_iterator Iter = m_TxtFilesForPatch.begin(); Iter != m_TxtFilesForPatch.end(); ++Iter)
        if (!patchTxtFile(*Iter))
            return false;
    return true;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::patchBinFiles()
{
    for (TStringList::const_iterator Iter = m_BinFilesForPatch.begin(); Iter != m_BinFilesForPatch.end(); ++Iter)
        if (!patchBinFile(*Iter))
            return false;
    return true;
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::exec()
{
    if (!getQtDir())
        return false;
    if (!getNewQtDir())
        return false;

    TBackup Backup;
    if (!m_ArgsMap.contains(OPT_DRY_RUN)) {
        Backup.setSkipBackup(m_ArgsMap.contains(OPT_NOBACKUP));
        if (!Backup.backupFile(m_QtDir + "/bin/qt.conf", TBackup::bmRename))
            return false;
    }

    if (!isPatchNeeded()) {
        if (m_ArgsMap.contains(OPT_FORCE)) {
            LOG("\nThe new and the old pathes to Qt directory are the same.\n"
                "Perform forced patching.\n\n");
        } else {
            LOG("\nThe new and the old pathes to Qt directory are the same.\n"
                "Patching not needed.\n");
            return true;
        }
    }

    createPatchValues();
    if (!createTxtFilesForPatchList() || !createBinFilesForPatchList())
        return false;

    if (!m_ArgsMap.contains(OPT_DRY_RUN)) {
        if (!Backup.backupFiles(m_TxtFilesForPatch) || !Backup.backupFiles(m_BinFilesForPatch))
            return false;
    }

    if (!patchTxtFiles() || !patchBinFiles())
        return false;


    if (!m_ArgsMap.contains(OPT_DRY_RUN)) {
        // Finalization.
        if (m_ArgsMap.contains(OPT_BACKUP))
            Backup.save();
        else {
            if (!Backup.remove())
                return false;
        }
    }

    return true;
}

//------------------------------------------------------------------------------

TQtBinPatcher::TQtBinPatcher(const TStringListMap& argsMap)
    : m_ArgsMap(argsMap),
    m_QMake(getStartDir()),
    m_hasError(false)
{
    if (m_QMake.hasError()) {
        m_hasError = true;
        LOG_E("%s\n", m_QMake.errorString().c_str());
    } else {
        m_hasError = !exec();
    }
}

//------------------------------------------------------------------------------

bool TQtBinPatcher::exec(const TStringListMap& argsMap)
{
    TQtBinPatcher QtBinPatcher(argsMap);
    return !QtBinPatcher.m_hasError;
}

//------------------------------------------------------------------------------
