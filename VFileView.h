// NtFileView.h : main header file for the NtFileView application
//
#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CNtFileViewApp:
// See NtFileView.cpp for the implementation of this class
//

#include "windowstatesaver.h"

class CVFileViewApp : public CWinApp
{
public:
    CVFileViewApp();

// Overrides
public:
    virtual BOOL InitInstance();

    void GetStoredValues(WINDOWPLACEMENT& wp);
    void RestoreWindowState();
    void SaveWindowState();

// Implementation
    afx_msg void OnAppAbout();

    DECLARE_MESSAGE_MAP()

    virtual int ExitInstance();

private:
    WindowStateSaver *m_pState;
};

extern CVFileViewApp theApp;