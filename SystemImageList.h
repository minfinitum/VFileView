#pragma once

#include <string>


#define		SYSTEMIMAGE_DRIVEHD		4


class CSystemImageList
{
public:
    CSystemImageList(void);
    virtual ~CSystemImageList(void);

    static CSystemImageList* instance();

    void loadList();
    void loadList(const std::wstring &sPath, DWORD nFlags);

    void release();

    CImageList* getImageList();

private:
    CImageList m_imageList;
};
