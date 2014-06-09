/*******************************************************************************

         Yuri V. Krugloff. 2013-2014. http://www.tver-soft.org

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

#ifndef __QTBINPATCHER2_QMAKE__
#define __QTBINPATCHER2_QMAKE__

//------------------------------------------------------------------------------

#include "Common.hpp"

//------------------------------------------------------------------------------

class TQMake
{
    private :
        static const std::string m_QMakeName;
        static const std::string m_BinDirName;

        TStringMap  m_QMakeValues;
        std::string m_QMakePath;
        std::string m_QtPath;
        std::string m_QMakeOutput;
        char        m_QtVersion;

    public :
        TQMake();

        bool find(const std::string& qtDir);
        bool query();
        bool parse();
        std::string value(const std::string& variable) const;

        inline const std::string& qmakePath() const
            { return m_QMakePath; }
        inline const std::string& qtPath() const
            { return m_QtPath; }
        inline const TStringMap& qmakeValues() const
            { return m_QMakeValues; }
        inline char qtVersion() const
            { return m_QtVersion; }
        inline std::string qtInstallPrefix() const
            { return value("QT_INSTALL_PREFIX"); }
};

//------------------------------------------------------------------------------

#endif // __QTBINPATCHER2_QMAKE__
