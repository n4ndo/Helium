#include "stdafx.h"
#include "CustomColors.h"

#include "Common/String/Tokenize.h"
#include "Common/Types.h"

#include <sstream>

// Using
using namespace UIToolKit;

// Statics
// Delimiter between each color
static const std::string s_ColorDelimiter = "#";

///////////////////////////////////////////////////////////////////////////////
// Gets the registry key for where the custom colors should be stored by default.
// 
std::string CustomColors::GetDefaultRegistryKey()
{
  return "CustomColors";
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the custom colors in colorData and builds a delimited
// string representation of the information.  This information can be persisted
// by the application and reloaded later by calling Load.
// 
std::string CustomColors::Save( wxColourData& colorData )
{
  std::ostringstream result;
  
  for ( i32 colorIndex = 0; colorIndex < NumColors; ++colorIndex )
  {
    if ( colorIndex > 0 )
    {
      result << s_ColorDelimiter;
    }

    const wxColour& color = colorData.GetCustomColour( colorIndex );
    if ( color.IsOk() )
    {
      // GetAsString returns HTML format (example: #FF00FF).  Strip the leading #.
      result << color.GetAsString( wxC2S_HTML_SYNTAX ).substr( 1 ).c_str();
    }
  }

  return result.str();
}

///////////////////////////////////////////////////////////////////////////////
// Populates the custom colors of colorData with the values stored in the info
// string.  The info string is expected to have been generated by calling the
// above Save function.
// 
void CustomColors::Load( wxColourData& colorData, const std::string& info )
{
  if ( !info.empty() )
  {
    V_string colors;
    Tokenize( info, colors, s_ColorDelimiter );
    
    V_string::const_iterator colorItr = colors.begin();
    V_string::const_iterator colorEnd = colors.end();
    for ( i32 colorIndex = 0; colorItr != colorEnd && colorIndex < NumColors; ++colorItr, ++colorIndex )
    {
      const std::string& colorStr = *colorItr;
      if ( colorStr.length() >= 6 )
      {
        u32 red = 0;
        u32 green = 0;
        u32 blue = 0;
        sscanf( colorStr.c_str(), "%02X%02X%02X", &red, &green, &blue );

        colorData.SetCustomColour( colorIndex, wxColour( (u8)red, (u8)green, (u8)blue ) );
      }
    }
  }
}
