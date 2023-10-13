#ifndef ASM_PARSER_H
#define ASM_PARSER_H

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

assembler::Line parse(const std::string &line);

} // namespace assembler

#endif
