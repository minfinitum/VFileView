#include "StdAfx.h"
#include "windowstatesaver.h"

WindowStateSaver::WindowStateSaver(CWnd* pWnd, const CString& regKey)
: m_pWnd(pWnd)
, m_RegKey(regKey)
{
    ASSERT(pWnd != NULL);
    ASSERT(!regKey.IsEmpty());
}

WindowStateSaver::~WindowStateSaver(void)
{
}

bool WindowStateSaver::RestoreWindowState()
{
    WINDOWPLACEMENT dwp; // default that MFC gives us
    dwp.length = sizeof WINDOWPLACEMENT;
    m_pWnd->GetWindowPlacement(&dwp);

    WINDOWPLACEMENT wp; // one we'll work on and maybe use.
    wp = dwp;

    // Grab the stored values, if any. If we fail to get a value,
    // then we don't do anything (just accept the defaults)
    if (GetStoredValues(wp))
    {
        wp.rcNormalPosition = ForceRectOnScreen(wp.rcNormalPosition);

        HandleWinMinMax(wp, dwp);

        m_pWnd->SetWindowPlacement(&wp);

        return TRUE;
    }
    return FALSE;
}

void WindowStateSaver::SaveWindowState()
{
    WINDOWPLACEMENT wp;
    wp.length = sizeof(WINDOWPLACEMENT);
    m_pWnd->GetWindowPlacement(&wp);

    RECT& rect(wp.rcNormalPosition);

    SetIntValue(_T("flags"), wp.flags);
    SetIntValue(_T("showCmd"), wp.showCmd);
    SetIntValue(_T("x1"), rect.left);
    SetIntValue(_T("y1"), rect.top);
    SetIntValue(_T("x2"), rect.right);
    SetIntValue(_T("y2"), rect.bottom);
}

bool WindowStateSaver::GetStoredValues(WINDOWPLACEMENT& wp)
{
    RECT& rect(wp.rcNormalPosition);

    if ((wp.flags = GetIntValue(_T("flags"), -1)) != -1 &&
        (wp.showCmd = GetIntValue(_T("showCmd"), -1)) != -1 &&
        (rect.left = GetIntValue(_T("x1"), -1)) != -1 &&
        (rect.top = GetIntValue(_T("y1"), -1)) != -1 &&
        (rect.right = GetIntValue(_T("x2"), -1)) != -1 &&
        (rect.bottom = GetIntValue(_T("y2"), -1)) != -1)
    {
        // got reasonable values for all
        return true;
    }

    // didn't get reasonable values.
    return false;
}

RECT WindowStateSaver::ForceRectOnScreen(const RECT& rect) const
{
    CRect tmp_rct;
    CRect scrn_rct;

    // ::SystemParametersInfo(SPI_GETWORKAREA) gives
    // us the screen space minus the system taskbar and
    // application toolbars, which is what we want our
    // application to fit into.
    ::SystemParametersInfo(SPI_GETWORKAREA, 0,
        &scrn_rct, 0);

    // Normalise the existing rectangle
    tmp_rct = rect;
    tmp_rct.NormalizeRect();

    // test the size first and make sure it's within
    // the limits of the screen size.
    CSize excess = tmp_rct.Size() - scrn_rct.Size();
    excess.cx = max(excess.cx, 0);
    excess.cy = max(excess.cy, 0);

    // get the size under control.
    tmp_rct -= CRect(CPoint(0,0), excess);

    // Now let's relocate the window so that it's all
    // within bounds.
    CPoint offset(scrn_rct.Size());

    offset -= tmp_rct.BottomRight();

    offset.x = min(0, offset.x);
    offset.y = min(0, offset.y);

    tmp_rct.OffsetRect(offset);

    offset = -tmp_rct.TopLeft();
    offset.x = max(0, offset.x);
    offset.y = max(0, offset.y);

    tmp_rct.OffsetRect(offset);

    return tmp_rct;
}

void WindowStateSaver::HandleWinMinMax(
                                       WINDOWPLACEMENT& wp, const WINDOWPLACEMENT& dwp) const
{
    // wp is the struct that is filled with info from the
    // registry and we will possibly modify.
    //
    // dwp is the struct that has the default values
    // that would be used by the framework and it contains
    // the minimize/maximize request that the app was started
    // with.
    if (dwp.showCmd == SW_SHOWMINIMIZED)
    {
        wp.showCmd = SW_SHOWMINIMIZED;
    }
    else if (dwp.showCmd == SW_SHOWMAXIMIZED)
    {
        wp.showCmd = SW_SHOWMAXIMIZED;
    }
    else if (wp.showCmd == SW_SHOWMINIMIZED)
    {
        wp.showCmd = SW_RESTORE;
    }
}

int WindowStateSaver::GetIntValue(const TCHAR* name, int def) const
{
    ASSERT(name != NULL);

    return AfxGetApp()->GetProfileInt(m_RegKey, name, def);
}

void WindowStateSaver::SetIntValue(const TCHAR* name, int val) const
{
    ASSERT(name != NULL);

    VERIFY(AfxGetApp()->WriteProfileInt(m_RegKey, name, val));
}

