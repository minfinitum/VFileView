// FileListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VFileView.h"

#include "filelistctrl.h"
#include "utils.h"
#include "filesystem.h"
#include "mainfrm.h"
#include "clipboard.h"
#include "systemimagelist.h"


// CRegListCtrl
#define		FILE_NAME_INDEX					0
#define		FILE_NAME_TEXT					L"Name"

#define		FILE_ATTR_INDEX					1
#define		FILE_ATTR_TEXT					L"Attributes"

#define		FILE_SIZE_INDEX					2
#define		FILE_SIZE_TEXT					L"Size"

#define		FILE_CREATED_INDEX				3
#define		FILE_CREATED_TEXT				L"Created"

#define     FILE_LAST_ACCESSED_INDEX		4
#define		FILE_LAST_ACCESSED_TEXT			L"Last Accessed"

#define		FILE_LAST_WRITE_INDEX			5
#define		FILE_LAST_WRITE_TEXT			L"Last Write"

#define     FILE_CHANGED_INDEX				6
#define		FILE_CHANGED_TEXT				L"Last Changed"

#define		FILE_MAX				7

#define		CUSTOM_FILE_ATTRIBUTE_STREAM				0x10000000
#define		CUSTOM_FILE_ATTRIBUTE_COVERT				0x20000000

#include <strsafe.h>
#include "defines.h"


// CFileListCtrl

IMPLEMENT_DYNAMIC(CFileListCtrl, CListCtrl)
CFileListCtrl::CFileListCtrl()
:m_regKey(REG_LISTVIEW_STATE)
{
    m_nSortIndex = 0;
    m_bSortDescend = false;
}

CFileListCtrl::~CFileListCtrl()
{
    m_imageListSystem.Detach();

    saveColumns();
}

BEGIN_MESSAGE_MAP(CFileListCtrl, CListCtrl)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &CFileListCtrl::OnLvnItemchanged)
    ON_NOTIFY(HDN_ITEMCLICKA, 0, &CFileListCtrl::OnHdnItemclick)
    ON_NOTIFY(HDN_ITEMCLICKW, 0, &CFileListCtrl::OnHdnItemclick)
    ON_WM_CONTEXTMENU()
    ON_NOTIFY(HDN_ENDTRACKA, 0, &CFileListCtrl::OnHdnEndtrack)
    ON_NOTIFY(HDN_ENDTRACKW, 0, &CFileListCtrl::OnHdnEndtrack)
    ON_NOTIFY(HDN_ITEMCHANGEDA, 0, &CFileListCtrl::OnHdnItemchanged)
    ON_NOTIFY(HDN_ITEMCHANGEDW, 0, &CFileListCtrl::OnHdnItemchanged)
END_MESSAGE_MAP()

void CFileListCtrl::initialise()
{
    loadColumns();
    loadImageList();
}

