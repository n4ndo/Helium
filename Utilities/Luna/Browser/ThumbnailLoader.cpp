#include "Precompile.h"
#include "ThumbnailLoader.h"
#include "Render.h"

#include "Asset/ShaderAsset.h"
#include "Asset/ColorMapAttribute.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "igDXRender/d3dmanager.h"

using namespace Luna;

void* ThumbnailLoader::LoadThread::Entry()
{
#ifdef NOC_ASSERT_ENABLED
  bool emptyQueuePollingCheck = false;
#endif

  while ( true )
  {
    m_Loader.m_Signal.Decrement();

    if ( m_Loader.m_Quit )
    {
      break;
    }

    // the device is gone, our glorious benefactor is probably cleaning up
    IDirect3DDevice9* device = m_Loader.m_D3DManager->GetD3DDevice();
    if ( !device )
    {
      // You should stop this thread before letting go of the window that
      // owns the device.
      NOC_BREAK();
      break;
    }

    // while the device is lost, just wait for it to come back
    while ( device->TestCooperativeLevel() != D3D_OK )
    {
      if ( m_Loader.m_Quit )
      {
        break;
      }

      wxThread::Sleep( 100 );
      continue;
    }

    if ( m_Loader.m_Quit )
    {
      break;
    }

    Asset::AssetFilePtr file;

    {
      Platform::Locker<Asset::OS_AssetFiles>::Handle queue = m_Loader.m_FileQueue.Lock();
      if ( !queue->Empty() )
      {
#ifdef NOC_ASSERT_ENABLED
        emptyQueuePollingCheck = false;
#endif
        file = queue->Front();
      }
      else
      {
        // if you hit this then the bookkeeping of the counting semaphore is broken
        NOC_ASSERT( !emptyQueuePollingCheck );

#ifdef NOC_ASSERT_ENABLED
        emptyQueuePollingCheck = true;
#endif
        continue;
      }

      if ( file.ReferencesObject() )
      {
        // Why does the queue have an invalid file pointer?
        queue->Remove( file );
      }
      else
      {
        NOC_BREAK();
      }
    }

    ResultArgs args;
    args.m_File = file;
    args.m_Cancelled = false;

    if ( Luna::IsSupportedTexture( file->GetFilePath() ) )
    {
      IDirect3DTexture9* texture = NULL;
      if ( texture = LoadTexture( device, file->GetFilePath() ) )
      {
        ThumbnailPtr thumbnail = new Thumbnail( m_Loader.m_D3DManager, texture );
        args.m_Textures.push_back( thumbnail );
      }
    }
    else
    {
      V_string files;
      FileSystem::GetFiles( Finder::GetThumbnailFolder( file->GetFileID() ), files );

      for ( V_string::const_iterator itr = files.begin(), end = files.end();
        itr != end;
        ++itr )
      {
        IDirect3DTexture9* texture = NULL;
        if ( texture = LoadTexture( device, *itr ) )
        {
          ThumbnailPtr thumbnail = new Thumbnail( m_Loader.m_D3DManager, texture );
          args.m_Textures.push_back( thumbnail );
        }
      }

      // Include the color map of a shader as a possible thumbnail image
      if ( FileSystem::HasExtension( file->GetFilePath(), FinderSpecs::Asset::SHADER_DECORATION.GetDecoration() ) )
      {
        Asset::ShaderAssetPtr shader = Reflect::ObjectCast< Asset::ShaderAsset >( Asset::AssetFile::GetAssetClass( file ) );
        if ( shader )
        {
          Asset::ColorMapAttributePtr colorMap = Reflect::ObjectCast< Asset::ColorMapAttribute >( shader->GetAttribute( Reflect::GetType< Asset::ColorMapAttribute >() ) );
          if ( colorMap && colorMap->m_FileID != TUID::Null )
          {
            std::string colorMapPath = File::GlobalManager().GetPath( colorMap->m_FileID );
            if ( !colorMapPath.empty() && FileSystem::Exists( colorMapPath ) && Luna::IsSupportedTexture( colorMapPath ) )
            {
              IDirect3DTexture9* texture = NULL;
              if ( texture = LoadTexture( device, colorMapPath ) )
              {
                ThumbnailPtr thumbnail = new Thumbnail( m_Loader.m_D3DManager, texture );
                args.m_Textures.push_back( thumbnail );
              }
            }
          }
        }
      }
    }

    m_Loader.m_Result.Raise( args );
  }

  return NULL;
}

ThumbnailLoader::ThumbnailLoader( igDXRender::D3DManager* d3dManager )
: m_LoadThread( *this )
, m_Quit( false )
, m_D3DManager( d3dManager )
{
  m_LoadThread.Create();
  m_LoadThread.Run();
}

ThumbnailLoader::~ThumbnailLoader()
{
  m_Quit = true;
  m_Signal.Increment();
  m_LoadThread.Wait();
}

void ThumbnailLoader::Load( const Asset::V_AssetFiles& files )
{
  Platform::Locker<Asset::OS_AssetFiles>::Handle queue = m_FileQueue.Lock();

  for ( Asset::V_AssetFiles::const_reverse_iterator itr = files.rbegin(), end = files.rend();
    itr != end;
    ++itr )
  {
    bool signal = !queue->Remove( *itr );
    queue->Prepend( *itr );
    if ( signal )
    {
      m_Signal.Increment();
    }
  }
}

void ThumbnailLoader::Stop()
{
  Platform::Locker<Asset::OS_AssetFiles>::Handle queue = m_FileQueue.Lock();
  if ( queue->Empty() )
  {
    return;
  }

  while ( !queue->Empty() )
  {
    ResultArgs args;
    args.m_File = queue->Front();
    args.m_Cancelled = true;
    m_Result.Raise( args );
    queue->Remove( args.m_File );
  }

  m_Signal.Reset();
}