#pragma once

#include <string>
#include <vector>
#include <map>


#include "filesystem.h"
#include "defines.h"

// CFileListCtrl

#define			VEC_COLUMN_SIZE			std::vector<int>

class CFileListCtrl : public CListCtrl
{
    DECLARE_DYNAMIC(CFileListCtrl)

public:
    CFileListCtrl();
    virtual ~CFileListCtrl();

    void initialise();

    void addPaths(PathInfo &path, VEC_PATH &children);
    void addFilesFromPath(const std::wstring &sPath);
    void addFilesAndDirectoriesFromPath(const std::wstring &sPath);

    void addItem(const PathInfo &path);
    void addItem(const FileInfo &file, const FileInfo &stream);

    void refresh();
    void deleteSelectedFile(bool bOnReboot);
    void copySelectedFilePathToClipboard();


    PathInfo getSelectedItem();

    void OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult);

protected:
    void loadColumns();
    void saveColumns();

    void loadImageList();

    void deleteFile(const PathInfo &file, bool bOnReboot);
    void copyFilePathToClipboard(const PathInfo &file);
    void openFilePathInExplorer(const PathInfo &file);

    static int CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    CImageList m_imageListSystem;
    int m_nSortIndex;
    bool m_bSortDescend;

    std::wstring m_sPath;
    CString  m_regKey;
    VEC_COLUMN_SIZE m_column;
    VEC_PATH m_files;

protected:
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


