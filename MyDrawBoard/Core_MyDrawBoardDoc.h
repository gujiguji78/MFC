// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_MyDrawBoardDoc.h
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（序列化/异常安全/Doc-Command 归属）。 
// =================================================================================
#pragma once
#include <vector>
#include <memory>
#include "Core_Shapes.h"
#include "Core_Commands.h"

class CMyDrawBoardDoc : public CDocument
{
protected:
    CMyDrawBoardDoc() noexcept;
    DECLARE_DYNCREATE(CMyDrawBoardDoc)

public:
    using ShapeList = std::vector<std::shared_ptr<IShape>>;

    const ShapeList& GetShapes() const noexcept { return m_shapes; }
    ShapeList& GetShapesMutable() noexcept { return m_shapes; }

    // Command owned by Doc (better SDI architecture)
    CommandManager& GetCommandManager() noexcept { return m_cmd; }

    void Execute(const std::shared_ptr<IDrawCommand>& cmd) {
        m_cmd.ExecuteCommand(cmd);
        SetModifiedFlag(TRUE);
    }
    void Undo() { m_cmd.Undo(); SetModifiedFlag(TRUE); }
    void Redo() { m_cmd.Redo(); SetModifiedFlag(TRUE); }
    void ClearHistory() { m_cmd.Clear(); }

public:
    virtual BOOL OnNewDocument() override;
    virtual void Serialize(CArchive& ar) override;

    virtual ~CMyDrawBoardDoc();
#ifdef _DEBUG
    virtual void AssertValid() const override;
    virtual void Dump(CDumpContext& dc) const override;
#endif

protected:
    DECLARE_MESSAGE_MAP()

private:
    ShapeList m_shapes;
    CommandManager m_cmd;
};
