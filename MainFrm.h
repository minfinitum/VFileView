// MainFrm.h : interface of the CMainFrame class
//

#pragma once

#include <string>

class CRightView;

#define		WM_UPDATE_STATUSBAR			(WM_APP + 5000)


class CMainFrame : public CFrameWnd
{
    // Implementation
public:
    virtual ~CMainFrame();
    CRightView* GetRightPane();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    void UpdateStatusBar(std::wstring &sPath);

    void OnStatusBarView();
    void OnUpdateViewStatusBar(CCmdUI* pCmdUI);


protected:
    CMainFrame();
    DECLARE_DYNCREATE(CMainFrame)

    CSplitterWnd m_wndSplitter;
    CStatusBar m_wndStatusBar;

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
    afx_msg void OnClose();

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


