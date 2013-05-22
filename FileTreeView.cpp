// FileTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "VFileView.h"
#include "filetreeview.h"
#include "MainFrm.h"

// CFileTreeView

IMPLEMENT_DYNCREATE(CFileTreeView, CView)

CFileTreeView::CFileTreeView()
{

}

CFileTreeView::~CFileTreeView()
{
}

BEGIN_MESSAGE_MAP(CFileTreeView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CFileTreeView drawing

void CFileTreeView::OnDraw(CDC* pDC)
{
    CDocument* pDoc = GetDocument();
    // TODO: add draw code here
}


// CFileTreeView diagnostics

#ifdef _DEBUG
void CFileTreeView::AssertValid() const
{
    CView::AssertValid();
}

#ifndef _WIN32_WCE
void CFileTreeView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CFileTreeView message handlers

int CFileTreeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create the style
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP |
        LVS_REPORT | TVS_HASLINES | TVS_SHOWSELALWAYS;

    // Create the list control.  Don't worry about specifying
    // correct coordinates.  That will be handled in OnSize()
    BOOL bResult = m_fileTreeCtrl.Create(dwStyle, CRect(0,0,0,0), this, IDC_LEFTVIEW);

    return (bResult ? 0 : -1);
}

void CFileTreeView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    if (::IsWindow(m_fileTreeCtrl.m_hWnd))
        m_fileTreeCtrl.MoveWindow(0, 0, cx, cy, TRUE);
}

BOOL CFileTreeView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{

    if(wParam == IDC_LEFTVIEW)
    {
        // process tree view notifications
        NMHDR *pNHMDR = (NMHDR *)lParam;

        if(pNHMDR->code == TVN_SELCHANGED)
        {
            ((CMainFrame *)AfxGetMainWnd())->UpdateStatusBar(m_fileTreeCtrl.getSelectedFullPath());

            GetDocument()->UpdateAllViews(this, 0, &m_fileTreeCtrl);
        }

    }

    return CView::OnNotify(wParam, lParam, pResult);
}
