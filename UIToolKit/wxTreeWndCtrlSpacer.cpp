///////////////////////////////////////////////////////////////////////////////
// Name:        wxTreeWndCtrlSpacer.cpp
// Purpose:     Spacer window implementation
// Author:      Insomniac Games
// Created:     03/23/03
// Modified by:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "stdafx.h"
#include "wxTreeWndCtrlSpacer.h"
#include "wxTreeWndCtrl.h"

using namespace UIToolKit;

// ----------------------------------------------------------------------------
// wxTreeWndCtrlSpacer
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTreeWndCtrlSpacer, wxPanel)

BEGIN_EVENT_TABLE( wxTreeWndCtrlSpacer, wxPanel )
EVT_LEFT_DOWN( wxTreeWndCtrlSpacer::OnMouseDown )
EVT_PAINT( wxTreeWndCtrlSpacer::OnPaint )
END_EVENT_TABLE()

wxTreeWndCtrlSpacer::wxTreeWndCtrlSpacer()
                   : wxPanel(),
                     m_numColumns(0),
                     m_node(wxTreeWndCtrlItemIdInvalid),
                     m_treeWndCtrl(NULL)
{
}

wxTreeWndCtrlSpacer::wxTreeWndCtrlSpacer(wxTreeWndCtrl *parent,
                                         const wxTreeItemId& node,
                                         unsigned int numColumns)
                   : wxPanel(parent),
                     m_numColumns(numColumns),
                     m_node(node),
                     m_treeWndCtrl(parent)
{
    Layout();
}

bool wxTreeWndCtrlSpacer::Layout()
{
    SetSize(m_numColumns * m_treeWndCtrl->GetColumnSize(), wxDefaultSize.GetHeight());
    return __super::Layout();
}
    
void wxTreeWndCtrlSpacer::SetNumColumns(unsigned int numColumns)
{
    m_numColumns = numColumns;
    Layout();
}
    
int wxTreeWndCtrlSpacer::GetVerticalMidpoint(wxBitmap bitmap)
{
    switch ( m_treeWndCtrl->GetLineMode() )
    {
        case wxTWC_LINE_TOP:
            return bitmap.GetHeight()/2;
        
        case wxTWC_LINE_BOTTOM:
            return GetSize().GetHeight() - 1 - bitmap.GetHeight()/2;

        case wxTWC_LINE_CENTER:
        default:
            return GetSize().GetHeight()/2;
    }
}

wxPoint wxTreeWndCtrlSpacer::GetBitmapPosition(wxBitmap bitmap, unsigned int columnIndex)
{
    unsigned int columnSize = m_treeWndCtrl->GetColumnSize();
    unsigned int columnStartX = columnIndex * columnSize;
    unsigned int columnCenterX = columnStartX + (columnSize / 2);
    unsigned int columnCenterY = GetVerticalMidpoint(bitmap);

    return wxPoint(columnCenterX - bitmap.GetWidth() / 2, columnCenterY - bitmap.GetHeight() / 2);
}

void wxTreeWndCtrlSpacer::DrawHorizontalLine(wxPaintDC& dc,
                                             unsigned int x1,
                                             unsigned int x2,
                                             unsigned int y)
{
    switch ( m_treeWndCtrl->GetDashMode() )
    {
        case wxTWC_DASH_CUSTOM:
            dc.DrawLine( x1, y, x2, y );
            break;
        
        case wxTWC_DASH_DEFAULT:
        default:
            {
                int originX = 0;
                int originY = 0;
                ClientToScreen( &originX, &originY );
                int origin = originX + originY + y;

                for ( unsigned int x = x1; x <= x2; ++x )
                {
                    if ( ( origin + x ) % 2 )
                        dc.DrawPoint( x, y );
                }
            }
            break;
    }
}
    
void wxTreeWndCtrlSpacer::DrawVerticalLine(wxPaintDC& dc,
                                           unsigned int y1,
                                           unsigned int y2,
                                           unsigned int x)
{
    switch ( m_treeWndCtrl->GetDashMode() )
    {
        case wxTWC_DASH_CUSTOM:
            dc.DrawLine( x, y1, x, y2 );
            break;
        
        case wxTWC_DASH_DEFAULT:
        default:
            {
                int originX = 0;
                int originY = 0;
                ClientToScreen( &originX, &originY );
                int origin = originX + originY + x;

                for ( unsigned int y = y1; y <= y2; ++y )
                {
                    if ( ( origin + y ) % 2 )
                        dc.DrawPoint( x, y );
                }
            }
            break;
    }
}
    
