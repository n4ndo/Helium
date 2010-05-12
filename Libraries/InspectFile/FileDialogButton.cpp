#include "stdafx.h"
#include "FileDialogButton.h"

#include "Inspect/Canvas.h"
#include "FileSystem/FileSystem.h"
#include "FileUI/ManagedFileDialog.h"
#include "Finder/Finder.h"
#include "Common/Container/Insert.h" 

// Using
using namespace File;
using namespace Inspect;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileDialogButton::FileDialogButton()
: m_Title( "Open" )
, m_IsTuidRequired( false )
, m_RequestedFileID( TUID::Null )
{
  Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( "All files (*.*)|*.*" ) );

  SetPath( Finder::ProjectAssets() );
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::Realize( Inspect::Container* parent )
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if ( !IsRealized() )
  {
    __super::Realize( parent );

    if ( IsBound() )
    {
      ReadPathData( m_Path );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::Read()
{
  std::string path = GetPath();
}

///////////////////////////////////////////////////////////////////////////////
// Wrapper around Control::ReadData that cleans the file path returned
//
void FileDialogButton::ReadPathData( std::string& path ) const
{
  ReadData( path );
  if ( !path.empty() )
  {
    FileSystem::CleanName( path );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Shows the file browser dialog.  If the user selects a file, the cleaned
// path is written back into the control.
// 
bool FileDialogButton::Write()
{
  bool result = true;

  // Show the file dialog and write the data back to the control.
  if ( IsBound() )
  {
    std::string filterStr = "All files (*.*)|*.*";
    if ( !m_Filters.empty() )
    {
      filterStr = "";
      S_string::iterator it = m_Filters.begin();
      S_string::iterator itEnd = m_Filters.end();
      for ( ; it != itEnd ; ++it )
      {
        filterStr += (*it);
        filterStr += "|";
      }
      filterStr.erase( filterStr.size() - 1 );
    }

    wxWindow* parent = GetCanvas() ? GetCanvas()->GetControl() : NULL;

    File::ManagedFileDialog fileDialog( parent, m_Title.c_str(), GetPath().c_str(), "", filterStr.c_str(), UIToolKit::FileDialogStyles::DefaultOpen );
    fileDialog.SetTuidRequired( m_IsTuidRequired );
    if ( m_IsTuidRequired )
    {
      fileDialog.SetRequestedFileID( m_RequestedFileID );
    }
    if ( fileDialog.ShowModal() == wxID_OK )
    {
      std::string path = fileDialog.GetPath().c_str();
      result = WriteData( path );
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::SetTitleBar( const std::string& title )
{
  m_Title = title;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::SetFilter( const std::string& filter )
{
  m_Filters.clear();
  Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::SetFilter( const V_string& filter )
{
  m_Filters.clear();

  V_string::const_iterator it = filter.begin();
  V_string::const_iterator itEnd = filter.end();
  for ( ; it != itEnd ; ++it )
  {
    Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( *it ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
void FileDialogButton::AddFilter( const std::string& filter )
{
  Nocturnal::Insert<S_string>::Result inserted = m_Filters.insert( S_string::value_type( filter ) );
}


///////////////////////////////////////////////////////////////////////////////
// Allows you to specify a file ID that should be used if the user selects
// a file that is not in the resolver.
// 
void FileDialogButton::SetRequestedFileID( const tuid& fileID )
{
  m_RequestedFileID = fileID;
}

///////////////////////////////////////////////////////////////////////////////
// 
// 
std::string FileDialogButton::GetPath()
{
  if ( IsBound() )
  {
    ReadPathData( m_Path );
  }

  return m_Path;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the path that the file browser will initally start at.  Can be a full
// path to a file, or just a file.
// 
void FileDialogButton::SetPath( const std::string& path )
{
  if ( !path.empty() )
  {
    m_Path = path;
    FileSystem::CleanName( m_Path );
    if ( IsBound() )
    {
      WriteData( m_Path );
    }
  }
  else
  {
    WriteData( "" );
    m_Path.clear();
  }
}

void FileDialogButton::SetTuidRequired( bool isRequired )
{
  m_IsTuidRequired = isRequired;
}

///////////////////////////////////////////////////////////////////////////////
// Called before the button is realized.  Stores the attributes specific to 
// this button.
// 
bool FileDialogButton::Process(const std::string& key, const std::string& value)
{
  bool wasHandled = false;

  if ( key == BUTTON_FILEDIALOG_ATTR_FILTER )
  {
    SetFilter( value );
    wasHandled = true;
  }
  else if ( key == BUTTON_FILEDIALOG_ATTR_PATH )
  {
    SetPath( value );
    wasHandled = true;
  }
  else if ( key == BUTTON_FILEDIALOG_ATTR_TITLE )
  {
    SetTitleBar( value );
    wasHandled = true;
  }
  else
  {
    wasHandled = __super::Process( key, value );
  }

  return wasHandled;
}