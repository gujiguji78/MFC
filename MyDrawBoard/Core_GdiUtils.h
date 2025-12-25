// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_GdiUtils.h
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（RAII/异常安全）。 
// =================================================================================
#pragma once
#include <afxwin.h>
#include <wingdi.h>

class CGdiSelectGuard final {
public:
    // 传入 MFC 的 CGdiObject（CPen/CBrush/CBitmap 都是）
    CGdiSelectGuard(CDC* dc, CGdiObject* obj)
        : m_dc(dc), m_old(nullptr)
    {
        if (m_dc && obj) {
            // 用 Win32 SelectObject，返回 HGDIOBJ，稳定不踩重载
            m_old = ::SelectObject(m_dc->GetSafeHdc(), (HGDIOBJ)obj->GetSafeHandle());
        }
    }

    ~CGdiSelectGuard()
    {
        if (m_dc && m_old) {
            ::SelectObject(m_dc->GetSafeHdc(), m_old);
        }
    }

    CGdiSelectGuard(const CGdiSelectGuard&) = delete;
    CGdiSelectGuard& operator=(const CGdiSelectGuard&) = delete;

private:
    CDC* m_dc = nullptr;
    HGDIOBJ m_old = nullptr;
};
