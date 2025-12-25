// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_MyDrawBoardView.cpp
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（双缓冲/交互/命令模式）。 
// =================================================================================
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MyDrawBoard.h"
#endif

#include "Core_MyDrawBoardView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMyDrawBoardView, CView)

BEGIN_MESSAGE_MAP(CMyDrawBoardView, CView)
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)

    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_KEYDOWN()

    ON_COMMAND(ID_TOOL_LINE, &CMyDrawBoardView::OnToolLine)
    ON_COMMAND(ID_TOOL_RECT, &CMyDrawBoardView::OnToolRect)
    ON_COMMAND(ID_TOOL_ELLIPSE, &CMyDrawBoardView::OnToolEllipse)
    ON_COMMAND(ID_TOOL_FREEHAND, &CMyDrawBoardView::OnToolFreehand)
    ON_COMMAND(ID_TOOL_COLOR, &CMyDrawBoardView::OnToolColor)

    ON_UPDATE_COMMAND_UI(ID_TOOL_LINE, &CMyDrawBoardView::OnUpdateToolLine)
    ON_UPDATE_COMMAND_UI(ID_TOOL_RECT, &CMyDrawBoardView::OnUpdateToolRect)
    ON_UPDATE_COMMAND_UI(ID_TOOL_ELLIPSE, &CMyDrawBoardView::OnUpdateToolEllipse)
    ON_UPDATE_COMMAND_UI(ID_TOOL_FREEHAND, &CMyDrawBoardView::OnUpdateToolFreehand)

    ON_COMMAND(ID_WIDTH_1, &CMyDrawBoardView::OnWidth1)
    ON_COMMAND(ID_WIDTH_3, &CMyDrawBoardView::OnWidth3)
    ON_COMMAND(ID_WIDTH_5, &CMyDrawBoardView::OnWidth5)
    ON_COMMAND(ID_WIDTH_8, &CMyDrawBoardView::OnWidth8)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_1, &CMyDrawBoardView::OnUpdateWidth1)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_3, &CMyDrawBoardView::OnUpdateWidth3)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_5, &CMyDrawBoardView::OnUpdateWidth5)
    ON_UPDATE_COMMAND_UI(ID_WIDTH_8, &CMyDrawBoardView::OnUpdateWidth8)

    ON_COMMAND(ID_LANG_CN, &CMyDrawBoardView::OnLangCN)
    ON_COMMAND(ID_LANG_EN, &CMyDrawBoardView::OnLangEN)
    ON_COMMAND(ID_LANG_JP, &CMyDrawBoardView::OnLangJP)
    ON_UPDATE_COMMAND_UI(ID_LANG_CN, &CMyDrawBoardView::OnUpdateLangCN)
    ON_UPDATE_COMMAND_UI(ID_LANG_EN, &CMyDrawBoardView::OnUpdateLangEN)
    ON_UPDATE_COMMAND_UI(ID_LANG_JP, &CMyDrawBoardView::OnUpdateLangJP)
END_MESSAGE_MAP()

CMyDrawBoardView::CMyDrawBoardView() noexcept {
    m_renderer = std::make_unique<GdiRenderStrategy>(); // Strategy 默认实现
}

CMyDrawBoardView::~CMyDrawBoardView() {}

BOOL CMyDrawBoardView::PreCreateWindow(CREATESTRUCT& cs) { return CView::PreCreateWindow(cs); }

void CMyDrawBoardView::OnDraw(CDC* pDC)
{
    auto* pDoc = GetDocument();
    if (!pDoc) return;

    CRect rc;
    GetClientRect(&rc);

    // Double buffering
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);

    CBitmap memBmp;
    memBmp.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());
    CObject* oldBmp = memDC.SelectObject(&memBmp);

    memDC.FillSolidRect(rc, RGB(255, 255, 255));

    m_renderer->Render(&memDC, pDoc->GetShapes(), m_tempShape);

    pDC->BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(oldBmp);
}

