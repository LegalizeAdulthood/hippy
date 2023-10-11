#include "utils.h"

namespace hippy
{

int GetErrorLineNumber(const wxString &text)
{
    const wxString prefix(wxT("error: line "));
    return text.StartsWith(prefix) ? wxAtoi(text.SubString(prefix.Length(), text.Length())) : -1;
}

} // namespace hippy
