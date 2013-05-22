#pragma once

#include <string>
#include "defines.h"


// CFileTreeCtrl


class CFileTreeCtrl : public CTreeCtrl
{
    DECLARE_DYNAMIC(CFileTreeCtrl)

public:
    CFileTreeCtrl();
    virtual ~CFileTreeCtrl();

    void initialise();
    std::wstring getSelectedFullPath();
    void setSelectedFullPath(const std::wstring &sPath);

private:
    void loadImageList();
    void populate();

    bool getSelectedPath(std::wstring &sPath);
    bool getPath(HTREEITEM hItem, std::wstring &sPath);

    void setImage(HTREEITEM hItem, CString &sPath, DWORD uiAction);

    void expandingItem(NMHDR *pNMHDR, LRESULT *pResult);
    void onNMCustomdrawTree(NMHDR *pNMHDR, LRESULT *pResult);

private:

    HTREEITEM m_hRoot;
    //CImageList m_imageList;
    //CImageList m_imageListSystem;

protected:
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

    afx_msg void OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult);

    afx_msg void OnFileRefresh();
    afx_msg void OnFileCopyPathToClipboard();
    afx_msg void OnOpenPathInExplorer();
    afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


