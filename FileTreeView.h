#pragma once

#include "FileTreeCtrl.h"

// CFileTreeView view

class CFileTreeView : public CView
{
    DECLARE_DYNCREATE(CFileTreeView)

protected:
    CFileTreeView();           // protected constructor used by dynamic creation
    virtual ~CFileTreeView();

public:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
    virtual void AssertValid() const;
#ifndef _WIN32_WCE
    virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
    DECLARE_MESSAGE_MAP()

    CFileTreeCtrl m_fileTreeCtrl;

public:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


