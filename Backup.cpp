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

#include "Backup.hpp"

#include <sstream>

#include "Logger.hpp"
#include "Functions.hpp"

//------------------------------------------------------------------------------

using namespace std;
using namespace Functions;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

TBackup::TFileMapping::TFileMapping(string _fileName, string _bakFileName)
    : fileName(_fileName), bakFileName(_bakFileName)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

const char* const TBackup::bakFileSuffix = ".bak";

//------------------------------------------------------------------------------

string TBackup::backupFileName(const string& fileName)
{
    string BakFileName = fileName + bakFileSuffix;
    unsigned int i = 0;
    while (isFileExists(BakFileName)) {
        stringstream SStream;
        SStream << fileName << '.' << currentTime("%Y-%m-%d_%H-%M-%S");
        if (i++)
            SStream << "_" << i;
        SStream << bakFileSuffix;
        BakFileName = SStream.str();
    }
    return BakFileName;
}

//------------------------------------------------------------------------------

TBackup::TBackup()
    : m_SkipBackup(false)
{
}

//------------------------------------------------------------------------------

TBackup::~TBackup()
{
    restore();
}

//------------------------------------------------------------------------------

bool TBackup::backupFile(const string& fileName, const TBackupMethod method)
{
    if (isFileExists(fileName)) {
        if (!m_SkipBackup) {
            string BakFileName = backupFileName(fileName);
            switch (method) {
                case bmCopy:
                    if (!copyFile(fileName, BakFileName))
                        return false;
                    break;
                case bmRename:
                    if (!renameFile(fileName, BakFileName))
                        return false;
                    break;
            }
            m_FilesMapping.push_back(TFileMapping(fileName, BakFileName));
        } else {
            if (method == bmRename && !removeFile(fileName))
                return false;
        }
    } else {
        if (!m_SkipBackup) {
            LOG_V("Skipping backup of file \"%s\" - file not found.\n",
                fileName.c_str());
        }
    }
    return true;
}

//------------------------------------------------------------------------------

bool TBackup::backupFiles(const TStringList& files, const TBackupMethod method)
{
    if (m_SkipBackup && method == bmCopy)
        return true;

    for (TStringList::const_iterator Iter = files.begin(); Iter != files.end(); ++Iter)
        if (!backupFile(*Iter, method))
            return false;
    return true;
}

//------------------------------------------------------------------------------

bool TBackup::remove()
{
    bool Result = true;
    if (!m_FilesMapping.empty()) {
        LOG_V("\nCleaning backup list.\n");
        for (TFilesMapping::const_iterator Iter = m_FilesMapping.begin(); Iter != m_FilesMapping.end(); ++Iter)
            if (!removeFile(Iter->bakFileName))
                Result = false;
        m_FilesMapping.clear();
    }
    return Result;
}

//------------------------------------------------------------------------------

bool TBackup::restore()
{
    bool Result = true;
    if (!m_FilesMapping.empty()) {
        LOG_V("\nRestoring backup.\n");
        for (TFilesMapping::const_iterator Iter = m_FilesMapping.begin(); Iter != m_FilesMapping.end(); ++Iter)
            if (!removeFile(Iter->fileName) || !renameFile(Iter->bakFileName, Iter->fileName))
                Result = false;
        m_FilesMapping.clear();
    }
    return Result;
}

//------------------------------------------------------------------------------

void TBackup::save()
{
    m_FilesMapping.clear();
}

//------------------------------------------------------------------------------

void TBackup::setSkipBackup(bool skipBackup)
{
    m_SkipBackup = skipBackup;
    if (m_SkipBackup)
        LOG_V("Skipping backup of files.\n");
}

//------------------------------------------------------------------------------
