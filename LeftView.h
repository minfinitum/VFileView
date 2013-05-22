// LeftView.h : interface of the CLeftView class
//


#pragma once

#include "FileTreeView.h"

class CVFileViewDoc;

class CLeftView : public CFileTreeView
{
protected: // create from serialization only
    CLeftView();
    DECLARE_DYNCREATE(CLeftView)

// Attributes
public:
    CVFileViewDoc* GetDocument();

// Operations
public:

// Overrides
    public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    protected:
    virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
    virtual ~CLeftView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()

    afx_msg void OnFileRefresh();
    afx_msg void OnFileCopyPathToClipboard();
    afx_msg void OnOpenPathInExplorer();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CVFileViewDoc* CLeftView::GetDocument()
   { return reinterpret_cast<CVFileViewDoc*>(m_pDocument); }
#endif

