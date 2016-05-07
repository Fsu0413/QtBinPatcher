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

#include "CmdLineChecker.hpp"

#include "CmdLineOptions.hpp"

//------------------------------------------------------------------------------

using namespace std;

//------------------------------------------------------------------------------

TCmdLineChecker::TCmdLineChecker(const TStringListMap& argsMap)
    : m_ArgsMap(argsMap)
{
}

//------------------------------------------------------------------------------

void TCmdLineChecker::check(const string& option, const TCmdLineChecker::TOptionType optionType)
{
    TStringListMap::iterator Iter = m_ArgsMap.find(option);
    if (Iter != m_ArgsMap.end()) {
        switch (optionType) {
            case otNoValue:
                if (!Iter->second.empty())
                    m_ErrorString += "Option \"--" + option + "\" cannot have value.\n";
                break;
            case otSingleValue:
                if (Iter->second.size() > 1)
                    m_ErrorString += "Option \"--" + option + "\" can be only one.\n";
                // break is missing! It's right!
            case otMultiValue:
                if (Iter->second.empty())
                    m_ErrorString += "Option \"--" + option + "\" must have value.\n";
                break;
        }
        m_ArgsMap.erase(Iter);
    }
}

//------------------------------------------------------------------------------

void TCmdLineChecker::checkIncompatible(const string& option1, const string& option2)
{
    if (m_ArgsMap.find(option1) != m_ArgsMap.end() && m_ArgsMap.find(option2) != m_ArgsMap.end())
        m_ErrorString += "Options \"--" + option1 + "\" and \"--" + option2 + "\" are incompatible.\n";
}

//------------------------------------------------------------------------------

void TCmdLineChecker::endCheck()
{
    for (TStringListMap::const_iterator Iter = m_ArgsMap.begin(); Iter != m_ArgsMap.end(); ++Iter)
        m_ErrorString += "Unknown option: \"--" + Iter->first + "\".\n";

}

//------------------------------------------------------------------------------

string TCmdLineChecker::check(const TStringListMap& argsMap)
{
    TCmdLineChecker Checker(argsMap);

    Checker.checkIncompatible(OPT_BACKUP, OPT_NOBACKUP);
    Checker.check(OPT_VERSION, otNoValue);
    Checker.check(OPT_HELP, otNoValue);
    Checker.check(OPT_VERBOSE, otNoValue);
    Checker.check(OPT_LOGFILE, otSingleValue);
    Checker.check(OPT_BACKUP, otNoValue);
    Checker.check(OPT_NOBACKUP, otNoValue);
    Checker.check(OPT_FORCE, otNoValue);
    Checker.check(OPT_QT_DIR, otSingleValue);
    Checker.check(OPT_NEW_DIR, otSingleValue);
    Checker.check(OPT_OLD_DIR, otMultiValue);
    Checker.check(OPT_DRY_RUN, otNoValue);
    Checker.endCheck();

    return Checker.m_ErrorString;
}

//------------------------------------------------------------------------------
