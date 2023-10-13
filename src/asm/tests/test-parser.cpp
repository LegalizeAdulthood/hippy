#include <parser.h>

#include <gtest/gtest.h>

class TestParser : public testing::Test
{
protected:
    assembler::Line m_expected{};
};

TEST_F(TestParser, emptyLine)
{
    EXPECT_EQ(m_expected, assembler::parse({}));
}

TEST_F(TestParser, labelOnly)
{
    m_expected.label = "foo";

    EXPECT_EQ(m_expected, assembler::parse("foo"));
}

TEST_F(TestParser, labelSpace)
{
    m_expected.label = "foo";

    EXPECT_EQ(m_expected, assembler::parse("foo "));
}

TEST_F(TestParser, labelTab)
{
    m_expected.label = "foo";

    EXPECT_EQ(m_expected, assembler::parse("foo\t"));
}

TEST_F(TestParser, lineComment)
{
    m_expected.comment = "This is a comment";

    EXPECT_EQ(m_expected, assembler::parse("* This is a comment"));
}

TEST_F(TestParser, spaceOpcode)
{
    m_expected.opcode = "END";

    EXPECT_EQ(m_expected, assembler::parse(" END"));
}

TEST_F(TestParser, tabOpcode)
{
    m_expected.opcode = "END";

    EXPECT_EQ(m_expected, assembler::parse("\tEND"));
}

TEST_F(TestParser, opcodeOperand)
{
    m_expected.opcode = "ORG";
    m_expected.operands = "$8000";

    EXPECT_EQ(m_expected, assembler::parse(" ORG $8000"));
}
