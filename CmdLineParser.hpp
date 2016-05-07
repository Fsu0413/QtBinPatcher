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

#ifndef __QTBINPATCHER2_CMDLINEPARSER__
#define __QTBINPATCHER2_CMDLINEPARSER__

//------------------------------------------------------------------------------

#include "CommonTypes.hpp"

//------------------------------------------------------------------------------

class TCmdLineParser
{
private:
    TStringListMap m_ArgsMap;
    std::string    m_ErrorString;

public:
    TCmdLineParser(int argc, const char *argv[]);

    std::string dump() const;

    const TStringListMap& argsMap() const
    {
        return m_ArgsMap;
    }
    inline bool hasError() const
    {
        return !m_ErrorString.empty();
    }
    inline const std::string& errorString() const
    {
        return m_ErrorString;
    }
};

//------------------------------------------------------------------------------

#endif // __QTBINPATCHER2_CMDLINEPARSER__
