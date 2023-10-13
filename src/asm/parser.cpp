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

    if (!std::isspace(line[0]))
    {
        const size_t space = line.find_first_of(" \t");
        result.label = line.substr(0, space);
    }
    return result;
}

} // namespace assembler
