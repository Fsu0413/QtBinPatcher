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

#ifndef __QTBINPATCHER2_FUNCTIONS__
#define __QTBINPATCHER2_FUNCTIONS__

//------------------------------------------------------------------------------

#include "CommonTypes.hpp"

//------------------------------------------------------------------------------

namespace Functions
{
    void splice(TStringList* pX, TStringList Y);
    char separator();
    std::string normalizeSeparators(const std::string& path);
    std::string trimSeparators(const std::string& str);
    bool hasOnlyNormalSeparators(const char* path);
    void replace(std::string* pS, char before, const char* after);
    bool startsWith(const std::string& str, const char* start);
    std::string absolutePath(const std::string& relativePath);
    std::string currentDir();
    bool isFileExists(const char* fileName);
    long getFileSize(FILE* file);
    bool zeroFile(FILE* file);
    bool renameFile(const char* oldFileName, const char* newFileName);
    bool copyFile(const char* fromFileName, const char* toFileName);
    bool removeFile(const char* fileName);
    std::string getProgramOutput(const char* fileName);
    std::string currentTime(const char* format);
    TStringList findFiles(std::string dir, const std::string& mask);
    TStringList findFilesRecursive(std::string dir, const std::string& mask);
    std::string stringListToStr(const TStringList& list, const std::string& prefix, const std::string& suffix);
    std::string stringMapToStr(const TStringMap& map, const std::string& prefix, const std::string& separator, const std::string& suffix);

    //--------------------------------------------------------------------------

    inline bool hasOnlyNormalSeparators(const std::string& path)
    {
        return hasOnlyNormalSeparators(path.c_str());
    }

    inline bool startsWith(const std::string& str, const std::string& start)
    {
        return startsWith(str, start.c_str());
    }

    inline bool isFileExists(const std::string& fileName)
    {
        return isFileExists(fileName.c_str());
    }

    inline bool renameFile(const std::string& oldFileName, const std::string& newFileName)
    {
        return renameFile(oldFileName.c_str(), newFileName.c_str());
    }

    inline bool copyFile(const std::string& fromFileName, const std::string& toFileName)
    {
        return copyFile(fromFileName.c_str(), toFileName.c_str());
    }

    inline bool removeFile(const std::string& fileName)
    {
        return removeFile(fileName.c_str());
    }

    inline std::string getProgramOutput(const std::string& fileName)
    {
        return getProgramOutput(fileName.c_str());
    }

}  // namespace Functions

//------------------------------------------------------------------------------

#endif // __QTBINPATCHER2_FUNCTIONS__
