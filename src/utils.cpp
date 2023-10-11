#include "utils.h"

namespace hippy
{

int GetErrorLineNumber(const wxString &text)
{
    const wxString prefix(wxT("error: line "));
    return text.StartsWith(prefix) ? wxAtoi(text.SubString(prefix.Length(), text.Length())) : -1;
}

wxString GetHexFileName(const wxString &fileName)
{
    wxString text(fileName);
    size_t   pos = text.find_last_of(wxT('.'));
    if(pos != wxString::npos)
    {
        text = text.SubString(0, pos) + wxT("hex");
    }
    else
    {
        text += wxT(".hex");
    }
    return text;
}

} // namespace hippy
