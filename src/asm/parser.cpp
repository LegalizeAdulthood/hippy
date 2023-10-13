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
    if (!std::isspace(line[0]))
    {
        auto space = line.find_first_of(" \t");
        result.label = line.substr(0, space);
    }
    return result;
}

} // namespace assembler
