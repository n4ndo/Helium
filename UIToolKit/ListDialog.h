#pragma once

#include "API.h"
#include "SortableListView.h"
#include <wx/wx.h>

namespace UIToolKit
{
  //////////////////////////////////////////////////////////////////////////////
  // Dialog that displays a list of messages.
  // 
  class UITOOLKIT_API ListDialog : public wxDialog 
  {
  protected:
    wxStaticText* m_StaticText;
    SortableListView* m_MsgList;
    wxButton* m_OK;
    std::string m_Description;

  public:
    ListDialog( wxWindow* parent, const std::string& title, const std::string& desc, const V_string& msgs );
  };
}
