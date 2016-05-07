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

#include "CmdLineParser.hpp"

#include <string.h>

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

inline bool strStartingWith(const string& str1, const char *const str2)
{
    return str1.compare(0, strlen(str2), str2) == 0;
}

//------------------------------------------------------------------------------

TCmdLineParser::TCmdLineParser(int argc, const char* argv[])
{
    for (int argNumber = 1; argNumber < argc; ++argNumber) {
        string Opt = argv[argNumber];
        if (strStartingWith(Opt, "--") && Opt.length() > 2) {
            Opt = Opt.substr(2);
            string::size_type pos = Opt.find("=");
            string OptName = Opt.substr(0, pos);
            string OptValue;
            if (pos != string::npos)
                OptValue = Opt.substr(pos + 1);
            if (!OptValue.empty())
                m_ArgsMap[OptName].push_back(OptValue);
            else
                m_ArgsMap[OptName]; // Creating new key with empty value.
        } else {
            m_ErrorString += "Unknown command line parameter: \"" + Opt + "\".\n";
        }
    }
}

//------------------------------------------------------------------------------

string TCmdLineParser::dump() const
{
    string Result = "Parsed command line options:\n";
    for (TStringListMap::const_iterator Iter = m_ArgsMap.begin(); Iter != m_ArgsMap.end(); ++Iter) {
        Result += "  " + Iter->first + ' ';
        size_t spaces = Iter->first.length() + 3;
        TStringList Values = Iter->second;
        if (!Values.empty()) {
            for (TStringList::const_iterator Jter = Values.begin(); Jter != Values.end(); ++Jter) {
                if (Jter != Values.begin())
                    Result += string(spaces, ' ');
                Result += '\"' + *Jter + "\"\n";
            }
        } else {
            Result += '\n';
        }
    }
    Result += '\n';
    return Result;
}

//------------------------------------------------------------------------------
