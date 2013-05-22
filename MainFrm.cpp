// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "VFileView.h"

#include "mainfrm.h"
#include "leftview.h"
#include "rightview.h"
#include "systemimagelist.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


static UINT indicators[] =
{
    IDS_STATUSBAR_PATH
};


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
    ON_WM_CREATE()
    ON_WM_GETMINMAXINFO()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
    // Initalise the global image list
    CSystemImageList::instance()->loadList();
}

CMainFrame::~CMainFrame()
{

}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (!m_wndStatusBar.Create(this) ||
        !m_wndStatusBar.SetIndicators(indicators,
        sizeof(indicators)/sizeof(UINT)))
    {
        TRACE0("Failed to create status bar\n");
        return -1;      // fail to create
    }

    m_wndStatusBar.SetPaneInfo(0, 0, SBPS_STRETCH, 50);

    return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs,
    CCreateContext* pContext)
{
    // Create splitter window
    if (!m_wndSplitter.CreateStatic(this, 1, 2))
        return FALSE;

    if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CLeftView), CSize(300, 300), pContext) ||
        !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CRightView), CSize(300, 300), pContext))
    {
        m_wndSplitter.DestroyWindow();
        return FALSE;
    }

    return TRUE;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~FWS_ADDTOTITLE;

    if( !CFrameWnd::PreCreateWindow(cs) )
        return FALSE;

    return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize.x = 640;
    lpMMI->ptMinTrackSize.y = 480;

    // CFrameWnd::OnGetMinMaxInfo(lpMMI);
}

void CMainFrame::UpdateStatusBar(std::wstring &sPath)
{
    m_wndStatusBar.SetPaneText(0, sPath.c_str());
}

void CMainFrame::OnStatusBarView()
{
    m_wndStatusBar.ShowWindow((m_wndStatusBar.GetStyle() & WS_VISIBLE) == 0);
    RecalcLayout();
}

void CMainFrame::OnClose()
{
    CVFileViewApp *pApp = (CVFileViewApp *)AfxGetApp();
    pApp->SaveWindowState();

    CFrameWnd::OnClose();
}

