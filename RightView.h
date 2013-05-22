// RightView.h : interface of the CRightView class
//


#pragma once
#include "filelistctrl.h"
#include "fileedit.h"

#include "afxwin.h"
#include "filesystem.h"

class CRightView : public CFormView
{
protected: // create from serialization only
    CRightView();
    DECLARE_DYNCREATE(CRightView)

public:
    enum{ IDD = IDD_VFILEVIEW_FORM };

// Attributes
public:
    CVFileViewDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
    virtual ~CRightView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
public:
    CFileListCtrl m_list;
    CFileEdit m_edit;

protected:
    virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileRefresh();
    afx_msg void OnFileDelete();
    afx_msg void OnFileDeleteOnReboot();
    afx_msg void OnFileCopyPathToClipboard();
    afx_msg void OnOpenPathInExplorer();

protected:
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

    std::wstring m_sPath;
};

#ifndef _DEBUG  // debug version in RightView.cpp
inline CVFileViewDoc* CRightView::GetDocument() const
   { return reinterpret_cast<CVFileViewDoc*>(m_pDocument); }
#endif

