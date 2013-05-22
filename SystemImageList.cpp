#include "StdAfx.h"
#include "SystemImageList.h"

CSystemImageList::CSystemImageList(void)
{

}

CSystemImageList::~CSystemImageList(void)
{
    release();
}

CSystemImageList* CSystemImageList::instance()
{
    static CSystemImageList imageList;
    return &imageList;
}

void CSystemImageList::loadList()
{
    // Setup Image List
    SHFILEINFO ssfi;
    ZeroMemory(&ssfi, sizeof(ssfi));

    // Get a handle to the system small icon list
    HIMAGELIST hSystemImageList = (HIMAGELIST)SHGetFileInfo(L"C:\\", 0,
        &ssfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    // Attach it to the small image list
    m_imageList.Attach(hSystemImageList);
}

void CSystemImageList::loadList(const std::wstring &sPath, DWORD nFlags)
{
    // Setup Image List
    SHFILEINFO ssfi;
    ZeroMemory(&ssfi, sizeof(ssfi));

    // Get a handle to the system small icon list
    HIMAGELIST hSystemImageList = (HIMAGELIST)SHGetFileInfo(sPath.c_str(), 0,
        &ssfi, sizeof(SHFILEINFO), nFlags);

    // Attach it to the small image list
    m_imageList.Attach(hSystemImageList);

    if(ssfi.hIcon != NULL)
        DestroyIcon(ssfi.hIcon);
}

void CSystemImageList::release()
{
    m_imageList.Detach();
}

CImageList* CSystemImageList::getImageList()
{
    return &m_imageList;
}