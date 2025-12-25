// =================================================================================
// 项目名称: MyDrawBoard (Project 2)
// 文件名称: Core_MyDrawBoardDoc.cpp
// AI 辅助声明: 本文件由 AI 辅助生成，已人工审核（自定义格式/强异常安全）。 
// =================================================================================
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "MyDrawBoard.h"
#endif

#include "Core_MyDrawBoardDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMyDrawBoardDoc, CDocument)
BEGIN_MESSAGE_MAP(CMyDrawBoardDoc, CDocument)
END_MESSAGE_MAP()

CMyDrawBoardDoc::CMyDrawBoardDoc() noexcept {}
CMyDrawBoardDoc::~CMyDrawBoardDoc() {}

BOOL CMyDrawBoardDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument()) return FALSE;

    m_shapes.clear();
    m_cmd.Clear();         // ✅ 新建必须清历史（F-04）
    SetModifiedFlag(FALSE);
    return TRUE;
}

void CMyDrawBoardDoc::Serialize(CArchive& ar)
{
    // 自定义二进制格式：Magic + Version + payload
    constexpr DWORD  kMagic = 0x4442444D; // 'MDBD' little-endian
    constexpr UINT32 kVersion = 1;

    try
    {
        if (ar.IsStoring())
        {
            ar << kMagic;
            ar << kVersion;

            int count = (int)m_shapes.size();
            ar << count;

            for (const auto& shape : m_shapes) {
                ar << (int)shape->GetType();
                shape->Save(ar);
            }
        }
        else
        {
            DWORD magic = 0;
            UINT32 version = 0;
            ar >> magic;
            ar >> version;

            if (magic != kMagic || version != kVersion) {
                AfxThrowArchiveException(CArchiveException::badSchema);
            }

            ShapeList temp;
            int count = 0;
            ar >> count;

            if (count < 0 || count > 100000) {
                AfxThrowArchiveException(CArchiveException::badSchema);
            }
            temp.reserve(count);

            for (int i = 0; i < count; ++i) {
                int typeId = 0;
                ar >> typeId;

                auto shape = ShapeFactory::Create((ShapeType)typeId);
                if (!shape) {
                    AfxThrowArchiveException(CArchiveException::badClass);
                }
                shape->Load(ar);
                temp.push_back(std::move(shape));
            }

            // ✅ 强异常安全：全部成功才替换
            m_shapes = std::move(temp);
            m_cmd.Clear();     // ✅ 打开文件后清命令历史（F-04）
            UpdateAllViews(NULL);
            SetModifiedFlag(FALSE);
        }
    }
    catch (CException* e)
    {
        e->ReportError();
        e->Delete();

        // ✅ 关键：不要清空 m_shapes，保持原内容（强异常安全）
        // 如需明确提示可以加 MessageBox，但不强制
        // AfxMessageBox(_T("打开失败，画布保持原内容。"), MB_ICONWARNING);
    }
}

#ifdef _DEBUG
void CMyDrawBoardDoc::AssertValid() const { CDocument::AssertValid(); }
void CMyDrawBoardDoc::Dump(CDumpContext& dc) const { CDocument::Dump(dc); }
#endif
