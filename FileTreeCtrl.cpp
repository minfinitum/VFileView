// FileTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VFileView.h"
#include "filetreectrl.h"
#include "filesystem.h"
#include "utils.h"
#include "clipboard.h"
#include "systemimagelist.h"


// CFileTreeCtrl
#define			EMPTYITEM				L"emptyitem"

#define			FILE_COVERT				0xFFFF


IMPLEMENT_DYNAMIC(CFileTreeCtrl, CTreeCtrl)
CFileTreeCtrl::CFileTreeCtrl()
{
}

CFileTreeCtrl::~CFileTreeCtrl()
{

}

BEGIN_MESSAGE_MAP(CFileTreeCtrl, CTreeCtrl)
    ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, &CFileTreeCtrl::OnTvnItemexpanding)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CFileTreeCtrl::OnNMCustomdraw)
    ON_WM_CONTEXTMENU()
    ON_NOTIFY_REFLECT(NM_RCLICK, &CFileTreeCtrl::OnNMRclick)
    ON_COMMAND(ID_FILE_REFRESH, &CFileTreeCtrl::OnFileRefresh)
    ON_COMMAND(ID_FILE_COPYPATH, &CFileTreeCtrl::OnFileCopyPathToClipboard)
    ON_COMMAND(ID_FILE_OPENPATH, &CFileTreeCtrl::OnOpenPathInExplorer)
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CFileTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()


// CFileTreeCtrl message handlers

void CFileTreeCtrl::initialise()
{
    loadImageList();

    populate();
}

void CFileTreeCtrl::loadImageList()
{
    SetImageList(CSystemImageList::instance()->getImageList(), LVSIL_NORMAL);
}

