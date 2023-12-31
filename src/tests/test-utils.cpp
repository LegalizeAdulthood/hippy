#include <utils.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(TestUtils, GetErrorLineNumberNoPrefix)
{
    const int line = hippy::GetErrorLineNumber(wxT("no prefix"));

    EXPECT_EQ(-1, line);
}

TEST(TestUtils, GetErrorLineNumberValid)
{
    const int line = hippy::GetErrorLineNumber(wxT("error: line 10"));

    EXPECT_EQ(10, line);
}

TEST(TestUtils, GetErrorLineNumberTrailingText)
{
    const int line = hippy::GetErrorLineNumber(wxT("error: line 10: invalid operand"));

    EXPECT_EQ(10, line);
}

TEST(TestUtils, GetHexFileNameForAsmFile)
{
    wxString hexFile = hippy::GetHexFileName(wxT("foo.asm"));

    EXPECT_EQ(wxT("foo.hex"), hexFile);
}

TEST(TestUtils, GetHexFileNameForFileNoExtension)
{
    wxString hexFile = hippy::GetHexFileName(wxT("foo"));

    EXPECT_EQ(wxT("foo.hex"), hexFile);
}