void CFileListCtrl::loadColumns()
{
    // Update Expense List View
    CRect rect;
    GetClientRect(&rect);
    int nBarWidth = GetSystemMetrics(SM_CXVSCROLL);
    int nColumnWidth = (rect.right - rect.left - nBarWidth) / FILE_MAX;

    m_column.resize(FILE_MAX, nColumnWidth);

    m_column[FILE_NAME_INDEX] = AfxGetApp()->GetProfileInt(m_regKey, FILE_NAME_TEXT, nColumnWidth);
    InsertColumn(FILE_NAME_INDEX,	FILE_NAME_TEXT, LVCFMT_LEFT, m_column[FILE_NAME_INDEX]);

    m_column[FILE_ATTR_INDEX] = AfxGetApp()->GetProfileInt(m_regKey, FILE_ATTR_TEXT, nColumnWidth);
    InsertColumn(FILE_ATTR_INDEX,	FILE_ATTR_TEXT, LVCFMT_LEFT, m_column[FILE_ATTR_INDEX]);

    m_column[FILE_SIZE_INDEX] = AfxGetApp()->GetProfileInt(m_regKey, FILE_SIZE_TEXT, nColumnWidth);
    InsertColumn(FILE_SIZE_INDEX,	FILE_SIZE_TEXT, LVCFMT_LEFT, m_column[FILE_SIZE_INDEX]);

    m_column[FILE_CREATED_INDEX] = AfxGetApp()->GetProfileInt(m_regKey, FILE_CREATED_TEXT, nColumnWidth);
    InsertColumn(FILE_CREATED_INDEX, FILE_CREATED_TEXT, LVCFMT_LEFT, m_column[FILE_CREATED_INDEX]);

    m_column[FILE_LAST_ACCESSED_INDEX] = AfxGetApp()->GetProfileInt(m_regKey, FILE_LAST_ACCESSED_TEXT, nColumnWidth);
    InsertColumn(FILE_LAST_ACCESSED_INDEX, FILE_LAST_ACCESSED_TEXT, LVCFMT_LEFT, m_column[FILE_LAST_ACCESSED_INDEX]);

    m_column[FILE_LAST_WRITE_INDEX] = AfxGetApp()->GetProfileInt(m_regKey, FILE_LAST_WRITE_TEXT, nColumnWidth);
    InsertColumn(FILE_LAST_WRITE_INDEX, FILE_LAST_WRITE_TEXT, LVCFMT_LEFT, m_column[FILE_LAST_WRITE_INDEX]);

    m_column[FILE_CHANGED_INDEX] = AfxGetApp()->GetProfileInt(m_regKey, FILE_CHANGED_TEXT, nColumnWidth);
    InsertColumn(FILE_CHANGED_INDEX, FILE_CHANGED_TEXT, LVCFMT_LEFT, m_column[FILE_CHANGED_INDEX]);


    SetExtendedStyle(LVS_EX_INFOTIP | LVS_EX_FULLROWSELECT);
}

void CFileListCtrl::saveColumns()
{
    if(m_column.size() != FILE_MAX)
        return;

    AfxGetApp()->WriteProfileInt(m_regKey, FILE_NAME_TEXT, m_column[FILE_NAME_INDEX]);
    AfxGetApp()->WriteProfileInt(m_regKey, FILE_ATTR_TEXT, m_column[FILE_ATTR_INDEX]);
    AfxGetApp()->WriteProfileInt(m_regKey, FILE_SIZE_TEXT, m_column[FILE_SIZE_INDEX]);
    AfxGetApp()->WriteProfileInt(m_regKey, FILE_CREATED_TEXT, m_column[FILE_CREATED_INDEX]);
    AfxGetApp()->WriteProfileInt(m_regKey, FILE_LAST_ACCESSED_TEXT, m_column[FILE_LAST_ACCESSED_INDEX]);
    AfxGetApp()->WriteProfileInt(m_regKey, FILE_LAST_WRITE_TEXT, m_column[FILE_LAST_WRITE_INDEX]);
    AfxGetApp()->WriteProfileInt(m_regKey, FILE_CHANGED_TEXT, m_column[FILE_CHANGED_INDEX]);

}

void CFileListCtrl::loadImageList()
{
    SetImageList(CSystemImageList::instance()->getImageList(), LVSIL_SMALL);
}

