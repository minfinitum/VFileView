// NtFileView.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "vfileview.h"
#include "mainfrm.h"

#include "vfileviewdoc.h"
#include "leftview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNtFileViewApp

BEGIN_MESSAGE_MAP(CVFileViewApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, &CVFileViewApp::OnAppAbout)
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CNtFileViewApp construction

CVFileViewApp::CVFileViewApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance

    m_pState = NULL;
}


// The one and only CNtFileViewApp object

CVFileViewApp theApp;


// CNtFileViewApp initialization

BOOL CVFileViewApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(REG_PATH);

    LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views
    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(CVFileViewDoc),
        RUNTIME_CLASS(CMainFrame),       // main SDI frame window
        RUNTIME_CLASS(CLeftView));
    if (!pDocTemplate)
        return FALSE;
    AddDocTemplate(pDocTemplate);



    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);


    // Dispatch commands specified on the command line.  Will return FALSE if
    // app was launched with /RegServer, /Register, /Unregserver or /Unregister.
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;


    // Restore Window State
    m_pState = NULL;
    m_pState = new WindowStateSaver(m_pMainWnd, REG_WINDOWS_STATE);
    m_pState->RestoreWindowState();



    // The one and only window has been initialized, so show and update it
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand

    return TRUE;
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CVFileViewApp::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

// CNtFileViewApp message handlers

void CVFileViewApp::GetStoredValues(WINDOWPLACEMENT& wp)
{
    if(m_pState != NULL)
        m_pState->GetStoredValues(wp);
}

void CVFileViewApp::RestoreWindowState()
{
    if(m_pState != NULL)
        m_pState->RestoreWindowState();
}

void CVFileViewApp::SaveWindowState()
{
    if(m_pState != NULL)
        m_pState->SaveWindowState();
}

int CVFileViewApp::ExitInstance()
{
    if(m_pState != NULL)
        delete m_pState;

    return CWinApp::ExitInstance();
}
