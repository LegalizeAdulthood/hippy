#ifndef ASM_PARSER_H
#define ASM_PARSER_H

#include <iostream>
#include <string>

namespace assembler
{

struct Fields
{
    std::string label;
    std::string opcode;
    std::string operands;
    std::string comment;
};

inline bool operator==(const Fields &lhs, const Fields &rhs)
{
    return lhs.label == rhs.label && lhs.opcode == rhs.opcode && lhs.operands == rhs.operands &&
           lhs.comment == rhs.comment;
}

inline std::ostream &operator<<(std::ostream &str, const Fields &line)
{
    return str << "{ '" << line.label << "', '" << line.opcode << "', '" << line.operands << "', '" << line.comment
               << "' }";
}

Fields parse(const std::string &text);

} // namespace assembler

#endif