void CFileListCtrl::addFilesAndDirectoriesFromPath(const std::wstring &sPath)
{
    SetRedraw(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    DeleteAllItems();

    m_sPath = sPath;

    CFileSystem fileio;
    bool more = false;
    bool first = true;
    do {
        PathInfo info;

        if(first) {
            more = fileio.findFirst(m_sPath, info);
            first = false;
        } else {
            more = fileio.findNext(info);
        }

        if(more) {
                addItem(info);
        }
    } while(more);

    SortItems(SortFunc, (DWORD_PTR)this);

    SetCursor(LoadCursor (NULL, IDC_ARROW));
    SetRedraw(TRUE);
}

void CFileListCtrl::addPaths(PathInfo &path, VEC_PATH &children)
{
    SetRedraw(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    DeleteAllItems();

    for(auto stream = path.streams.begin(); stream != path.streams.end(); ++stream) {
        addItem(path, *stream);
    }

    for(auto child = children.begin(); child != children.end(); ++child) {
        if(!(child->Attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            addItem(*child);
        }
    }

    SortItems(SortFunc, (DWORD_PTR)this);

    SetCursor(LoadCursor (NULL, IDC_ARROW));
    SetRedraw(TRUE);
}

void CFileListCtrl::addFilesFromPath(const std::wstring &sPath)
{
    SetRedraw(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    DeleteAllItems();

    m_sPath = sPath;

    CFileSystem fileio;

    PathInfo directory;
    if(fileio.queryDirectoryInfo(m_sPath, directory))
    {
        VEC_FILES::iterator iterStream;
        for(iterStream = directory.streams.begin(); iterStream != directory.streams.end(); ++iterStream)
            addItem(directory, *iterStream);
    }

    bool more = false;
    bool first = true;
    do {
        PathInfo info;

        if(first) {
            more = fileio.findFirst(m_sPath, info);
            first = false;
        } else {
            more = fileio.findNext(info);
        }

        if(more) {
            if(!(info.Attributes & FILE_ATTRIBUTE_DIRECTORY))
                addItem(info);
        }
    } while(more);

    SortItems(SortFunc, (DWORD_PTR)this);

    SetCursor(LoadCursor (NULL, IDC_ARROW));
    SetRedraw(TRUE);
}

void CFileListCtrl::addItem(const PathInfo &path)
{
    LVITEM lvItem;
    ZeroMemory(&lvItem, sizeof(lvItem));

    int nNextItem = GetItemCount();
    const int nMaxLen = 512;
    wchar_t szValue[nMaxLen] = L"";

    m_files.push_back(path);

    SHFILEINFO shfi;
    ZeroMemory(&shfi, sizeof(shfi));

    SHGetFileInfo(path.sPath.c_str(), path.Attributes, &shfi, sizeof(shfi),
        SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    if(shfi.hIcon != NULL)
        DestroyIcon(shfi.hIcon);

    // Path
    lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = shfi.iIcon;
    lvItem.iSubItem = FILE_NAME_INDEX;
    lvItem.lParam = (LPARAM) m_files.size() - 1;
    lvItem.pszText = (LPWSTR) path.sName.c_str();

    InsertItem(&lvItem);

    // Type
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_ATTR_INDEX;

    std::wstring sAttr = CFileSystem::attributesToString(path.Attributes);
    lvItem.pszText = (LPWSTR)sAttr.c_str();

    SetItem(&lvItem);

    // Size
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_SIZE_INDEX;
    lvItem.lParam = (LPARAM) 0;

    std::wstring sSize = CFileSystem::formatBytes(path.Size.QuadPart);
    lvItem.pszText = (LPWSTR) sSize.c_str();

    SetItem(&lvItem);

    // Creation
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_CREATED_INDEX;
    lvItem.lParam = (LPARAM) 0;

    std::wstring sCreation = Utils::systemTimeToLocalTimeString(path.CreationTime);
    lvItem.pszText = (LPWSTR) sCreation.c_str();

    SetItem(&lvItem);

    // Last Accessed
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_LAST_ACCESSED_INDEX;
    lvItem.lParam = (LPARAM) 0;

    std::wstring sLastAccess = Utils::systemTimeToLocalTimeString(path.LastAccessTime);
    lvItem.pszText = (LPWSTR) sLastAccess.c_str();

    SetItem(&lvItem);

    // Last Write
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_LAST_WRITE_INDEX;
    lvItem.lParam = (LPARAM) 0;

    std::wstring sLastWrite = Utils::systemTimeToLocalTimeString(path.LastWriteTime);
    lvItem.pszText = (LPWSTR) sLastWrite.c_str();

    SetItem(&lvItem);

    // Changed
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_CHANGED_INDEX;
    lvItem.lParam = (LPARAM) 0;

    std::wstring sChanged = Utils::systemTimeToLocalTimeString(path.ChangeTime);
    lvItem.pszText = (LPWSTR) sChanged.c_str();

    SetItem(&lvItem);

    if(!path.streams.empty())
    {
        VEC_FILES::const_iterator iterStream;
        for(iterStream = path.streams.begin(); iterStream != path.streams.end(); ++iterStream)
        {
            addItem(path, *iterStream);
        }
    }

    m_nSortIndex = 0;
    m_bSortDescend = false;
}

void CFileListCtrl::addItem(const FileInfo &file, const FileInfo &stream)
{
    LVITEM lvItem;
    ZeroMemory(&lvItem, sizeof(lvItem));

    int nNextItem = GetItemCount();

    const int nMaxLen = 512;
    wchar_t szValue[nMaxLen] = L"";

    // Impersonate file for sorting
    PathInfo filestream;
    filestream.sName = file.sName + stream.sName;
    filestream.sPath = stream.sPath;
    filestream.AllocationSize = stream.AllocationSize;
    filestream.Attributes = CUSTOM_FILE_ATTRIBUTE_STREAM;

    m_files.push_back(filestream);

    SHFILEINFO shfi;
    ZeroMemory(&shfi, sizeof(shfi));
    SHGetFileInfo(file.sPath.c_str(), file.Attributes, &shfi, sizeof(shfi),
        SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

    if(shfi.hIcon != NULL)
        DestroyIcon(shfi.hIcon);

    // Path
    lvItem.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = shfi.iIcon;
    lvItem.iSubItem = FILE_NAME_INDEX;
    lvItem.lParam = (LPARAM) m_files.size() - 1;
    lvItem.pszText = (LPWSTR) filestream.sName.c_str();

    InsertItem(&lvItem);

    // Type
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_ATTR_INDEX;

    std::wstring sAttr = L"stream";
    lvItem.pszText = (LPWSTR)sAttr.c_str();

    SetItem(&lvItem);

    // Size
    lvItem.mask = LVIF_TEXT;
    lvItem.state = 0;
    lvItem.stateMask = 0;

    lvItem.iItem = nNextItem;
    lvItem.iImage = 0;
    lvItem.iSubItem = FILE_SIZE_INDEX;
    lvItem.lParam = (LPARAM) 0;

    std::wstring sSize = CFileSystem::formatBytes(stream.Size.QuadPart);
    lvItem.pszText = (LPWSTR) sSize.c_str();

    SetItem(&lvItem);
}

void CFileListCtrl::OnNMCustomdrawList(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW* pCD = (NMLVCUSTOMDRAW*)pNMHDR;

    switch(pCD->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYSUBITEMDRAW;          // ask for subitem notifications.
        break;

    case CDDS_ITEMPREPAINT:
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
        {
            pCD->clrText = COLOUR_NORMAL;
            *pResult = CDRF_NEWFONT;

            // Order is important

            if(m_files[pCD->nmcd.lItemlParam].Attributes & FILE_ATTRIBUTE_HIDDEN)
            {
                pCD->clrText = COLOUR_HIDDEN;
                *pResult = CDRF_NEWFONT;
            }

            if(m_files[pCD->nmcd.lItemlParam].Attributes & FILE_ATTRIBUTE_SYSTEM)
            {
                pCD->clrText = COLOUR_SYSTEM;
                *pResult = CDRF_NEWFONT;
            }

            if(m_files[pCD->nmcd.lItemlParam].Attributes == CUSTOM_FILE_ATTRIBUTE_STREAM)
            {
                pCD->clrText = COLOUR_STREAM;
                *pResult = CDRF_NEWFONT;
            }

            if(m_files[pCD->nmcd.lItemlParam].Attributes & FILE_ATTRIBUTE_COMPRESSED)
            {
                pCD->clrText = COLOUR_COMPRESS;
                *pResult = CDRF_NEWFONT;
            }

            if(m_files[pCD->nmcd.lItemlParam].bCovert)
            {
                pCD->clrText = COLOUR_COVERT;
                *pResult = CDRF_NEWFONT;
            }

            break;
        }

    default:// it wasn't a notification that was interesting to us.
        *pResult = CDRF_DODEFAULT;
    }
}

PathInfo CFileListCtrl::getSelectedItem()
{
    int iItem = GetNextItem(-1, LVNI_SELECTED);
    if(iItem < 0)
        return PathInfo();

    DWORD_PTR nIndex = GetItemData(iItem);

    PathInfo nValue;
    if(nIndex >= 0 && nIndex < m_files.size())
        nValue = m_files[nIndex];

    return nValue;
}

void CFileListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    // Update Status Bar
    PathInfo info = getSelectedItem();
    ((CMainFrame *)AfxGetMainWnd())->UpdateStatusBar(info.sPath);

    *pResult = 0;
}

void CFileListCtrl::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);

    // Header clicked start sort
    NMLISTVIEW *pLV = (NMLISTVIEW *) pNMHDR;

    if(m_nSortIndex != pLV->iItem)
        m_bSortDescend = false;
    else
        m_bSortDescend = !m_bSortDescend;

    m_nSortIndex = pLV->iItem;
    SortItems(SortFunc, (DWORD_PTR)this);

    *pResult = 0;
}

int CALLBACK CFileListCtrl::SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    int nRetVal = 0;

    CFileListCtrl *pListCtrl = (CFileListCtrl *)lParamSort;

    PathInfo x = pListCtrl->m_files[lParam1];
    PathInfo y = pListCtrl->m_files[lParam2];

    // Don't compare entries unless same type (directory always wins over files)
    if(x.Attributes & FILE_ATTRIBUTE_DIRECTORY && !(y.Attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        nRetVal = -1;
    }
    else if(!(x.Attributes & FILE_ATTRIBUTE_DIRECTORY) && y.Attributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        nRetVal = 1;
    }
    else
    {

        switch(pListCtrl->m_nSortIndex)
        {
        case FILE_NAME_INDEX:
            {
                int nCompare = CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                    x.sName.c_str(), -1, y.sName.c_str(), -1);

                if(nCompare == CSTR_LESS_THAN)
                    nRetVal = -1;
                else if(nCompare == CSTR_EQUAL)
                    nRetVal = 0;
                else
                    nRetVal = 1;
            }
            break;

        case FILE_ATTR_INDEX:
            {
                std::wstring attribx = CFileSystem::attributesToString(x.Attributes);
                std::wstring attriby = CFileSystem::attributesToString(y.Attributes);

                int nCompare = CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNORECASE,
                    attribx.c_str(), -1, attriby.c_str(), -1);

                if(nCompare == CSTR_LESS_THAN)
                    nRetVal = -1;
                else if(nCompare == CSTR_EQUAL)
                    nRetVal = 0;
                else
                    nRetVal = 1;
            }
            break;

        case FILE_SIZE_INDEX:
            {
                if(x.Size.QuadPart > y.Size.QuadPart)
                    nRetVal = 1;
                else if(x.Size.QuadPart < y.Size.QuadPart)
                    nRetVal = -1;
                else
                    nRetVal = 0;
            }
            break;

        case FILE_CREATED_INDEX:
            {
                CTime timex(x.CreationTime);
                CTime timey(y.CreationTime);

                if(timex > timey)
                    nRetVal = 1;
                else if(timex < timey)
                    nRetVal = -1;
                else
                    nRetVal = 0;
            }
            break;

        case FILE_LAST_ACCESSED_INDEX:
            {
                CTime timex(x.LastAccessTime);
                CTime timey(y.LastAccessTime);

                if(timex > timey)
                    nRetVal = 1;
                else if(timex < timey)
                    nRetVal = -1;
                else
                    nRetVal = 0;
            }
            break;

        case FILE_LAST_WRITE_INDEX:
            {
                CTime timex(x.LastWriteTime);
                CTime timey(y.LastWriteTime);

                if(timex > timey)
                    nRetVal = 1;
                else if(timex < timey)
                    nRetVal = -1;
                else
                    nRetVal = 0;
            }
            break;

        case FILE_CHANGED_INDEX:
            {
                CTime timex(x.ChangeTime);
                CTime timey(y.ChangeTime);

                if(timex > timey)
                    nRetVal = 1;
                else if(timex < timey)
                    nRetVal = -1;
                else
                    nRetVal = 0;
            }
            break;
        }
    }

    if(pListCtrl->m_bSortDescend)
    {
        // Descending order swap results
        if(nRetVal > 0)
            nRetVal = -1;
        else if(nRetVal < 0)
            nRetVal = 1;
    }

    return nRetVal;
}

void CFileListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
    if (point.x == -1)
        return;

    // Ensure an item is selected
    int iItem = GetNextItem(-1, LVNI_SELECTED);
    if(iItem < 0)
        return;

    CMenu menu;
    menu.LoadMenu(IDR_MENU_FILELIST);

    CMenu * pop;
    pop = menu.GetSubMenu(0);

    UINT uCmd = pop->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN |	TPM_RIGHTBUTTON, point.x, point.y, this, NULL );

    /* Menu item chosen ? */
    if (uCmd != 0)
    {
        PathInfo file = getSelectedItem();

        switch(uCmd)
        {
        case ID_FILELIST_DELETE:
            deleteFile(file, false);
            break;

        case ID_FILELIST_DELETEONREBOOT:
            deleteFile(file, true);
            break;

        case ID_FILELIST_COPYPATH:
            copyFilePathToClipboard(file);
            break;

        case ID_FILELIST_OPENPATH:
            openFilePathInExplorer(file);
            break;
        }
    }

    menu.DestroyMenu();
}

