#include <disassembler.h>

#include <gtest/gtest.h>

#include <string>

TEST(TestDisassembler, invalidOpCodeFormsConstantByte)
{
    Disassembler dasm;
    BYTE bytes[1]{};
    char buffer[100]{};

    int result = dasm.Dasm(bytes, sizeof(bytes), buffer);

    EXPECT_EQ(1, result);
    EXPECT_EQ(std::string{"FCB     $00"}, buffer);
}
