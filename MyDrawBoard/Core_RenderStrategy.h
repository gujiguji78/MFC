#pragma once
// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_RenderStrategy.h
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（Strategy 渲染解耦）。 
// =================================================================================
#pragma once
#include <memory>
#include <vector>
#include "Core_Shapes.h"

class IRenderStrategy {
public:
    virtual ~IRenderStrategy() = default;
    virtual void Render(CDC* dc,
        const std::vector<std::shared_ptr<IShape>>& shapes,
        const std::shared_ptr<IShape>& tempShape) = 0;
};

class GdiRenderStrategy final : public IRenderStrategy {
public:
    void Render(CDC* dc,
        const std::vector<std::shared_ptr<IShape>>& shapes,
        const std::shared_ptr<IShape>& tempShape) override
    {
        for (const auto& s : shapes) s->Draw(dc);
        if (tempShape) tempShape->Draw(dc);
    }
};