void CFileTreeCtrl::populate()
{
    SetRedraw(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    DeleteAllItems();

    CString sRootPath = Utils::getComputerName().c_str();
    m_hRoot = InsertItem(sRootPath, TVI_ROOT, TVI_LAST);
    setImage(m_hRoot, sRootPath, 0);

    CFileSystem fileio;
    VEC_DRIVE drives;
    if(fileio.getLogicalDrives(drives))
    {
        VEC_DRIVE::const_iterator iterDrive;
        for(iterDrive = drives.begin(); iterDrive != drives.end(); ++iterDrive)
        {
            HTREEITEM hItem = InsertItem(iterDrive->sPath.c_str(), m_hRoot, TVI_LAST);

            // Only insert an empty item if drive contains folders
            DWORD nSystemFlags = 0;
            if(GetVolumeInformation(iterDrive->sPath.c_str(), 0, 0, 0, 0, &nSystemFlags, 0, 0))
                if(fileio.hasDirectories(iterDrive->sPath.c_str()))
                    InsertItem(EMPTYITEM, hItem, TVI_LAST);

            CString sDrivePath = iterDrive->sPath.c_str();
            setImage(hItem, sDrivePath, 0);
        }
    }

    Expand(m_hRoot, TVE_EXPAND);

    SetCursor(LoadCursor (NULL, IDC_ARROW));
    SetRedraw(TRUE);
}

void CFileTreeCtrl::expandingItem(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    UINT uiAction = pNMTreeView->action;

    HTREEITEM hChildItem = GetChildItem(hItem);
    CString sItemText = GetItemText(hItem);
    CString sChildText = GetItemText(hChildItem);

    if(uiAction == TVE_EXPAND)
    {
        if(GetParentItem(hItem) != NULL)
            setImage(hItem, sItemText, uiAction);

        if(sChildText.CompareNoCase(EMPTYITEM) == 0)
        {
            // Prepare for update
            SetRedraw(FALSE);
            SetCursor(LoadCursor(NULL, IDC_WAIT));

            HTREEITEM hParent = GetParentItem(hItem);
            DeleteItem(hChildItem);

            std::wstring sPath;
            if(getPath(hItem, sPath))
            {
                CFileSystem fileio;
                HTREEITEM hNewItem = NULL;
                PathInfo info;

                bool bContinue = fileio.findFirst(sPath, info);

                while(bContinue)
                {
                    if(info.Attributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        hNewItem = InsertItem(info.sName.c_str(), hItem, TVI_LAST);
                        setImage(hNewItem, CString(info.sName.c_str()), uiAction);

                        if(fileio.hasDirectories(sPath + info.sName))
                        {
                            InsertItem(EMPTYITEM, hNewItem, TVI_LAST);
                        }

                        // Set item state to required colour
                        if(info.bCovert)
                            SetItemData(hNewItem, FILE_COVERT);
                        else
                            SetItemData(hNewItem, info.Attributes);
                    }

                    bContinue = fileio.findNext(info);
                }

                SortChildren(hItem);
                Expand(hItem, TVE_EXPAND);
            }

            // Update complete
            SetCursor(LoadCursor (NULL, IDC_ARROW));
            SetRedraw(TRUE);
        }
    }
    else if(uiAction == TVE_COLLAPSE)
    {
        if(GetParentItem(hItem) != NULL)
            setImage(hItem, sItemText, uiAction);
    }

    *pResult = 0;
}

void CFileTreeCtrl::setImage(HTREEITEM hItem, CString &sPath, DWORD uiAction)
{
    if(sPath.CompareNoCase(EMPTYITEM) == 0)
        return;

    if(hItem == m_hRoot)
    {
        // Load My Computer Icon
        LPITEMIDLIST pidl = NULL;
        SHGetSpecialFolderLocation(m_hWnd, CSIDL_DRIVES, &pidl);

        if (pidl != NULL)
        {
            SHFILEINFO shfi;
            ZeroMemory(&shfi, sizeof(shfi));

            LPMALLOC pMalloc;
            SHGetMalloc (&pMalloc);

            SHGetFileInfo((LPCTSTR)pidl, 0, &shfi, sizeof(shfi),
                SHGFI_PIDL | SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

            SetItemImage(hItem, shfi.iIcon, shfi.iIcon);

            if(pidl)
                pMalloc->Free (pidl);

            pMalloc->Release();
        }
    }
    else
    {
        SHFILEINFO shfi;
        ZeroMemory(&shfi, sizeof(shfi));

        SHGetFileInfo(sPath, FILE_ATTRIBUTE_DIRECTORY, &shfi, sizeof(shfi),
            SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

        if(shfi.hIcon != NULL)
            DestroyIcon(shfi.hIcon);

        SetItemImage(hItem, shfi.iIcon, shfi.iIcon);
    }

}

bool CFileTreeCtrl::getSelectedPath(std::wstring &sPath)
{
    bool bSuccess = false;
    HTREEITEM hItem = GetSelectedItem();
    if(hItem != NULL)
        bSuccess = getPath(hItem, sPath);
    return bSuccess;
}

std::wstring CFileTreeCtrl::getSelectedFullPath()
{
    HTREEITEM hItem = GetSelectedItem();
    std::wstring sPath;
    if(hItem != NULL)
    {
        if(getPath(hItem, sPath))
        {
            if(sPath.empty())
                sPath = Utils::getComputerName();
        }
    }
    return sPath;
}

void CFileTreeCtrl::setSelectedFullPath(const std::wstring &sPath)
{
    populate();

    SetRedraw(FALSE);
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    SelectItem(m_hRoot);
    Expand(m_hRoot, TVE_EXPAND);

    // Traverse Tree
    HTREEITEM hItem = m_hRoot;
    HTREEITEM hChildItem;
    HTREEITEM hNextItem;

    std::vector<std::wstring> tokens;
    std::vector<std::wstring>::const_iterator iterToken;
    Utils::tokenize(sPath, tokens, L"\\/");

    CString sItemValue;
    for(iterToken = tokens.begin(); iterToken != tokens.end(); ++iterToken)
    {
        hChildItem = GetChildItem(hItem);

        while (hChildItem != NULL)
        {
            hNextItem = GetNextItem(hChildItem, TVGN_NEXT);

            std::wstring sItemValue = GetItemText(hChildItem);
            std::wstring::size_type pos = sItemValue.find(L"\\", 0);
            if(pos != std::wstring::npos)
                sItemValue.erase(pos);

            if(sItemValue == *iterToken)
            {
                break;
            }

            hChildItem = hNextItem;
        }

        hItem = hChildItem;

        if(hItem != NULL)
        {
            // Found a matching item, select it and force expansion to populate tree
            SelectItem(hItem);
            Expand(hItem, TVE_EXPAND);
        }
        else
        {
            // Didn't find a matching item
            break;
        }
    }

    SetCursor(LoadCursor (NULL, IDC_ARROW));
    SetRedraw(TRUE);
}

bool CFileTreeCtrl::getPath(HTREEITEM hItem, std::wstring &sPath)
{
    HTREEITEM hCurrent = hItem;
    CString sItem;

    HKEY hKey = NULL;
    std::wstring sCurrentPath;
    bool bContinue = true;

    while(bContinue && hCurrent != NULL)
    {
        if(hCurrent == m_hRoot)
        {
            bContinue = false;
        }
        else
        {
            sItem = GetItemText(hCurrent);

            sCurrentPath = sPath;
            sPath = sItem.GetBuffer();

            if(!sCurrentPath.empty())
            {
                if(sPath[sPath.length() - 1] != L'\\')
                    sPath += L"\\" + sCurrentPath;
                else
                    sPath += sCurrentPath;
            }
        }

        hCurrent = GetParentItem(hCurrent);
    }

    if(!sPath.empty() && sPath[sPath.length() - 1] != L'\\')
        sPath += L"\\";

    return true;
}

void CFileTreeCtrl::onNMCustomdrawTree(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW* pCD = (NMLVCUSTOMDRAW*)pNMHDR;

    switch (pCD->nmcd.dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT:

        // Order is important

        pCD->clrText = COLOUR_NORMAL;

        if(pCD->nmcd.lItemlParam & FILE_ATTRIBUTE_HIDDEN)
        {
            pCD->clrText = COLOUR_HIDDEN;
        }

        if(pCD->nmcd.lItemlParam & FILE_ATTRIBUTE_SYSTEM)
        {
            pCD->clrText = COLOUR_SYSTEM;
        }

        if(pCD->nmcd.lItemlParam & FILE_ATTRIBUTE_COMPRESSED)
        {
            pCD->clrText = COLOUR_COMPRESS;
        }

        if(pCD->nmcd.lItemlParam == FILE_COVERT)
        {
            pCD->clrText = COLOUR_COVERT;
        }

        break;

    default:// it wasn't a notification that was interesting to us.
        *pResult = CDRF_DODEFAULT;
        break;
    }

}

void CFileTreeCtrl::OnTvnItemexpanding(NMHDR *pNMHDR, LRESULT *pResult)
{
    expandingItem(pNMHDR, pResult);
}

void CFileTreeCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    onNMCustomdrawTree(pNMHDR, pResult);
}

void CFileTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
    if (point.x == -1)
    {
        HTREEITEM ht = GetSelectedItem();
        RECT rect;

        GetItemRect( ht, &rect, true );
        ClientToScreen( &rect );

        /* Offset the popup menu origin so
        * we can read some of the text
        */

        point.x = rect.left + 15;
        point.y = rect.top + 8;
    }

    CMenu menu;
    menu.LoadMenu(IDR_MENU_TREEVIEW);

    CMenu * pop;
    pop = menu.GetSubMenu(0);

    UINT uCmd = pop->TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN |	TPM_RIGHTBUTTON, point.x, point.y, this, NULL );

    /* Menu item chosen ? */
    if (uCmd != 0)
    {
        switch(uCmd)
        {
            case ID_TREEVIEW_REFRESH:
                OnFileRefresh();
                break;

            case ID_TREEVIEW_COPYPATH:
                OnFileCopyPathToClipboard();
                break;

            case ID_TREEVIEW_OPENPATH:
                OnOpenPathInExplorer();
                break;

        }
    }

    menu.DestroyMenu();
}

