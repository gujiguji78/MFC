#include <gtest/gtest.h>
#include "Core_Shapes.h"

// 简单测试：Factory 创建
TEST(ShapeFactoryTest, CreateAllTypes)
{
    EXPECT_NE(ShapeFactory::Create(ShapeType::Line), nullptr);
    EXPECT_NE(ShapeFactory::Create(ShapeType::Rectangle), nullptr);
    EXPECT_NE(ShapeFactory::Create(ShapeType::Ellipse), nullptr);
    EXPECT_NE(ShapeFactory::Create(ShapeType::Freehand), nullptr);
}

// 序列化回环：用 CMemFile + CArchive 模拟文件
TEST(SerializerTest, ShapeRoundTrip)
{
    auto s1 = ShapeFactory::Create(ShapeType::Freehand);
    s1->SetColor(RGB(10, 20, 30));
    s1->SetLineWidth(5);
    s1->SetStartPoint(CPoint(1, 1));
    s1->SetEndPoint(CPoint(2, 2));
    s1->SetEndPoint(CPoint(3, 3));

    CMemFile mem;
    {
        CArchive arStore(&mem, CArchive::store);
        arStore << (int)s1->GetType();
        s1->Save(arStore);
        arStore.Close();
    }

    mem.SeekToBegin();

    std::shared_ptr<IShape> s2;
    {
        CArchive arLoad(&mem, CArchive::load);
        int typeId = 0;
        arLoad >> typeId;
        s2 = ShapeFactory::Create((ShapeType)typeId);
        ASSERT_NE(s2, nullptr);
        s2->Load(arLoad);
        arLoad.Close();
    }

    EXPECT_EQ((int)s2->GetType(), (int)ShapeType::Freehand);
}
