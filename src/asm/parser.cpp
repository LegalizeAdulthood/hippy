#include "parser.h"

#include <cctype>

namespace assembler
{

Line parse(const std::string &line)
{
    if (line.empty())
    {
        return {};
    }

    const char *const whitespace{" \t"};
    Line              result;
    if (line[0] == '*')
    {
        const size_t start = line.find_first_not_of("* \t");
        result.comment = line.substr(start);
        return result;
    }

    size_t pos{};
    auto   nextWhiteSpace = [&] { return line.find_first_of(whitespace, pos); };
    if (!std::isspace(line[0]))
    {
        pos = nextWhiteSpace();
        result.label = line.substr(0, pos);
    }
    if (pos == std::string::npos)
    {
        // label, no trailing whitespace
        return result;
    }

    auto nextNotWhiteSpace = [&] { return line.find_first_not_of(whitespace, pos); };
    pos = nextNotWhiteSpace();
    if (pos == std::string::npos)
    {
        // label, trailing whitespace, no opcode
        return result;
    }

    size_t space = nextWhiteSpace();
    if (space != std::string::npos)
    {
        result.opcode = line.substr(pos, space - pos);
    }
    else
    {
        result.opcode = line.substr(pos);
    }
    pos = space;

    if (pos == std::string::npos)
    {
        return result;
    }

    pos = nextNotWhiteSpace();
    if (pos == std::string::npos)
    {
        return result;
    }

    space = nextWhiteSpace();
    if (space != std::string::npos)
    {
        result.operands = line.substr(pos, space - pos);
    }
    else
    {
        result.operands = line.substr(pos);
    }

    return result;
}

} // namespace assembler
