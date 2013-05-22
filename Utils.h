#pragma once

#include <windows.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>

class Utils
{
public:
    Utils(void);
    virtual ~Utils(void);

    static std::wstring Utils::getComputerName()
    {
        if(m_sComputerName.empty())
        {
            const DWORD nMaxLen = MAX_COMPUTERNAME_LENGTH + 1;
            DWORD nLen = nMaxLen;
            wchar_t szComputerName[nMaxLen];
            GetComputerNameW(szComputerName, &nLen);

            m_sComputerName = szComputerName;
        }

        return m_sComputerName;
    }

    static std::wstring Utils::systemTimeToLocalTime(SYSTEMTIME st)
    {

    }

    static std::wstring Utils::systemTimeToLocalTimeString(SYSTEMTIME st)
    {
        SYSTEMTIME lt;
        ZeroMemory(&lt, sizeof(lt));
        SystemTimeToTzSpecificLocalTime(NULL, &st, &lt);

        return systemTimeToString(lt);
    }

    static std::wstring Utils::systemTimeToString(SYSTEMTIME st)
    {
        const int nMaxLength = 1024;
        wchar_t buffer[nMaxLength];
        std::wstringstream sDateTime;

        if( GetDateFormatW( LOCALE_USER_DEFAULT, 0, &st, NULL, buffer, nMaxLength))
        {
            sDateTime << buffer;
        }

        sDateTime << L" ";

        if( GetTimeFormatW( LOCALE_USER_DEFAULT, NULL, &st, NULL, buffer, nMaxLength))
        {
            sDateTime << buffer;
        }

        return sDateTime.str();
    }

    static void tokenize(const std::wstring& str, std::vector<std::wstring>& tokens,
        const std::wstring& delimiters)
    {
        // Skip delimiters at beginning.
        std::wstring::size_type lastPos = str.find_first_not_of(delimiters, 0);

        // Find first "non-delimiter".
        std::wstring::size_type pos  = str.find_first_of(delimiters, lastPos);

        while (std::wstring::npos != pos || std::wstring::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));

            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of(delimiters, pos);

            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
    }

    static void ShellProperties(HWND hWnd, const std::wstring &sPath, const std::wstring &sFilename);
    static void ShellOpen(HWND hWnd, const std::wstring &sPath, const std::wstring &sFilename);

private:
    static std::wstring m_sComputerName;
};
