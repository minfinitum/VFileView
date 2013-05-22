// RightView.cpp : implementation of the CRightView class
//

#include "stdafx.h"
#include "VFileView.h"

#include "VFileViewdoc.h"
#include "rightview.h"

#include "filetreectrl.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRightView

IMPLEMENT_DYNCREATE(CRightView, CFormView)

BEGIN_MESSAGE_MAP(CRightView, CFormView)
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_REFRESH, &CRightView::OnFileRefresh)
    ON_COMMAND(ID_FILE_COPYPATH, &CRightView::OnFileCopyPathToClipboard)
    ON_COMMAND(ID_FILE_OPENPATH, &CRightView::OnOpenPathInExplorer)
    ON_COMMAND(ID_FILE_DELETE, &CRightView::OnFileDelete)
    ON_COMMAND(ID_FILE_DELETEONREBOOT, &CRightView::OnFileDeleteOnReboot)
END_MESSAGE_MAP()

// CRightView construction/destruction

CRightView::CRightView()
    : CFormView(CRightView::IDD)
{

}

CRightView::~CRightView()
{
}

void CRightView::DoDataExchange(CDataExchange* pDX)
{
    CFormView::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_list);
    DDX_Control(pDX, IDC_EDIT, m_edit);
}

BOOL CRightView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CFormView::PreCreateWindow(cs);
}

void CRightView::OnInitialUpdate()
{
    CFormView::OnInitialUpdate();
    GetParentFrame()->RecalcLayout();
    ResizeParentToFit();

    m_list.initialise();
    m_edit.initialise();
}


// CRightView diagnostics

#ifdef _DEBUG
void CRightView::AssertValid() const
{
    CFormView::AssertValid();
}

void CRightView::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}

CVFileViewDoc* CRightView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CVFileViewDoc)));
    return (CVFileViewDoc*)m_pDocument;
}
#endif //_DEBUG


// CRightView message handlers

void CRightView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    if(pSender == NULL || pHint == NULL)
        return;

    if(pHint->IsKindOf(RUNTIME_CLASS(CFileTreeCtrl)))
    {
        CFileTreeCtrl *pTree = (CFileTreeCtrl *)pHint;
        m_sPath = pTree->getSelectedFullPath();

        CFileSystem fs;
        PathInfo path;
        if(fs.queryDirectoryInfo(m_sPath, path)) {

            m_list.DeleteAllItems();
            m_edit.ClearFileInfo();

            VEC_PATH children;
            if(fs.getFilesAndDirectories(m_sPath, children)) {

                m_list.addPaths(path, children);
                m_edit.UpdatePathInfo(path, children);
            }
        }
    }
}

void CRightView::OnSize(UINT nType, int cx, int cy)
{
    CFormView::OnSize(nType, cx, cy);

    if(m_edit.GetSafeHwnd() == NULL || m_list.GetSafeHwnd() == NULL)
        return;

    CRect rectEdit;
    m_edit.GetWindowRect(&rectEdit);
    ScreenToClient(&rectEdit);

    // edit box is constant height
    static int nEditHeight = rectEdit.bottom - rectEdit.top;

    // Resize listview
    CRect rectList;
    m_list.GetWindowRect(&rectList);
    ScreenToClient(&rectList);

    m_list.MoveWindow(rectList.left, rectList.top, cx, cy - nEditHeight);

    // Resize editbox
    m_edit.GetWindowRect(&rectEdit);
    ScreenToClient(&rectEdit);

    m_edit.MoveWindow(rectEdit.left, cy - nEditHeight, cx, nEditHeight);
}

BOOL CRightView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    if(wParam == IDC_LIST)
    {
        LPNMHDR pNHMDR = (LPNMHDR) lParam;

        if(pNHMDR->code == NM_CUSTOMDRAW)
        {
            m_list.OnNMCustomdrawList(pNHMDR, pResult);
            return TRUE;
        }

        if(pNHMDR->code == LVN_ITEMCHANGED)
        {
            m_edit.UpdateFileInfo(m_list.getSelectedItem());
        }

        /*
        if(pNHMDR->code == NM_DBLCLK)
        {
            PathInfo path = m_fileList.getSelectedItem();
            if(path.Attributes & FILE_ATTRIBUTE_DIRECTORY)
                GetDocument()->UpdateAllViews(this, 0, &m_fileList);
        }
        */
    }

    return CFormView::OnNotify(wParam, lParam, pResult);
}

void CRightView::OnFileRefresh()
{
    m_list.refresh();
}

void CRightView::OnFileDelete()
{
    m_list.deleteSelectedFile(false);
}

void CRightView::OnFileDeleteOnReboot()
{
    m_list.deleteSelectedFile(true);
}

void CRightView::OnFileCopyPathToClipboard()
{
    m_list.copySelectedFilePathToClipboard();
}

void CRightView::OnOpenPathInExplorer()
{
    PathInfo file = m_list.getSelectedItem();

    Utils::ShellOpen(m_hWnd, file.sPath.c_str(), L"");
}
