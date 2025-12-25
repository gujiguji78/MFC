// MainFrm.h
#pragma once

class CMainFrame : public CFrameWnd
{
protected:
    CMainFrame() noexcept;
    DECLARE_DYNCREATE(CMainFrame)

public:
    void SetCoordText(const CString& text);   // ✅ 新增：坐标专栏

    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

    virtual ~CMainFrame();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    CToolBar   m_wndToolBar;
    CStatusBar m_wndStatusBar;

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};