void CFileTreeCtrl::OnNMRclick(NMHDR *pNMHDR, LRESULT *pResult)
{
    /* Get the mouse cursor position */
    DWORD dwPos = GetMessagePos();

    /* Convert the co-ords into a CPoint structure */
    CPoint pt(GET_X_LPARAM(dwPos), GET_Y_LPARAM(dwPos));
    CPoint spt = pt;

    /* Convert to screen co-ords for hittesting */
    ScreenToClient(&spt);

    UINT test;
    HTREEITEM hti = HitTest( spt, &test );

    /* Did the click occur on an item */
    if((hti != NULL) && (test & TVHT_ONITEM))
    {
        HTREEITEM htCur = GetSelectedItem();

        /* Do the context menu */
        OnContextMenu(this, pt);
    }

    *pResult = 0;
}

void CFileTreeCtrl::OnFileRefresh()
{
    std::wstring sFullPath = getSelectedFullPath();
    setSelectedFullPath(sFullPath);
}

void CFileTreeCtrl::OnFileCopyPathToClipboard()
{
    std::wstring sPath = getSelectedFullPath();
    CWnd *pWnd = GetParentOwner();

    CClipBoard cb;
    cb.Clear(*pWnd);
    cb.CopyTo(*pWnd, sPath);
}

void CFileTreeCtrl::OnOpenPathInExplorer()
{
    std::wstring sPath = getSelectedFullPath();
    Utils::ShellOpen(m_hWnd, sPath, L"");
}

void CFileTreeCtrl::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    *pResult = 0;
}
