#include "Windows/Windows.h"
#include "Semaphore.h"
#include "Platform.h"

#include "Common/Assert.h"
#include "Windows/Error.h"
#include "Console/Console.h"

using namespace Platform;

Semaphore::Semaphore()
{
  m_Handle = ::CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
  if ( m_Handle == NULL )
  {
    Platform::Print("Failed to create semaphore (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

Semaphore::~Semaphore()
{
  BOOL result = ::CloseHandle(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to close semaphore (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Semaphore::Increment()
{
  LONG count = 0;
  BOOL result = ::ReleaseSemaphore(m_Handle, 1, &count);
  if ( result != TRUE )
  {
    Platform::Print("Failed to inrement semaphore from %d (%s)\n", count, Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Semaphore::Decrement()
{
  DWORD result = ::WaitForSingleObject(m_Handle, INFINITE);
  if ( result != WAIT_OBJECT_0 )
  {
    Platform::Print("Failed to decrement semaphore (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Semaphore::Reset()
{
  BOOL result = ::CloseHandle(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to close semaphore (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }

  m_Handle = ::CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
  if ( m_Handle == NULL )
  {
    Platform::Print("Failed to create semaphore (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}
