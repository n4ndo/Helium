#include "stdafx.h"

#include "FileDialog.h"

#include "MessageDialog.h"

#include "FileSystem/FileSystem.h"

#include "Common/Types.h"
#include "Common/String/Utilities.h"
#include "Common/String/Tokenize.h"


/////////////////////////////////////////////////////////////////////////////

namespace UIToolKit
{  

  /////////////////////////////////////////////////////////////////////////////

  BEGIN_EVENT_TABLE( FileDialog, wxFileDialog )
    EVT_BUTTON( wxID_OK, FileDialog::OnOkButtonClicked )
    END_EVENT_TABLE()

    /////////////////////////////////////////////////////////////////////////////
    FileDialog::FileDialog
    (
    wxWindow *parent,
    const wxString& message,      /* = wxFileSelectorPromptStr, */
    const wxString& defaultDir,   /* = wxEmptyString, */
    const wxString& defaultFile,  /* = wxEmptyString, */
    const wxString& wildCard,     /* = wxFileSelectorDefaultWildcardStr, */
    FileDialogStyle style,        /* = FileDialogStyles::DefaultOpen, */
    const wxPoint& pos,           /* = wxDefaultPosition, */
    const wxSize& sz,             /* = wxDefaultSize, */
    const wxString& name          /* = wxFileDialogNameStr */
    )
    : wxFileDialog( parent )
    , m_Style( style )
  {
    long styleWXFD = LOWORD( style );

    Create( parent, message, defaultDir, defaultFile, wildCard, styleWXFD, pos, sz, name );
  }

  /////////////////////////////////////////////////////////////////////////////
  FileDialog::~FileDialog()
  {
  }

  ///////////////////////////////////////////////////////////////////////////////