void CFileListCtrl::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    HDITEM* pItem = phdr->pitem;

    // If this item mask indicates a change in width, then cxy holds that changed value
    if (pItem->mask & HDI_WIDTH)
        m_column[phdr->iItem] = pItem->cxy; // a zero-based index of the column that has changed

    *pResult = 0;
}

void CFileListCtrl::OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
    HDITEM* pItem = phdr->pitem;

    if (pItem->mask & HDI_WIDTH)
        m_column[phdr->iItem] = pItem->cxy; // a zero-based index of the column that has changed

    HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
    DefWindowProc(WM_NOTIFY, 0, LPARAM(pNMHDR));

    *pResult = 0;
}

void CFileListCtrl::deleteFile(const PathInfo &file, bool bOnReboot)
{
    std::wstring sMessage;

    if(bOnReboot)
        sMessage = L"Are you sure you want to delete the following file on reboot:\n";
    else
        sMessage = L"Are you sure you want to delete the following file:\n";

    sMessage += file.sName;

    if(AfxMessageBox(sMessage.c_str(), MB_YESNO, 0) == IDYES)
    {
        CFileSystem fs;
        bool bRetVal = false;

        if(bOnReboot)
            bRetVal = fs.deleteFileOnReboot(file.sPath);
        else
            bRetVal = fs.deleteFile(file.sPath);

        if(bRetVal)
        {
            if(bOnReboot)
                AfxMessageBox(L"File was marked for deletion.", MB_OK);
            else
                AfxMessageBox(L"File was deleted.", MB_OK);

            std::wstring::size_type pos = file.sPath.find_last_of(L"\\/");
            if(pos != std::wstring::npos)
                addFilesFromPath(file.sPath.substr(0, pos + 1));

        }
        else
        {
            AfxMessageBox(L"File was NOT deleted.", MB_OK);
        }
    }
}

