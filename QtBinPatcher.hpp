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

#ifndef __QTBINPATCHER2__
#define __QTBINPATCHER2__

//------------------------------------------------------------------------------

#include "CommonTypes.hpp"
//#include "CmdLineParser.hpp"
#include "QMake.hpp"

//------------------------------------------------------------------------------

class TQtBinPatcher
{
private:
    const TStringListMap& m_ArgsMap;
    std::string m_QtDir;
    std::string m_NewQtDir;
    TStringMap  m_TxtPatchValues;
    TStringMap  m_BinPatchValues;
    TStringList m_TxtFilesForPatch;
    TStringList m_BinFilesForPatch;
    TQMake      m_QMake;
    bool        m_hasError;

    std::string getStartDir() const;
    bool getQtDir();
    bool getNewQtDir();
    bool isPatchNeeded();
    void addTxtPatchValues(const std::string& oldPath);
    void addBinPatchValues(const std::string& oldPath);
    void createBinPatchValues();
    void createPatchValues();
    bool createTxtFilesForPatchList();
    bool createBinFilesForPatchList();
    bool patchTxtFile(const std::string& fileName);
    bool patchQtbug27593(const std::string &fileName);
    bool patchBinFile(const std::string& fileName);
    bool patchTxtFiles();
    bool patchBinFiles();
    bool exec();

    TQtBinPatcher(const TStringListMap& argsMap);

public:
    static bool exec(const TStringListMap& argsMap);

};

//------------------------------------------------------------------------------

#endif // __QTBINPATCHER2__
