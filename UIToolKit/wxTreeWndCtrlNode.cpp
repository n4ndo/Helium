///////////////////////////////////////////////////////////////////////////////
// Name:        wxTreeWndCtrlNode.cpp
// Purpose:     Tree control with windows node implementation
// Author:      Insomniac Games
// Created:     03/23/03
// Modified by:
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "stdafx.h"
#include "wxTreeWndCtrlNode.h"
#include "wxTreeWndCtrl.h"
#include "wxTreeWndCtrlSpacer.h"
#include "wx/arrimpl.cpp"

using namespace UIToolKit;

// ----------------------------------------------------------------------------
// wxTreeWndCtrlNode
// ----------------------------------------------------------------------------

wxTreeWndCtrlNode::wxTreeWndCtrlNode(wxTreeWndCtrl* treeWndCtrl,
                                     const wxTreeItemId& parent,
                                     wxWindow *window,
                                     unsigned int numColumns,
                                     wxTreeItemData *data,
                                     bool expanded)
                 : m_expanded(expanded),
                   m_window(window),
                   m_data(data),
                   m_id(this),
                   m_parent(parent)
{
  m_spacer = new wxTreeWndCtrlSpacer(treeWndCtrl, m_id, numColumns);
}

wxTreeWndCtrlNode::~wxTreeWndCtrlNode()
{
}
