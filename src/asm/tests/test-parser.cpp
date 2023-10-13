#include <gtest/gtest.h>

#include <iostream>
#include <string>

namespace assembler
{

struct Line
{
    std::string label;
    std::string opcode;
    std::string operands;
    std::string comment;
};

inline bool operator==(const Line &lhs, const Line &rhs)
{
    return lhs.label == rhs.label && lhs.opcode == rhs.opcode && lhs.operands == rhs.operands &&
           lhs.comment == rhs.comment;
}

inline std::ostream &operator<<(std::ostream &str, const Line &line)
{
    return str << "{ '" << line.label << "', '" << line.opcode << "', '" << line.operands << "', '" << line.comment
               << "' }";
}

assembler::Line parse(const std::string &line)
{
    return {};
}

} // namespace assembler

TEST(TestParser, emptyLine)
{
    EXPECT_EQ(assembler::Line{}, assembler::parse({}));
}
