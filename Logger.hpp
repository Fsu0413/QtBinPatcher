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

#ifndef __QTBINPATCHER2_LOGGER_HPP__
#define __QTBINPATCHER2_LOGGER_HPP__

//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdarg.h>

//------------------------------------------------------------------------------

class TLogger
{
private:
    static bool m_Verbose;
    FILE* m_pFile;

    TLogger();
    ~TLogger();
    void printf(FILE* stdstream, const char *const format, va_list vaList);

public:
    static TLogger* instance();

    void setFileName(const char *const fileName);
    void printf(const char *const format, ...);
    void printf_err(const char *const format, ...);

    static inline bool verbose() { return m_Verbose; }
    static inline void setVerbose(bool verbose) { m_Verbose = verbose; }
};

//------------------------------------------------------------------------------

#define LOG_SET_FILENAME(FileName) \
    TLogger::instance()->setFileName(FileName);
#define LOG(Format, ...) \
    TLogger::instance()->printf(Format, ##__VA_ARGS__)
#define LOG_E(Format, ...) \
    TLogger::instance()->printf_err(Format, ##__VA_ARGS__)
#define LOG_V(Format, ...) \
    if (TLogger::verbose()) \
        TLogger::instance()->printf(Format, ##__VA_ARGS__)

//------------------------------------------------------------------------------

#endif // __QTBINPATCHER2_LOGGER_HPP__
