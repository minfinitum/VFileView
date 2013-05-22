#include "StdAfx.h"
#include "ClipBoard.h"

#include <strsafe.h>

CClipBoard::CClipBoard(void)
{
}

CClipBoard::~CClipBoard(void)
{
}

void CClipBoard::Clear(const CWnd &wnd)
{
    if (OpenClipboard(wnd))
    {
        // clear clipboard
        EmptyClipboard();

        //close clipboard as we don't need it anymore
        CloseClipboard();
    }
}

void CClipBoard::CopyTo(const CWnd &wnd, const std::wstring &sData)
{
    //try to open clipboard first
    if (OpenClipboard(wnd))
    {
        HGLOBAL clipbuffer;
        //alloc enough mem for the string, must be GMEM_DDESHARE to work with the clipboard
        const int nMaxLen = ((int)sData.length() + 1) * sizeof(wchar_t);
        clipbuffer = GlobalAlloc(GMEM_DDESHARE, nMaxLen);
        if(nullptr != clipbuffer) {
            wchar_t * szBuffer = (wchar_t *) GlobalLock(clipbuffer);
            StringCchCopy(szBuffer, nMaxLen, sData.c_str());
            szBuffer[sData.length()] = NULL;
            GlobalUnlock(clipbuffer);

            //fill the clipboard with data
            ::SetClipboardData(CF_UNICODETEXT, clipbuffer);
        }

        //close clipboard as we don't need it anymore
        CloseClipboard();
    }
}