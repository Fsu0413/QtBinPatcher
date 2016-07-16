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

#include "Functions.hpp"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#if defined(OS_WINDOWS)
#include <io.h>
#include <direct.h>
#elif defined(OS_LINUX) || defined(OS_MACOS)
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>
#include <limits.h>
#endif
#include <time.h>

#include "Logger.hpp"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

#ifdef _MSC_VER
#define GETCWD _getcwd
#define POPEN  _popen
#define PCLOSE _pclose
#else
#define GETCWD getcwd
#define POPEN  popen
#define PCLOSE pclose
#endif

#ifdef OS_WINDOWS
#define POPEN_MODE "rt"
#else
#define POPEN_MODE "r"
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void eraseLastSeparators(string* pStr)
{
    for (int i = static_cast<int>(pStr->size()) - 1; i >= 0; --i) {
        const char& c = pStr->at(i);
        if (c == '/' || c == '\\')
            pStr->erase(i);
        else
            break;
    }
}

//------------------------------------------------------------------------------

void addLastSeparator(string* pStr)
{
    if (pStr != NULL && !pStr->empty()) {
        string::const_reverse_iterator Iter = pStr->rbegin();
        if (*Iter != '/' && *Iter != '\\')
            *pStr += Functions::separator();
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void Functions::splice(TStringList* pX, TStringList Y)
{
    if (pX != NULL && !Y.empty()) {
        TStringList::iterator Iter = pX->end();
        pX->splice(Iter, Y);
    }
}

//------------------------------------------------------------------------------

char Functions::separator()
{
    return '/';
}

//------------------------------------------------------------------------------

string Functions::normalizeSeparators(const string &path)
{
#ifdef OS_WINDOWS
    string Result = path;
    for (string::iterator Iter = Result.begin(); Iter != Result.end(); ++Iter)
        if (*Iter == '\\')
            *Iter = '/';
    return Result;
#else
    return path;
#endif
}

//------------------------------------------------------------------------------

string Functions::trimSeparators(const string& str)
{
    string Result;
    string::size_type first = 0;
    for (; first < str.length(); ++first)
        if (str[first] != '/' && str[first] != '\\')
            break;
    Result = str.substr(first);
    eraseLastSeparators(&Result);
    return Result;
}

//------------------------------------------------------------------------------

bool Functions::hasOnlyNormalSeparators(const char* path)
{
    while (*path != '\0')
        if (*path++ == '\\')
            return false;
    return true;
}

//------------------------------------------------------------------------------
// Replace char in string to new substring.

void Functions::replace(string* pS, char before, const char* after)
{
    size_t len = strlen(after);
    size_t pos = pS->find(before);
    while (pos != string::npos) {
        pS->replace(pos, 1, after);
        pos = pS->find(before, pos + len);
    }
}


//------------------------------------------------------------------------------

bool Functions::startsWith(const string& str, const char* start)
{
    return str.compare(0, strlen(start), start) == 0;
}

//------------------------------------------------------------------------------

string Functions::absolutePath(const string& relativePath)
{
    string Result;
#if defined(OS_WINDOWS)
    char AbsPath[_MAX_PATH];
    if (_fullpath(AbsPath, relativePath.c_str(), _MAX_PATH) != NULL)
        Result = normalizeSeparators(AbsPath);
#elif defined(OS_LINUX) || defined(OS_MACOS)
    char AbsPath[PATH_MAX];
    if (realpath(relativePath.c_str(), AbsPath) != NULL)
        Result = AbsPath;
#else
#error "Unsupported OS."
#endif

    if (Result.empty()) {
        LOG_E("Error translate path to absolute. Error %i.\n"
            "  (%s)",
            errno, relativePath.c_str());
    }

    eraseLastSeparators(&Result);
    return Result;
}

//------------------------------------------------------------------------------

string Functions::currentDir()
{
    string Result;

    char* cwd = GETCWD(NULL, 0);
    if (cwd != NULL) {
        Result = cwd;
        free(cwd);
    } else {
        LOG_E("Error getting current directory. Error %i.\n", errno);
    }

    eraseLastSeparators(&Result);

    return normalizeSeparators(Result);
}

//------------------------------------------------------------------------------

bool Functions::isFileExists(const char* fileName)
{
#if defined(OS_WINDOWS)
    _finddata_t FindData;
    intptr_t FindHandle = _findfirst(fileName, &FindData);
    if (FindHandle != -1) {
        _findclose(FindHandle);
        return true;
    }
#elif defined(OS_LINUX) || defined(OS_MACOS)
    glob_t GlobData;
    if (glob(fileName, 0, NULL, &GlobData) == 0) {
        globfree(&GlobData);
        return true;
    }
#else
#error "Unsupported OS."
#endif

    return false;
}

//------------------------------------------------------------------------------
// Getting size of opened file.

long Functions::getFileSize(FILE* file)
{
#if defined(OS_WINDOWS)
    return _filelength(_fileno(file));
#elif defined(OS_LINUX) || defined(OS_MACOS)
    struct stat Stat;
    if (fstat(fileno(file), &Stat) == 0)
        return Stat.st_size;
    return -1;
#else
#error "Unsupported OS."
#endif
}

//------------------------------------------------------------------------------
// Truncation file to empty (zero size).

bool Functions::zeroFile(FILE* file)
{
    rewind(file);
#if defined(OS_WINDOWS)
    return _chsize(_fileno(file), 0) == 0;
#elif defined(OS_LINUX) || defined(OS_MACOS)
    return ftruncate(fileno(file), 0) == 0;
#else
#error "Unsupported OS."
#endif
}
//------------------------------------------------------------------------------

bool Functions::renameFile(const char* oldFileName, const char* newFileName)
{
    LOG_V("Renaming file \"%s\"\n"
        "           to \"%s\".\n",
        oldFileName, newFileName);

    if (rename(oldFileName, newFileName) != 0) {
        LOG_E("Error renaming file \"%s\" to \"%s\". Error %i.\n",
            oldFileName, newFileName, errno);
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------

bool Functions::copyFile(const char* fromFileName, const char* toFileName)
{
    LOG_V("Copying file content from \"%s\"\n"
        "                       to \"%s\".\n",
        fromFileName, toFileName);

    bool Result = true;
    FILE* src = fopen(fromFileName, "rb");
    if (src != NULL) {
        FILE* dst = fopen(toFileName, "wb");
        if (dst != NULL) {
            char Buffer[1024 * 32];  // 32kb
            while (!feof(src)) {
                size_t size = fread(Buffer, 1, sizeof(Buffer), src);
                if (fwrite(Buffer, 1, size, dst) != size) {
                    LOG_E("Error writing to file \"%s\".\n", toFileName);
                    Result = false;
                    break;
                }
            }
            fclose(dst);
            if (!Result)
                removeFile(toFileName);
        } else {
            LOG_E("Error opening file \"%s\" for writing.\n", toFileName);
            Result = false;
        }
        fclose(src);
    } else {
        LOG_E("Error opening file \"%s\" for reading.\n", fromFileName);
        Result = false;
    }

    return Result;
}

//------------------------------------------------------------------------------

bool Functions::removeFile(const char* fileName)
{
    if (isFileExists(fileName)) {
        LOG_V("Removing file \"%s\"...\n", fileName);
        if (remove(fileName) != 0) {
            LOG_E("Error removing file \"%s\". Error %i.\n", fileName, errno);
            return false;
        }
    }
    return true;
}

//------------------------------------------------------------------------------

string Functions::currentTime(const char* format)
{
    time_t rawTime = time(NULL);
    struct tm* timeInfo = localtime(&rawTime);
    const int BufferSize = 1024;
    char Buffer[BufferSize];
    strftime(Buffer, BufferSize, format, timeInfo);
    return Buffer;
}

//------------------------------------------------------------------------------

std::string Functions::getProgramOutput(const char* fileName)
{
    string Result;
    FILE* out;
    out = POPEN(fileName, POPEN_MODE);
    if (out != NULL) {
        char Buffer[1024];
        while (fgets(Buffer, sizeof(Buffer), out) != NULL)
            Result += Buffer;
        if (ferror(out) != 0) {
            LOG_E("Error reading from pipe. Error %i.\n", errno);
            Result.clear();
        }
        if (PCLOSE(out) == -1)
            LOG_E("Error closing pipe. Error %i.\n", errno);
    } else {
        LOG_E("Error running program \"%s\". Error %i.\n", fileName, errno);
    }

    return Result;
}

//------------------------------------------------------------------------------

TStringList Functions::findFiles(string dir, const string& mask)
{
    TStringList Result;

    addLastSeparator(&dir);

#if defined(OS_WINDOWS)
    _finddata_t FindData;
    intptr_t FindHandle = _findfirst((dir + mask).c_str(), &FindData);
    if (FindHandle != -1) {
        do {
            if ((FindData.attrib & _A_SUBDIR) == 0)
                Result.push_back(dir + FindData.name);
        } while (_findnext(FindHandle, &FindData) == 0);
        _findclose(FindHandle);
    }
#elif defined(OS_LINUX) || defined(OS_MACOS)
    glob_t GlobData;
    if (glob((dir + mask).c_str(), GLOB_MARK, NULL, &GlobData) == 0) {
        for (size_t i = 0; i < GlobData.gl_pathc; ++i) {
            const char* path = GlobData.gl_pathv[i];
            if (path[strlen(path) - 1] != '/')
                Result.push_back(path);
        }
        globfree(&GlobData);
    }
#else
#error "Unsupported OS."
#endif

    return Result;
}

//------------------------------------------------------------------------------

TStringList Functions::findFilesRecursive(string dir, const string& mask)
{
    TStringList Result;

    addLastSeparator(&dir);

#if defined (OS_WINDOWS)
    _finddata_t FindData;
    intptr_t FindHandle = _findfirst((dir + "*").c_str(), &FindData);
    if (FindHandle != -1) {
        do {
            if ((FindData.attrib & _A_SUBDIR) != 0 &&
                strcmp(FindData.name, ".") != 0 &&
                strcmp(FindData.name, "..") != 0) {
                splice(&Result, findFilesRecursive(dir + FindData.name + "/", mask));
            }
        } while (_findnext(FindHandle, &FindData) == 0);
        _findclose(FindHandle);
    }
    splice(&Result, findFiles(dir, mask));
#elif defined (OS_LINUX) || defined(OS_MACOS)
    glob_t GlobData;
    if (glob((dir + mask).c_str(), GLOB_MARK, NULL, &GlobData) == 0) {
        for (size_t i = 0; i < GlobData.gl_pathc; ++i) {
            const char* path = GlobData.gl_pathv[i];
            if (path[strlen(path) - 1] != '/')
                Result.push_back(path);
            else
                splice(&Result, findFilesRecursive(path, mask));
        }
    }
#endif

    return Result;
}

//------------------------------------------------------------------------------

string Functions::stringListToStr(const TStringList& list, const string& prefix, const string& suffix)
{
    string Result;
    for (TStringList::const_iterator Iter = list.begin(); Iter != list.end(); ++Iter)
        Result += prefix + *Iter + suffix;
    return Result;
}

//------------------------------------------------------------------------------

string Functions::stringMapToStr(const TStringMap& map, const string& prefix, const string& separator, const string& suffix)
{
    string Result;
    for (TStringMap::const_iterator Iter = map.begin(); Iter != map.end(); ++Iter)
        Result += prefix + Iter->first + separator + Iter->second + suffix;
    return Result;
}

//------------------------------------------------------------------------------
