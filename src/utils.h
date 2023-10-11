#pragma once

#include <wx/wx.h>

namespace hippy
{

int GetErrorLineNumber(const wxString &text);
wxString GetHexFileName(const wxString &fileName);

} // namespace hippy
