#include "Windows/Windows.h"
#include "Client.h"
#include "Process.h"

#include "Common/Exception.h"
#include "Common/CommandLine.h"

#include "IPC/Pipe.h"

#include "Debug/Exception.h"
#include "AppUtils/AppUtils.h"
#include "Console/Console.h"

#include <sstream>

using namespace Worker;

// the ipc connection for worker applications
IPC::Connection* g_Connection = NULL;

// Background processes hook functions for message emission
static void PrintedListener(Console::PrintedArgs& args)
{
  if (g_Connection && g_Connection->GetState() == IPC::ConnectionStates::Active)
  {
    u32 size = sizeof( Worker::ConsoleOutput ) + (u32)args.m_Statement.m_String.length() + 1;

    IPC::Message* msg = g_Connection->CreateMessage( Worker::ConsoleOutputMessage, size );

    ConsoleOutput* output = (ConsoleOutput*)msg->GetData();
    output->m_Stream = args.m_Statement.m_Stream;
    output->m_Level = args.m_Statement.m_Level;
    output->m_Indent = args.m_Statement.m_Indent;
    memcpy(output->m_String, args.m_Statement.m_String.c_str(), args.m_Statement.m_String.length() + 1);

    if (g_Connection->Send(msg) != IPC::ConnectionStates::Active)
    {
      delete msg;
    }
  }
}

// Called from AppUtils on Shutdown()
static void ShutdownListener(const AppUtils::ShutdownArgs& args)
{
  // release our connection to our manager process
  Client::Cleanup();
}

// Called from Debug if an exception occurs
static void TerminateListener(const Debug::TerminateArgs& args)
{
  // release our connection to our manager process
  Client::Cleanup();
}

bool Client::Initialize()
{
  IPC::PipeConnection* connection = new IPC::PipeConnection ();

  // init pipe connection with this process' process id (hex)
  std::ostringstream stream;

  if (Nocturnal::GetCmdLineFlag( Worker::Args::Debug ))
  {
    stream << "worker_debug";
  }
  else
  {
    stream << "worker_" << std::hex << GetProcessId(GetCurrentProcess());
  }

  connection->Initialize(false, "Worker Process Connection", stream.str().c_str());

  // setup global connection
  g_Connection = connection;

  // wait a while for connection
  int timeout = DefaultWorkerTimeout;

  // wait forever is we are asked to
  if (Nocturnal::GetCmdLineFlag( Worker::Args::Wait ))
  {
    timeout = -1;
  }

  while ( timeout-- != 0 && g_Connection->GetState() != IPC::ConnectionStates::Active )
  {
    Sleep( 1 );
  }

  // error out with an exception if we didnt' connect
  if (g_Connection->GetState() != IPC::ConnectionStates::Active)
  {
    Console::Error("Timeout connecting to manager process");
    return false;
  }

  // hook up our handler to Console
  Console::AddPrintedListener( Console::PrintedSignature::Delegate (&PrintedListener) );

  // hook up our handler to apputils
  AppUtils::g_ShuttingDown.Add( &ShutdownListener );

  // hook up our handler to debug
  Debug::g_Terminating.Add( &TerminateListener );

  return true;
}

void Client::Cleanup()
{
  if (g_Connection)
  {
    delete g_Connection;
    g_Connection = NULL;
  }
}

IPC::Message* Client::Receive(bool wait)
{
  IPC::Message* msg = NULL;

  if (g_Connection)
  {
    g_Connection->Receive(&msg);

    if (!msg && wait)
    {
      while (g_Connection->GetState() == IPC::ConnectionStates::Active && !msg)
      {
        Sleep(0);

        g_Connection->Receive(&msg);
      }
    }
  }

  return msg;
}

bool Client::Send(u32 id, u32 size, const u8* data)
{
  if (g_Connection && g_Connection->GetState() == IPC::ConnectionStates::Active)
  {
    IPC::Message* msg = g_Connection->CreateMessage(id, data ? size : 0);

    if (data && size)
    {
      memcpy(msg->GetData(), data, size);
    }

    if (g_Connection->Send(msg) == IPC::ConnectionStates::Active)
    {
      return true;
    }
    else
    {
      delete msg;
    }
  }

  return false;
}