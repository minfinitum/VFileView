#pragma once

#include <string>

class CClipBoard
{
public:
    CClipBoard(void);
    virtual ~CClipBoard(void);

    void Clear(const CWnd &wnd);
    void CopyTo(const CWnd &wnd, const std::wstring &sData);
};