void CFileListCtrl::copyFilePathToClipboard(const PathInfo &file)
{
    CClipBoard cb;
    cb.Clear(*this);
    cb.CopyTo(*this, file.sPath);
}

void CFileListCtrl::refresh()
{
    PathInfo selected = getSelectedItem();
    std::wstring::size_type pos = selected.sPath.find_last_of(L"\\/");
    if(pos != std::wstring::npos)
        addFilesFromPath(selected.sPath.substr(0, pos + 1));

    SetRedraw(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    LVFINDINFO fi;
    ZeroMemory(&fi, sizeof(fi));

    fi.flags = LVFI_STRING;
    fi.psz = selected.sName.c_str();

    int iItem = FindItem(&fi, -1);
    if(iItem != -1)
    {
        SetItemState(iItem, LVIS_SELECTED, LVIS_SELECTED);
    }

    SetCursor(LoadCursor (NULL, IDC_ARROW));
    SetRedraw(TRUE);

}

void CFileListCtrl::deleteSelectedFile(bool bOnReboot)
{
    deleteFile(getSelectedItem(), bOnReboot);
}

void CFileListCtrl::copySelectedFilePathToClipboard()
{
    copyFilePathToClipboard(getSelectedItem());
}

void CFileListCtrl::openFilePathInExplorer(const PathInfo &file)
{
    std::wstring sPath = file.sPath;
    std::wstring::size_type pos = sPath.find_last_of(L"\\/");
    if(pos != std::wstring::npos)
        sPath.erase(pos);

    Utils::ShellOpen(m_hWnd, sPath, L"");
}