BOOL CMyDrawBoardView::OnPreparePrinting(CPrintInfo* pInfo) { return DoPreparePrinting(pInfo); }
void CMyDrawBoardView::OnBeginPrinting(CDC*, CPrintInfo*) {}
void CMyDrawBoardView::OnEndPrinting(CDC*, CPrintInfo*) {}

#ifdef _DEBUG
void CMyDrawBoardView::AssertValid() const { CView::AssertValid(); }
void CMyDrawBoardView::Dump(CDumpContext& dc) const { CView::Dump(dc); }
CMyDrawBoardDoc* CMyDrawBoardView::GetDocument() const {
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyDrawBoardDoc)));
    return (CMyDrawBoardDoc*)m_pDocument;
}
#endif

CString CMyDrawBoardView::MakeCoordText(CPoint pt) const
{
    CString s;
    if (m_currentLang == AppLang::English) s.Format(_T("Pos: %d, %d"), pt.x, pt.y);
    else if (m_currentLang == AppLang::Japanese) s.Format(_T("座標: %d, %d"), pt.x, pt.y);
    else s.Format(_T("坐标: %d, %d"), pt.x, pt.y);
    return s;
}

// Interaction
void CMyDrawBoardView::OnLButtonDown(UINT nFlags, CPoint point)
{
    m_isDrawing = true;
    SetCapture();

    m_tempShape = ShapeFactory::Create(m_currentTool);
    if (m_tempShape) {
        m_tempShape->SetColor(m_currentColor);
        m_tempShape->SetLineWidth(m_currentLineWidth);
        m_tempShape->SetStartPoint(point);
        m_tempShape->SetEndPoint(point);
    }

    CView::OnLButtonDown(nFlags, point);
}

void CMyDrawBoardView::OnMouseMove(UINT nFlags, CPoint point)
{
    // status bar coord
    if (auto* frame = (CMainFrame*)AfxGetMainWnd()) {
        frame->SetCoordText(MakeCoordText(point));
    }

    if (m_isDrawing && (nFlags & MK_LBUTTON) && m_tempShape) {
        m_tempShape->SetEndPoint(point); // Freehand will append points
        Invalidate(FALSE);
    }

    CView::OnMouseMove(nFlags, point);
}

void CMyDrawBoardView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_isDrawing && m_tempShape) {
        m_tempShape->SetEndPoint(point);

        m_isDrawing = false;
        ReleaseCapture();

        auto* doc = GetDocument();
        if (doc) {
            auto cmd = std::make_shared<CAddShapeCommand>(m_tempShape, doc->GetShapesMutable());
            doc->Execute(cmd);                 // ✅ doc owns command history
        }

        m_tempShape.reset();
        Invalidate(FALSE);
    }
    CView::OnLButtonUp(nFlags, point);
}

void CMyDrawBoardView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    auto* doc = GetDocument();
    if (doc && GetKeyState(VK_CONTROL) < 0) {
        if (nChar == 'Z') { doc->Undo(); Invalidate(FALSE); }
        else if (nChar == 'Y') { doc->Redo(); Invalidate(FALSE); }
    }
    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// tools
void CMyDrawBoardView::OnToolLine() { m_currentTool = ShapeType::Line; }
void CMyDrawBoardView::OnToolRect() { m_currentTool = ShapeType::Rectangle; }
void CMyDrawBoardView::OnToolEllipse() { m_currentTool = ShapeType::Ellipse; }
void CMyDrawBoardView::OnToolFreehand() { m_currentTool = ShapeType::Freehand; }

void CMyDrawBoardView::OnToolColor() {
    CColorDialog dlg(m_currentColor);
    if (dlg.DoModal() == IDOK) m_currentColor = dlg.GetColor();
}

// tool highlight
void CMyDrawBoardView::OnUpdateToolLine(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentTool == ShapeType::Line); }
void CMyDrawBoardView::OnUpdateToolRect(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentTool == ShapeType::Rectangle); }
void CMyDrawBoardView::OnUpdateToolEllipse(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentTool == ShapeType::Ellipse); }
void CMyDrawBoardView::OnUpdateToolFreehand(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentTool == ShapeType::Freehand); }

