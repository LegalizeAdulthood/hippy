#include "parser.h"

#include <cctype>

namespace assembler
{

static const char *const s_whitespace{" \t"};

size_t extractField(size_t pos, const std::string &text, std::string &field)
{
    const size_t space = text.find_first_of(s_whitespace, pos);
    if (space != std::string::npos)
    {
        field = text.substr(pos, space - pos);
    }
    else
    {
        field = text.substr(pos);
    }
    return space;
}

Fields parse(const std::string &text)
{
    if (text.empty())
    {
        return {};
    }

    Fields fields;
    if (text[0] == '*')
    {
        const size_t start = text.find_first_not_of("* \t");
        fields.comment = text.substr(start);
        return fields;
    }

    size_t pos{};
    auto   nextWhiteSpace = [&] { return text.find_first_of(s_whitespace, pos); };
    if (!std::isspace(text[0]))
    {
        pos = nextWhiteSpace();
        fields.label = text.substr(0, pos);
    }
    if (pos == std::string::npos)
    {
        // label, no trailing whitespace
        return fields;
    }

    auto nextNotWhiteSpace = [&] { return text.find_first_not_of(s_whitespace, pos); };
    pos = nextNotWhiteSpace();
    if (pos == std::string::npos)
    {
        // label, trailing whitespace, no opcode
        return fields;
    }

    pos = extractField(pos, text, fields.opcode);
    if (pos == std::string::npos)
    {
        return fields;
    }

    pos = nextNotWhiteSpace();
    if (pos == std::string::npos)
    {
        return fields;
    }
    pos = extractField(pos, text, fields.operands);

    return fields;
}

} // namespace assembler
