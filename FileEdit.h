#pragma once

#include "filesystem.h"

// CFileEdit

class CFileEdit : public CEdit
{
    DECLARE_DYNAMIC(CFileEdit)

public:
    CFileEdit();
    virtual ~CFileEdit();

    void initialise();

    void UpdatePathInfo(PathInfo &path, VEC_PATH &children);
    void UpdatePathInfo(const std::wstring &sPath);
    void UpdateFileInfo(const PathInfo &file);
    void ClearFileInfo();

    void SetBinary(unsigned char *pData, int nSize);

protected:
    CFont m_font;

protected:
    DECLARE_MESSAGE_MAP()
};


