// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_Shapes.h
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（工厂/序列化/异常安全）。 
// =================================================================================
#pragma once
#include <afxwin.h>
#include <memory>
#include <vector>
#include <cmath>
#include "Core_GdiUtils.h"

enum class ShapeType : int {
    Line = 1,
    Rectangle = 2,
    Ellipse = 3,
    Freehand = 4
};

class IShape {
public:
    virtual ~IShape() = default;

    virtual void SetStartPoint(CPoint pt) = 0;
    virtual void SetEndPoint(CPoint pt) = 0;
    virtual void Draw(CDC* pDC) = 0;
    virtual ShapeType GetType() const = 0;

    void SetColor(COLORREF c) { m_color = c; }
    void SetLineWidth(int w) { m_lineWidth = w; }

    virtual void Save(CArchive& ar) const { SaveBase(ar); }
    virtual void Load(CArchive& ar) { LoadBase(ar); }

protected:
    void SaveBase(CArchive& ar) const { ar << m_startPt << m_endPt << m_color << m_lineWidth; }
    void LoadBase(CArchive& ar) { ar >> m_startPt >> m_endPt >> m_color >> m_lineWidth; }

    CPoint   m_startPt{};
    CPoint   m_endPt{};
    COLORREF m_color = RGB(0, 0, 0);
    int      m_lineWidth = 1;
};

// ----- Line -----
class CLineShape : public IShape {
public:
    void SetStartPoint(CPoint pt) override { m_startPt = pt; }
    void SetEndPoint(CPoint pt) override { m_endPt = pt; }
    ShapeType GetType() const override { return ShapeType::Line; }

    void Draw(CDC* pDC) override {
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CGdiSelectGuard g(pDC, &pen);
        pDC->MoveTo(m_startPt);
        pDC->LineTo(m_endPt);
    }
};

// ----- Rect -----
class CRectShape : public IShape {
public:
    void SetStartPoint(CPoint pt) override { m_startPt = pt; }
    void SetEndPoint(CPoint pt) override { m_endPt = pt; }
    ShapeType GetType() const override { return ShapeType::Rectangle; }

    void Draw(CDC* pDC) override {
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CGdiSelectGuard gPen(pDC, &pen);

        // Null brush for hollow rect
        CBrush* oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(CRect(m_startPt, m_endPt));
        if (oldBrush) pDC->SelectObject(oldBrush);
    }
};

// ----- Ellipse -----
class CEllipseShape : public IShape {
public:
    void SetStartPoint(CPoint pt) override { m_startPt = pt; }
    void SetEndPoint(CPoint pt) override { m_endPt = pt; }
    ShapeType GetType() const override { return ShapeType::Ellipse; }

    void Draw(CDC* pDC) override {
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CGdiSelectGuard gPen(pDC, &pen);

        CBrush* oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
        pDC->Ellipse(CRect(m_startPt, m_endPt));
        if (oldBrush) pDC->SelectObject(oldBrush);
    }
};

// ----- Freehand -----
class CFreehandShape : public IShape {
public:
    ShapeType GetType() const override { return ShapeType::Freehand; }

    void SetStartPoint(CPoint pt) override {
        m_points.clear();
        m_points.push_back(pt);
        m_startPt = pt;
        m_endPt = pt;
    }

    void SetEndPoint(CPoint pt) override {
        if (!m_points.empty()) {
            const CPoint last = m_points.back();
            if (std::abs(last.x - pt.x) < 2 && std::abs(last.y - pt.y) < 2) return;
        }
        m_points.push_back(pt);
        m_endPt = pt;
    }

    void Draw(CDC* pDC) override {
        if (m_points.size() < 2) return;
        CPen pen(PS_SOLID, m_lineWidth, m_color);
        CGdiSelectGuard g(pDC, &pen);

        pDC->MoveTo(m_points[0]);
        for (size_t i = 1; i < m_points.size(); ++i) pDC->LineTo(m_points[i]);
    }

    void Save(CArchive& ar) const override {
        SaveBase(ar);
        int count = (int)m_points.size();
        ar << count;
        for (const auto& pt : m_points) ar << pt;
    }

    void Load(CArchive& ar) override {
        LoadBase(ar);
        int count = 0;
        ar >> count;

        constexpr int kMaxPoints = 200000;
        if (count < 0 || count > kMaxPoints) {
            AfxThrowArchiveException(CArchiveException::badSchema);
        }

        m_points.clear();
        m_points.reserve(count);
        for (int i = 0; i < count; ++i) {
            CPoint pt;
            ar >> pt;
            m_points.push_back(pt);
        }
    }

private:
    std::vector<CPoint> m_points;
};

// ----- Factory -----
class ShapeFactory {
public:
    static std::shared_ptr<IShape> Create(ShapeType type) {
        switch (type) {
        case ShapeType::Line:      return std::make_shared<CLineShape>();
        case ShapeType::Rectangle: return std::make_shared<CRectShape>();
        case ShapeType::Ellipse:   return std::make_shared<CEllipseShape>();
        case ShapeType::Freehand:  return std::make_shared<CFreehandShape>();
        default:                   return nullptr;
        }
    }
};