  bool FileDialog::Create
    (
    wxWindow *parent,
    const wxString& message,      /* = wxFileSelectorPromptStr, */
    const wxString& defaultDir,   /* = wxEmptyString, */
    const wxString& defaultFile,  /* = wxEmptyString, */
    const wxString& wildCard,     /* = wxFileSelectorDefaultWildcardStr, */
    long style,                   /* = wxFD_DEFAULT_STYLE, */
    const wxPoint& pos,           /* = wxDefaultPosition, */
    const wxSize& sz,             /* = wxDefaultSize, */
    const wxString& name          /* = wxFileDialogNameStr */
    )
  {
    bool result = wxFileDialog::Create( parent, message, defaultDir, defaultFile, wildCard, style, pos, sz, name );

    SetFilter( wildCard.c_str() );

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Overridden to provide path cleaning.
  // 
  int FileDialog::ShowModal()
  {
    // Clear out the file list, we're about to build it.
    m_Files.clear();
    int result = __super::ShowModal();

    if ( result == wxID_OK )
    {
      // Get the file paths
      wxArrayString paths;
      if ( IsMultipleSelectionEnabled() )
      {
        GetPaths( paths );
      }
      else
      {
        wxString path = GetPath();
        if ( !path.IsEmpty() )
        {
          paths.Add( path );
        }
      }

      // Cache as std::string
      const size_t num = paths.Count();
      for ( size_t index = 0; index < num; ++index )
      {
        m_Files.insert( paths[index].c_str() );
      }
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns true if this dialog was created with the FileDialogStyles::Multiple
  // flag.
  // 
  bool FileDialog::IsMultipleSelectionEnabled() const
  {
    return ( m_Style & FileDialogStyles::Multiple ) == FileDialogStyles::Multiple;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Overridden to clean the path before returning it.  Call GetFilePath to work
  // with std::string instead.
  // 
  wxString FileDialog::GetPath() const
  {
    std::string path( __super::GetPath().c_str() );
    FileSystem::CleanName( path );
    return path.c_str();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Overridden to clean the paths before returning them.  Call GetFilePaths to 
  // work with std::string instead.
  // 
  void FileDialog::GetPaths( wxArrayString& paths ) const
  {
    __super::GetPaths( paths );

    size_t count = paths.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
      std::string file;
      FileSystem::CleanName( paths[n].c_str(), file );
      paths[n] = file.c_str();
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the cleaned file path that was selected by the user.  Only call
  // this function when the dialog is in single-select mode, otherwise use
  // GetFilePaths.
  // 
  // NOTE: Return value is only valid if you have called ShowModal and the 
  // result was wxID_OK.
  // 
  const std::string& FileDialog::GetFilePath() const
  {
    // Only call this function when working with a dialog in single-select mode
    NOC_ASSERT( !IsMultipleSelectionEnabled() );

    static const std::string empty;
    if ( !m_Files.empty() )
    {
      return *m_Files.begin();
    }
    return empty;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns all the files selected by the user.  Only call this function when
  // working with a file dialog in single-select mode, otherwise use GetFilePath.
  // All file paths have been cleaned.
  // 
  // NOTE: Return value is only valid if you have called ShowModal and the 
  // result was wxID_OK.
  // 
  const S_string& FileDialog::GetFilePaths() const
  {
    // Only call this function when working with a dialog in multi-select mode
    NOC_ASSERT( IsMultipleSelectionEnabled() );

    return m_Files;
  }


  /////////////////////////////////////////////////////////////////////////////
  void FileDialog::SetFilter( const std::string& filter )
  {
    m_Filters.Clear();
    AddFilter( filter );
  }


  /////////////////////////////////////////////////////////////////////////////
  void FileDialog::SetFilterIndex( const std::string& filter )
  {
    i32 index = 0;
    OS_string::Iterator itr = m_Filters.Begin();
    OS_string::Iterator end = m_Filters.End();
    for ( i32 count = 0; itr != end; ++itr, ++count )
    {
      const std::string& current = *itr;
      if ( current == filter )
      {
        index = count;
        break;
      }
    }

    __super::SetFilterIndex( index );
  }


  /////////////////////////////////////////////////////////////////////////////
  // For example, this input:
  //  "BMP and GIF files (*.bmp;*.gif)|*.bmp;*.gif|PNG files (*.png)|*.png"
  //
  // Becomes this map
  //  "BMP and GIF files (*.bmp;*.gif)" -> "*.bmp;*.gif"
  //  "PNG files (*.png)" -> "*.png"
  //
  void FileDialog::AddFilter( const std::string& filter )
  {
    V_string splitFilter;
    Tokenize( filter, splitFilter, "\\|" );

    if ( (int)splitFilter.size() % 2 != 0 )
      return; // error

    int numFilters = (int)splitFilter.size() / 2;
    for ( int i = 0; i < (int)splitFilter.size() ; i+=2 )
    {
      std::string display = splitFilter.at( i );
      std::string mask    = splitFilter.at( i+1 );

      display += "|" + mask;

      bool inserted = m_Filters.Append( display ); 
    }

    UpdateFilter();
  }


  /////////////////////////////////////////////////////////////////////////////
  void FileDialog::UpdateFilter()
  {
    if ( m_Style & FileDialogStyles::ShowAllFilesFilter )
    {
      bool inserted = m_Filters.Append( "All files (*.*)|*.*" );
    }

    std::string filterStr = "";
    if ( !m_Filters.Empty() )
    {
      OS_string::Iterator it = m_Filters.Begin();
      OS_string::Iterator itEnd = m_Filters.End();
      for ( ; it != itEnd ; ++it )
      {
        if ( !filterStr.empty() )
        {
          filterStr += "|";
        }
        filterStr += (*it);
      }
    }

    // update the wxFileDialog wild card
    SetWildcard( filterStr.c_str() );
    __super::SetFilterIndex( m_filterIndex );
  }


  /////////////////////////////////////////////////////////////////////////////
  void FileDialog::OnOkButtonClicked( wxCommandEvent& evt )
  {
    // FIXME: check for errors in the user data and submit the form
    // - the AssetName may contain a full path
    // - try to find the location
    // - ensure that the file exists (if required)
    // - ensure that the proper file extension is present
    // - if the location exists but there is no file, go to that location in the view and reset the full path
  }

} // namespace UIToolKit


