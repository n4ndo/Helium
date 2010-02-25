#include "stdafx.h"
#include "Label.h"
#include "Container.h"
#include "StringData.h"

using namespace Inspect;

class Text : public wxPanel
{
private:
  Label* m_Label;
  wxStaticText* m_StaticText;

public:
  Text(wxWindow* parent, Label* label)
    : wxPanel (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, "LText")
    , m_Label (label)
  {
    SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
    wxSizer* sizer = GetSizer();
    
    m_StaticText = new wxStaticText( this, wxID_ANY, "Temp" );
    sizer->Add( m_StaticText, 0, wxALIGN_CENTER_VERTICAL, 0);
  }

  void OnSize(wxSizeEvent& event)
  {
    m_Label->Read();
    Layout();
  }

  void SetLabel(const wxString& label)
  {
    m_StaticText->SetLabel(label);
  }

  void UpdateToolTip( const wxString& toolTip )
  {
    m_StaticText->SetToolTip( toolTip );
  }

  virtual bool SetForegroundColour(wxColour& color)
  {
    return __super::SetForegroundColour(color) && m_StaticText->SetForegroundColour(color);
  }

  virtual bool SetBackgroundColour(wxColour& color)
  {
    return __super::SetBackgroundColour(color) && m_StaticText->SetBackgroundColour(color);
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(Text, wxPanel)
EVT_SIZE(Text::OnSize)
END_EVENT_TABLE()

Label::Label()
: m_AutoToolTip( true )
{
  SetProportionalWidth( 1.f/3.f );
}

bool Label::Process(const std::string& key, const std::string& value)
{
  bool handled = false;
  if (__super::Process(key, value))
    return true;

  if (key == LABEL_ATTR_TEXT)
  {
    SetText( value );
    return true;
  }

  return false;
}

void Label::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );
  
  if (m_Window != NULL)
    return;

  m_Window = new Text (parent->GetWindow(), this);

  __super::Realize( parent );
}

void Label::Read()
{
  if ( IsBound() )
  {
    std::string str;
    ReadData( str );
    UpdateUI( str );

    __super::Read();
  }
}

void Label::SetText(const std::string& text)
{
  if ( !IsBound() )
  {
    Bind( new StringFormatter<std::string>( new std::string( text ), true ) );
  }
  else
  {
    WriteData( text );
  }

  UpdateUI( text );
}

std::string Label::GetText() const
{
  std::string text;
  if ( IsBound() )
  {
    ReadData( text );
  }
  return text;
}

void Label::SetAutoToolTip( bool enable )
{
  // This may not be necessary, but it provides a way to turn off
  // the automatic tooltip generation for truncated labels.
  m_AutoToolTip = enable;
}

void Label::SetToolTip( const std::string& toolTip )
{
  // This updates the panel with the proper tooltip.
  // Removed because it produces a tooltip flicker as you mouse 
  // over the area between the static text and the panel in the
  // background.
  //__super::SetToolTip( toolTip );

  // This updates the inner static text control with the tooltip.
  Text* control = Control::Cast< Text >( this );
  control->UpdateToolTip( toolTip.c_str() );
}

bool Label::TrimString(std::string& str, int width)
{
  std::string tooltip = str;
  bool trimmed = __super::TrimString( str, width );
  if ( m_AutoToolTip )
  {
    if ( !trimmed )
    {
      tooltip.clear();
    }

    // If the label is trimmed, set the tooltip.
    SetToolTip( tooltip );
  }
  return trimmed;
}

void Label::UpdateUI( const std::string& text )
{
  if ( IsRealized() )
  {
    std::string trimmed = text;
    TrimString( trimmed, m_Window->GetSize().GetWidth() );

    wxStaticText* staticText = Control::Cast< wxStaticText >( this );
    if ( trimmed != staticText->GetLabel().c_str() )
    {
      m_Window->Freeze();
      staticText->SetLabel( trimmed.c_str() );
      m_Window->Thaw();
    }
  }
}