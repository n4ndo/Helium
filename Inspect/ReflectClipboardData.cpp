#include "stdafx.h"
#include "ReflectClipboardData.h"

using namespace Inspect;

// Definition
REFLECT_DEFINE_ABSTRACT( ReflectClipboardData );

void ReflectClipboardData::EnumerateClass( Reflect::Compositor<ReflectClipboardData>& comp )
{
}


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ReflectClipboardData::InitializeType()
{
  Reflect::RegisterClass< ReflectClipboardData >( "ReflectClipboardData" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ReflectClipboardData::CleanupType()
{
  Reflect::UnregisterClass< ReflectClipboardData >();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ReflectClipboardData::ReflectClipboardData()
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ReflectClipboardData::~ReflectClipboardData()
{
}