#include <parser.h>

#include <gtest/gtest.h>

TEST(TestParser, emptyLine)
{
    EXPECT_EQ(assembler::Line{}, assembler::parse({}));
}
