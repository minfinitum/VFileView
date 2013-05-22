#include "StdAfx.h"
#include "Utils.h"


std::wstring Utils::m_sComputerName;


Utils::Utils(void)
{
}

Utils::~Utils(void)
{
}


void Utils::ShellProperties(HWND hWnd, const std::wstring &sPath, const std::wstring &sFilename)
{
    SHELLEXECUTEINFO seInfo;
    ZeroMemory(&seInfo, sizeof(seInfo));

    seInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    seInfo.hwnd = hWnd;
    seInfo.lpVerb = L"properties";

    seInfo.lpFile = NULL;
    if(!sFilename.empty())
        seInfo.lpFile = sFilename.c_str();

    seInfo.lpFile = sFilename.c_str();
    seInfo.lpParameters = NULL;

    seInfo.lpDirectory = NULL;
    if(!sPath.empty())
        seInfo.lpDirectory = sPath.c_str();

    seInfo.nShow = SW_SHOW;

    seInfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_INVOKEIDLIST | SEE_MASK_UNICODE;

    ShellExecuteEx(&seInfo);
}


void Utils::ShellOpen(HWND hWnd, const std::wstring &sPath, const std::wstring &sFilename)
{
    SHELLEXECUTEINFO seInfo = { 0 };
    seInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    seInfo.hwnd = hWnd;
    seInfo.lpVerb = L"open";

    seInfo.lpFile = NULL;
    if(!sFilename.empty())
        seInfo.lpFile = sFilename.c_str();

    seInfo.lpParameters = NULL;

    seInfo.lpDirectory = NULL;
    if(!sPath.empty())
        seInfo.lpDirectory = sPath.c_str();

    seInfo.nShow = SW_SHOW;

    seInfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_UNICODE;

    ShellExecuteEx(&seInfo);
    if((int)seInfo.hInstApp <= 32)
    {
        seInfo.lpVerb = L"openas";
        seInfo.fMask = 0;

        ShellExecuteEx(&seInfo);
    }
}
