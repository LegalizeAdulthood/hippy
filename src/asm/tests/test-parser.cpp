#include <parser.h>

#include <gtest/gtest.h>

TEST(TestParser, emptyLine)
{
    EXPECT_EQ(assembler::Line{}, assembler::parse({}));
}

TEST(TestParser, labelOnly)
{
    assembler::Line expected{};
    expected.label = "foo";

    EXPECT_EQ(expected, assembler::parse("foo"));
}

TEST(TestParser, labelSpace)
{
    assembler::Line expected{};
    expected.label = "foo";

    EXPECT_EQ(expected, assembler::parse("foo "));
}

TEST(TestParser, labelTab)
{
    assembler::Line expected{};
    expected.label = "foo";

    EXPECT_EQ(expected, assembler::parse("foo\t"));
}
