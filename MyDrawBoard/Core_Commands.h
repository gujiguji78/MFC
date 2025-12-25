// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_Commands.h
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（命令模式正确性/异常安全）。 
// =================================================================================
#pragma once
#include <vector>
#include <memory>
#include <stack>
#include <algorithm>
#include "Core_Shapes.h"

class IDrawCommand {
public:
    virtual ~IDrawCommand() = default;
    virtual void Execute() = 0;
    virtual void Undo() = 0;
};

class CAddShapeCommand final : public IDrawCommand {
public:
    CAddShapeCommand(std::shared_ptr<IShape> shape,
        std::vector<std::shared_ptr<IShape>>& canvas)
        : m_shape(std::move(shape)), m_canvas(canvas) {
    }

    void Execute() override {
        if (!m_executedOnce) {
            m_insertIndex = m_canvas.size();
            m_canvas.push_back(m_shape);
            m_executedOnce = true;
            return;
        }

        // Redo: insert back
        if (m_insertIndex <= m_canvas.size()) {
            m_canvas.insert(m_canvas.begin() + (ptrdiff_t)m_insertIndex, m_shape);
        }
        else {
            m_canvas.push_back(m_shape);
            m_insertIndex = m_canvas.size() - 1;
        }
    }

    void Undo() override {
        if (m_canvas.empty()) return;

        // Prefer exact index removal
        if (m_insertIndex < m_canvas.size() && m_canvas[m_insertIndex] == m_shape) {
            m_canvas.erase(m_canvas.begin() + (ptrdiff_t)m_insertIndex);
            return;
        }

        // Fallback: find and remove
        auto it = std::find(m_canvas.begin(), m_canvas.end(), m_shape);
        if (it != m_canvas.end()) {
            m_insertIndex = (size_t)std::distance(m_canvas.begin(), it);
            m_canvas.erase(it);
        }
    }

private:
    std::shared_ptr<IShape> m_shape;
    std::vector<std::shared_ptr<IShape>>& m_canvas;

    size_t m_insertIndex = 0;
    bool m_executedOnce = false;
};

class CommandManager final {
public:
    void ExecuteCommand(const std::shared_ptr<IDrawCommand>& cmd) {
        if (!cmd) return;
        cmd->Execute();
        m_undo.push(cmd);
        while (!m_redo.empty()) m_redo.pop();
    }

    void Undo() {
        if (m_undo.empty()) return;
        auto cmd = m_undo.top(); m_undo.pop();
        cmd->Undo();
        m_redo.push(cmd);
    }

    void Redo() {
        if (m_redo.empty()) return;
        auto cmd = m_redo.top(); m_redo.pop();
        cmd->Execute();
        m_undo.push(cmd);
    }

    void Clear() {
        while (!m_undo.empty()) m_undo.pop();
        while (!m_redo.empty()) m_redo.pop();
    }

    bool CanUndo() const noexcept { return !m_undo.empty(); }
    bool CanRedo() const noexcept { return !m_redo.empty(); }

private:
    std::stack<std::shared_ptr<IDrawCommand>> m_undo;
    std::stack<std::shared_ptr<IDrawCommand>> m_redo;
};
