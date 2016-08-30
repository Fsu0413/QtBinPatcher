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

#include "Logger.hpp"

#include <errno.h>
#include <string.h>

//------------------------------------------------------------------------------

bool TLogger::m_Verbose = false;

//------------------------------------------------------------------------------

TLogger::TLogger()
    : m_pFile(NULL)
{
}

//------------------------------------------------------------------------------

TLogger::~TLogger()
{
    if (m_pFile != NULL)
        fclose(m_pFile);
}

//------------------------------------------------------------------------------

void TLogger::printf(FILE* stdstream, const char* const format, va_list vaList)
{
    if (m_pFile != NULL) {
        va_list vaList2;
#if defined(_MSC_VER) && (_MSC_VER <= 1700)
        vaList2 = vaList;
#else
        va_copy(vaList2, vaList);
#endif
        vfprintf(m_pFile, format, vaList2);
        va_end(vaList2);
        fflush(m_pFile);
    }
    vfprintf(stdstream, format, vaList);
    fflush(stdstream);
}

//------------------------------------------------------------------------------

TLogger* TLogger::instance()
{
    static TLogger Logger;
    return &Logger;
}

//------------------------------------------------------------------------------

void TLogger::setFileName(const char* const fileName)
{
    if (m_pFile != NULL)
        fclose(m_pFile);

    if (fileName != NULL && strlen(fileName) > 0) {
        m_pFile = fopen(fileName, "w");
        if (m_pFile == NULL) {
            fprintf(stderr, "Error opening logfile\"%s\".\n"
                "Error %i.\n",
                fileName, errno);
        }
    } else {
        m_pFile = NULL;
    }
}

//------------------------------------------------------------------------------

void TLogger::printf(const char* const format, ...)
{
    va_list vaList;
    va_start(vaList, format);
    printf(stdout, format, vaList);
    va_end(vaList);
}

//------------------------------------------------------------------------------

void TLogger::printf_err(const char* const format, ...)
{
    va_list vaList;
    va_start(vaList, format);
    printf(stderr, format, vaList);
    va_end(vaList);
}

//------------------------------------------------------------------------------
