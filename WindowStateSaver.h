#pragma once

class WindowStateSaver
{
public:
    WindowStateSaver(CWnd* pWnd, const CString& regKey);
    virtual ~WindowStateSaver(void);

    bool RestoreWindowState();
    void SaveWindowState();
    bool GetStoredValues(WINDOWPLACEMENT& wp);

protected:
    int GetIntValue(const TCHAR* name, int def) const;
    void SetIntValue(const TCHAR* name, int val) const;
    RECT ForceRectOnScreen(const RECT& rect) const;
    void HandleWinMinMax(WINDOWPLACEMENT& wp,
        const WINDOWPLACEMENT& dwp) const;
private:
    CWnd* m_pWnd;
    CString m_RegKey;
};
