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

    Line result;
    if(line[0] == '*')
    {
        const size_t comment = line.find_first_not_of("* \t");
        result.comment = line.substr(comment);
        return result;
    }

    size_t pos{};
    if (!std::isspace(line[0]))
    {
        pos = line.find_first_of(" \t");
        result.label = line.substr(0, pos);
    }

    if (pos != std::string::npos)
    {
        const size_t notSpace = line.find_first_not_of(" \t", pos);
        if (notSpace != std::string::npos)
        {
            result.opcode = line.substr(notSpace);
        }
    }

    return result;
}

} // namespace assembler