void wxTreeWndCtrlSpacer::DrawLines(wxPaintDC& dc,
                                    const wxTreeItemId& node,
                                    int columnIndex,
                                    bool leaf)
{
    wxTreeItemId parent = m_treeWndCtrl->GetItemParent(node);
    if ( columnIndex > 0 )
    {
        if ( parent != wxTreeWndCtrlItemIdInvalid )
            DrawLines(dc, parent, columnIndex - 1, false);
            
        if ( m_treeWndCtrl->GetLineDrawMode() == wxTWC_LINEDRAW_ROOT_ONLY )
            return;
    }
    
    unsigned int columnSize = m_treeWndCtrl->GetColumnSize();

    int columnStartX = columnIndex * columnSize;
    int columnCenterX = columnStartX + columnSize/2;
    int columnCenterY = GetVerticalMidpoint(m_treeWndCtrl->GetToggleBitmap(m_node));
    int columnStartY = 0;

    bool hasSibling = false;
    if ( parent == wxTreeWndCtrlItemIdInvalid )
    {
      columnStartY = columnCenterY;
    }
    else
    {
      hasSibling = m_treeWndCtrl->GetNextSibling(node) != wxTreeWndCtrlItemIdInvalid;
      if ( ( columnIndex == 0 ) && ( m_node == node ) && ( m_treeWndCtrl->GetPrevSibling(node) == wxTreeWndCtrlItemIdInvalid ) )
      {
        columnStartY = columnCenterY;
      }
    }
    
    wxPen oldPen = dc.GetPen();
    dc.SetPen(m_treeWndCtrl->GetPen());
    
    if ( leaf )
    {
        int columnEndX = columnStartX + columnSize - 1;
        int columnEndY = hasSibling ? GetSize().GetHeight() : columnCenterY;

        DrawHorizontalLine( dc, columnCenterX, columnEndX, columnCenterY );
        if ( columnStartY != columnEndY )
            DrawVerticalLine( dc, columnStartY, columnEndY, columnCenterX );
    }
    else if ( hasSibling )
    {
        int columnEndY = GetSize().GetHeight();
        DrawVerticalLine( dc, columnStartY, columnEndY, columnCenterX );
    }
    
    dc.SetPen(oldPen);
}
    
void wxTreeWndCtrlSpacer::DrawToggle(wxPaintDC& dc, unsigned int columnIndex)
{
    wxBitmap bitmap = m_treeWndCtrl->GetToggleBitmap(m_node);
    wxPoint bitmapPosition = GetBitmapPosition(bitmap, columnIndex);
    dc.DrawBitmap(bitmap, bitmapPosition.x, bitmapPosition.y, true);
}

void wxTreeWndCtrlSpacer::OnMouseDown(wxMouseEvent& e)
{
  e.Skip();

  if ( !m_numColumns )
      return;
      
  if ( !m_treeWndCtrl->ToggleVisible(m_node) )
      return;
  
  wxBitmap bitmap = m_treeWndCtrl->GetToggleBitmap(m_node);
  wxPoint bitmapStart = GetBitmapPosition(bitmap, m_numColumns - 1);

  int clickTolerance = m_treeWndCtrl->GetClickTolerance();

  if ( e.GetPosition().x < bitmapStart.x - clickTolerance )
      return;

  if ( e.GetPosition().x > bitmapStart.x + bitmap.GetWidth() + clickTolerance )
      return;

  if ( e.GetPosition().y < bitmapStart.y - clickTolerance )
      return;

  if ( e.GetPosition().y > bitmapStart.y + bitmap.GetHeight() + clickTolerance )
      return;

  m_treeWndCtrl->Toggle(m_node);
}

void wxTreeWndCtrlSpacer::OnPaint(wxPaintEvent& e)
{
  wxPaintDC dc(this);

  if ( m_numColumns == 0 )
      return;

  if ( m_treeWndCtrl->GetLineDrawMode() != wxTWC_LINEDRAW_NONE )
      DrawLines(dc, m_node, m_numColumns - 1, true);

  if ( m_treeWndCtrl->ToggleVisible(m_node) )
      DrawToggle( dc, m_numColumns - 1 );
  
  e.Skip();
}