// widths
void CMyDrawBoardView::OnWidth1() { m_currentLineWidth = 1; }
void CMyDrawBoardView::OnWidth3() { m_currentLineWidth = 3; }
void CMyDrawBoardView::OnWidth5() { m_currentLineWidth = 5; }
void CMyDrawBoardView::OnWidth8() { m_currentLineWidth = 8; }
void CMyDrawBoardView::OnUpdateWidth1(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 1); }
void CMyDrawBoardView::OnUpdateWidth3(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 3); }
void CMyDrawBoardView::OnUpdateWidth5(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 5); }
void CMyDrawBoardView::OnUpdateWidth8(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLineWidth == 8); }

// language
void CMyDrawBoardView::OnLangCN() { m_currentLang = AppLang::Chinese; UpdateMenuText(); }
void CMyDrawBoardView::OnLangEN() { m_currentLang = AppLang::English; UpdateMenuText(); }
void CMyDrawBoardView::OnLangJP() { m_currentLang = AppLang::Japanese; UpdateMenuText(); }
void CMyDrawBoardView::OnUpdateLangCN(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLang == AppLang::Chinese); }
void CMyDrawBoardView::OnUpdateLangEN(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLang == AppLang::English); }
void CMyDrawBoardView::OnUpdateLangJP(CCmdUI* pCmdUI) { pCmdUI->SetCheck(m_currentLang == AppLang::Japanese); }

