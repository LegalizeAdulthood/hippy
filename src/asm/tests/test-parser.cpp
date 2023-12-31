#include <parser.h>

#include <gtest/gtest.h>

class TestParser : public testing::Test
{
protected:
    assembler::Fields m_expected{};
};

TEST_F(TestParser, emptyLine)
{
    EXPECT_EQ(m_expected, assembler::parse({}));
}

TEST_F(TestParser, labelOnly)
{
    m_expected.label = "FOO";

    EXPECT_EQ(m_expected, assembler::parse("foo"));
}

TEST_F(TestParser, labelSpace)
{
    m_expected.label = "FOO";

    EXPECT_EQ(m_expected, assembler::parse("foo "));
}

TEST_F(TestParser, labelTab)
{
    m_expected.label = "FOO";

    EXPECT_EQ(m_expected, assembler::parse("foo\t"));
}

TEST_F(TestParser, lineCommentStar)
{
    m_expected.comment = "This is a comment";

    EXPECT_EQ(m_expected, assembler::parse("* This is a comment"));
}

TEST_F(TestParser, lineCommentSemiColon)
{
    m_expected.comment = "This is a comment";

    EXPECT_EQ(m_expected, assembler::parse("; This is a comment"));
}

TEST_F(TestParser, spaceOpcode)
{
    m_expected.opcode = "END";

    EXPECT_EQ(m_expected, assembler::parse(" end"));
}

TEST_F(TestParser, tabOpcode)
{
    m_expected.opcode = "END";

    EXPECT_EQ(m_expected, assembler::parse("\tend"));
}

TEST_F(TestParser, opcodeOperand)
{
    m_expected.opcode = "STAA";
    m_expected.operands = "0,X";

    EXPECT_EQ(m_expected, assembler::parse(" staa 0,x"));
}

TEST_F(TestParser, opcodeOperandComment)
{
    m_expected.opcode = "ORG";
    m_expected.operands = "$8000";
    m_expected.comment = "set origin";

    EXPECT_EQ(m_expected, assembler::parse(" org $8000     set origin"));
}

TEST_F(TestParser, opcodeOperandCommentStar)
{
    m_expected.opcode = "ORG";
    m_expected.operands = "$8000";
    m_expected.comment = "set origin";

    EXPECT_EQ(m_expected, assembler::parse(" org $8000     * set origin"));
}

TEST_F(TestParser, opcodeOperandCommentSemiColon)
{
    m_expected.opcode = "ORG";
    m_expected.operands = "$8000";
    m_expected.comment = "set origin";

    EXPECT_EQ(m_expected, assembler::parse(" org $8000     ; set origin"));
}

TEST_F(TestParser, indentedCommentStar)
{
    m_expected.comment = "indented comment";

    EXPECT_EQ(m_expected, assembler::parse("  * indented comment"));
}

TEST_F(TestParser, indentedCommentSemiColon)
{
    m_expected.comment = "indented comment";

    EXPECT_EQ(m_expected, assembler::parse("  ; indented comment"));
}
