// LeftView.cpp : implementation of the CLeftView class
//

#include "stdafx.h"
#include "VFileView.h"
#include "VFileViewdoc.h"
#include "leftview.h"
#include "clipboard.h"
#include "filelistctrl.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLeftView

IMPLEMENT_DYNCREATE(CLeftView, CTreeView)

BEGIN_MESSAGE_MAP(CLeftView, CFileTreeView)
    ON_COMMAND(ID_FILE_REFRESH, &CLeftView::OnFileRefresh)
    ON_COMMAND(ID_FILE_COPYPATH, &CLeftView::OnFileCopyPathToClipboard)
    ON_COMMAND(ID_FILE_OPENPATH, &CLeftView::OnOpenPathInExplorer)
END_MESSAGE_MAP()


// CLeftView construction/destruction

CLeftView::CLeftView()
{

}

CLeftView::~CLeftView()
{
}

BOOL CLeftView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

    return CFileTreeView::PreCreateWindow(cs);
}

void CLeftView::OnInitialUpdate()
{
    CFileTreeView::OnInitialUpdate();

    m_fileTreeCtrl.initialise();
}


// CLeftView diagnostics

#ifdef _DEBUG
void CLeftView::AssertValid() const
{
    CFileTreeView::AssertValid();
}

void CLeftView::Dump(CDumpContext& dc) const
{
    CFileTreeView::Dump(dc);
}

CVFileViewDoc* CLeftView::GetDocument() // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVFileViewDoc)));
    return (CVFileViewDoc*)m_pDocument;
}
#endif //_DEBUG

// CLeftView message handlers

void CLeftView::OnFileRefresh()
{
    m_fileTreeCtrl.OnFileRefresh();
}

void CLeftView::OnFileCopyPathToClipboard()
{
    m_fileTreeCtrl.OnFileCopyPathToClipboard();
}

void CLeftView::OnOpenPathInExplorer()
{
    m_fileTreeCtrl.OnOpenPathInExplorer();
}

void CLeftView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if(pSender == NULL || pHint == NULL)
        return;

    if(pHint->IsKindOf(RUNTIME_CLASS(CFileListCtrl)))
    {
        CFileListCtrl *pList = (CFileListCtrl *)pHint;
        PathInfo path = pList->getSelectedItem();

        // Update the selected folder
        m_fileTreeCtrl.setSelectedFullPath(path.sPath);
    }
}