void CMyDrawBoardView::UpdateMenuText()
{
    CMenu* pMenu = AfxGetMainWnd()->GetMenu();
    if (!pMenu) return;

    if (m_currentLang == AppLang::English) {
        pMenu->ModifyMenu(ID_FILE_NEW, MF_BYCOMMAND | MF_STRING, ID_FILE_NEW, _T("New"));
        pMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND | MF_STRING, ID_FILE_OPEN, _T("Open"));
        pMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND | MF_STRING, ID_FILE_SAVE, _T("Save"));

        pMenu->ModifyMenu(ID_TOOL_LINE, MF_BYCOMMAND | MF_STRING, ID_TOOL_LINE, _T("Line"));
        pMenu->ModifyMenu(ID_TOOL_RECT, MF_BYCOMMAND | MF_STRING, ID_TOOL_RECT, _T("Rectangle"));
        pMenu->ModifyMenu(ID_TOOL_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_TOOL_ELLIPSE, _T("Ellipse"));
        pMenu->ModifyMenu(ID_TOOL_FREEHAND, MF_BYCOMMAND | MF_STRING, ID_TOOL_FREEHAND, _T("Freehand"));
        pMenu->ModifyMenu(ID_TOOL_COLOR, MF_BYCOMMAND | MF_STRING, ID_TOOL_COLOR, _T("Color..."));

        pMenu->ModifyMenu(ID_WIDTH_1, MF_BYCOMMAND | MF_STRING, ID_WIDTH_1, _T("1 px"));
        pMenu->ModifyMenu(ID_WIDTH_3, MF_BYCOMMAND | MF_STRING, ID_WIDTH_3, _T("3 px"));
        pMenu->ModifyMenu(ID_WIDTH_5, MF_BYCOMMAND | MF_STRING, ID_WIDTH_5, _T("5 px"));
        pMenu->ModifyMenu(ID_WIDTH_8, MF_BYCOMMAND | MF_STRING, ID_WIDTH_8, _T("8 px"));

        AfxGetMainWnd()->SetWindowText(_T("MyDrawBoard - English Mode"));
    }
    else if (m_currentLang == AppLang::Japanese) {
        pMenu->ModifyMenu(ID_FILE_NEW, MF_BYCOMMAND | MF_STRING, ID_FILE_NEW, _T("新規作成"));
        pMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND | MF_STRING, ID_FILE_OPEN, _T("開く"));
        pMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND | MF_STRING, ID_FILE_SAVE, _T("保存"));

        pMenu->ModifyMenu(ID_TOOL_LINE, MF_BYCOMMAND | MF_STRING, ID_TOOL_LINE, _T("直線"));
        pMenu->ModifyMenu(ID_TOOL_RECT, MF_BYCOMMAND | MF_STRING, ID_TOOL_RECT, _T("長方形"));
        pMenu->ModifyMenu(ID_TOOL_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_TOOL_ELLIPSE, _T("楕円"));
        pMenu->ModifyMenu(ID_TOOL_FREEHAND, MF_BYCOMMAND | MF_STRING, ID_TOOL_FREEHAND, _T("自由曲線"));
        pMenu->ModifyMenu(ID_TOOL_COLOR, MF_BYCOMMAND | MF_STRING, ID_TOOL_COLOR, _T("色設定..."));

        pMenu->ModifyMenu(ID_WIDTH_1, MF_BYCOMMAND | MF_STRING, ID_WIDTH_1, _T("1 ピクセル"));
        pMenu->ModifyMenu(ID_WIDTH_3, MF_BYCOMMAND | MF_STRING, ID_WIDTH_3, _T("3 ピクセル"));
        pMenu->ModifyMenu(ID_WIDTH_5, MF_BYCOMMAND | MF_STRING, ID_WIDTH_5, _T("5 ピクセル"));
        pMenu->ModifyMenu(ID_WIDTH_8, MF_BYCOMMAND | MF_STRING, ID_WIDTH_8, _T("8 ピクセル"));

        AfxGetMainWnd()->SetWindowText(_T("MyDrawBoard - 日本語モード"));
    }
    else {
        pMenu->ModifyMenu(ID_FILE_NEW, MF_BYCOMMAND | MF_STRING, ID_FILE_NEW, _T("新建"));
        pMenu->ModifyMenu(ID_FILE_OPEN, MF_BYCOMMAND | MF_STRING, ID_FILE_OPEN, _T("打开"));
        pMenu->ModifyMenu(ID_FILE_SAVE, MF_BYCOMMAND | MF_STRING, ID_FILE_SAVE, _T("保存"));

        pMenu->ModifyMenu(ID_TOOL_LINE, MF_BYCOMMAND | MF_STRING, ID_TOOL_LINE, _T("直线"));
        pMenu->ModifyMenu(ID_TOOL_RECT, MF_BYCOMMAND | MF_STRING, ID_TOOL_RECT, _T("矩形"));
        pMenu->ModifyMenu(ID_TOOL_ELLIPSE, MF_BYCOMMAND | MF_STRING, ID_TOOL_ELLIPSE, _T("椭圆"));
        pMenu->ModifyMenu(ID_TOOL_FREEHAND, MF_BYCOMMAND | MF_STRING, ID_TOOL_FREEHAND, _T("自由曲线"));
        pMenu->ModifyMenu(ID_TOOL_COLOR, MF_BYCOMMAND | MF_STRING, ID_TOOL_COLOR, _T("选择颜色..."));

        pMenu->ModifyMenu(ID_WIDTH_1, MF_BYCOMMAND | MF_STRING, ID_WIDTH_1, _T("1 像素"));
        pMenu->ModifyMenu(ID_WIDTH_3, MF_BYCOMMAND | MF_STRING, ID_WIDTH_3, _T("3 像素"));
        pMenu->ModifyMenu(ID_WIDTH_5, MF_BYCOMMAND | MF_STRING, ID_WIDTH_5, _T("5 像素"));
        pMenu->ModifyMenu(ID_WIDTH_8, MF_BYCOMMAND | MF_STRING, ID_WIDTH_8, _T("8 像素"));

        AfxGetMainWnd()->SetWindowText(_T("MyDrawBoard - 中文模式"));
    }

    AfxGetMainWnd()->DrawMenuBar();
}
