// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_MyDrawBoardView.h
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（交互/预览/多语言/双缓冲）。 
// =================================================================================
#pragma once
#include "Core_MyDrawBoardDoc.h"
#include "Core_RenderStrategy.h"

enum class AppLang {
    Chinese = 0,
    English = 1,
    Japanese = 2
};

class CMyDrawBoardView : public CView
{
protected:
    CMyDrawBoardView() noexcept;
    DECLARE_DYNCREATE(CMyDrawBoardView)

public:
    CMyDrawBoardDoc* GetDocument() const;

    // current state
    ShapeType m_currentTool = ShapeType::Line;
    COLORREF  m_currentColor = RGB(0, 0, 0);
    int       m_currentLineWidth = 1;
    AppLang   m_currentLang = AppLang::Chinese;

    std::shared_ptr<IShape> m_tempShape;
    bool m_isDrawing = false;

    std::unique_ptr<IRenderStrategy> m_renderer;

public:
    virtual void OnDraw(CDC* pDC) override;
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo) override;
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) override;
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) override;

public:
    virtual ~CMyDrawBoardView();
#ifdef _DEBUG
    virtual void AssertValid() const override;
    virtual void Dump(CDumpContext& dc) const override;
#endif

protected:
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

    // tools
    afx_msg void OnToolLine();
    afx_msg void OnToolRect();
    afx_msg void OnToolEllipse();
    afx_msg void OnToolFreehand();
    afx_msg void OnToolColor();

    // tool highlight
    afx_msg void OnUpdateToolLine(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolRect(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolEllipse(CCmdUI* pCmdUI);
    afx_msg void OnUpdateToolFreehand(CCmdUI* pCmdUI);

    // widths
    afx_msg void OnWidth1();
    afx_msg void OnWidth3();
    afx_msg void OnWidth5();
    afx_msg void OnWidth8();
    afx_msg void OnUpdateWidth1(CCmdUI* pCmdUI);
    afx_msg void OnUpdateWidth3(CCmdUI* pCmdUI);
    afx_msg void OnUpdateWidth5(CCmdUI* pCmdUI);
    afx_msg void OnUpdateWidth8(CCmdUI* pCmdUI);

    // language
    afx_msg void OnLangCN();
    afx_msg void OnLangEN();
    afx_msg void OnLangJP();
    afx_msg void OnUpdateLangCN(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLangEN(CCmdUI* pCmdUI);
    afx_msg void OnUpdateLangJP(CCmdUI* pCmdUI);

    void UpdateMenuText();
    CString MakeCoordText(CPoint pt) const;
};
