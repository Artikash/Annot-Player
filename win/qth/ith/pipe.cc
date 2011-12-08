// ith/pipe.cc
// 10/14/2011
#include "ith/main.h"
#include "ith/hookman.h"
#include "ith/cmdq.h"

//#define DEBUG "ith:pipe"
#include "module/debug/debug.h"

#define NAMED_PIPE_DISCONNECT 1
#define NAMED_PIPE_CONNECT 2
static WCHAR pipe[]=L"\\??\\pipe\\ITH_PIPE";
static WCHAR command[]=L"\\??\\pipe\\ITH_COMMAND";
static bool newline=false;
static bool detach=false;
CRITICAL_SECTION detach_cs;
HANDLE hDetachEvent;
extern HANDLE hPipeExist;

namespace { // anonymous
  BYTE* Filter(BYTE *str, int len)
  {
    WORD s;
    while (true) {
      s = *(WORD*)str;
      if (len >= 2) {
        if (s==0x4081 || s==0x3000 || s<=0x20) { str+=2; len-=2; }
        else break;
      }
      else if (str[0]<=0x20) { str++; len--; }
      else break;
    }
    return str;
  }
} // anonymous namespace

void CreateNewPipe()
{
  DWORD acl[7]={0x1C0002,1,0x140000,GENERIC_READ|GENERIC_WRITE|SYNCHRONIZE,0x101,0x1000000,0};
  SECURITY_DESCRIPTOR sd={1,0,4,0,0,0,(PACL)acl};
  HANDLE hTextPipe,hCmdPipe,hThread;
  IO_STATUS_BLOCK ios;
  UNICODE_STRING us;
  RtlInitUnicodeString(&us,pipe);
  OBJECT_ATTRIBUTES oa={sizeof(oa),0,&us,OBJ_CASE_INSENSITIVE,&sd,0};
  LARGE_INTEGER time={-500000,-1};
  if (!NT_SUCCESS(NtCreateNamedPipeFile(&hTextPipe,GENERIC_READ|SYNCHRONIZE,&oa,&ios,
    FILE_SHARE_WRITE,FILE_OPEN_IF,FILE_SYNCHRONOUS_IO_NONALERT,1,1,0,-1,0x1000,0x1000,&time)))
  {
    //ConsoleOutput(ErrorCreatePipe);
    DOUT("CreateNewPipe: error");
    return;
  }
  RtlInitUnicodeString(&us,command);
  if (!NT_SUCCESS(NtCreateNamedPipeFile(&hCmdPipe,GENERIC_WRITE|SYNCHRONIZE,&oa,&ios,
    FILE_SHARE_READ,FILE_OPEN_IF,FILE_SYNCHRONOUS_IO_NONALERT,1,1,0,-1,0x1000,0x1000,&time)))
  {
    //ConsoleOutput(ErrorCreatePipe);
    DOUT("CreateNewPipe: error");
    return;
  }
  hThread=IthCreateThread(RecvThread,(DWORD)hTextPipe);
  man->RegisterPipe(hTextPipe,hCmdPipe,hThread);
}

void DetachFromProcess(DWORD pid)
{
  HANDLE hMutex,hEvent;
  IO_STATUS_BLOCK ios;
  //module=man->GetModuleByPID(pid);
  hEvent=IthCreateEvent(0);
  if (STATUS_PENDING==NtFsControlFile(man->GetCmdHandleByPID(pid),hEvent,0,0,&ios,
    CTL_CODE(FILE_DEVICE_NAMED_PIPE,NAMED_PIPE_DISCONNECT,0,0),0,0,0,0))
    NtWaitForSingleObject(hEvent,0,0);
  NtClose(hEvent);
  WCHAR mt[0x20];
  swprintf(mt,L"ITH_DETACH_%d",pid);

  hMutex=IthOpenMutex(mt);
  if (hMutex!=INVALID_HANDLE_VALUE)
  {
    NtWaitForSingleObject(hMutex,0,0);
    NtReleaseMutant(hMutex,0);
    NtClose(hMutex);
  }
  NtSetEvent(hDetachEvent,0);
  NtSetEvent(hPipeExist,0);
}
/*
void OutputDWORD(DWORD d)
{
  WCHAR str[0x20];
  swprintf(str,L"%.8X",d);
  ConsoleOutput(str);
}
*/
DWORD WINAPI RecvThread(LPVOID lpThreadParameter)
{
  HANDLE hTextPipe=(HANDLE)lpThreadParameter, hDisconnect;
  IO_STATUS_BLOCK ios; NTSTATUS status;
  NtFsControlFile(hTextPipe,0,0,0,&ios,CTL_CODE(FILE_DEVICE_NAMED_PIPE,NAMED_PIPE_CONNECT,0,0),0,0,0,0);
  if (!running||texts==0) return 0;
  DWORD pid,hookman,p,module,engine;
  BYTE *buff=new BYTE[0x1000],*it;
  NtReadFile(hTextPipe,0,0,0,&ios,buff,16,0,0);
  pid=*(DWORD*)buff;
  hookman=*(DWORD*)(buff+0x4);
  module=*(DWORD*)(buff+0x8);
  engine=*(DWORD*)(buff+0xC);
  man->RegisterProcess(pid,hookman,module,engine);
  CreateNewPipe();
  DWORD RecvLen;
  while (running)
  {
    //qDebug()<<"1:"<<lpThreadParameter;
    status=NtReadFile(hTextPipe,0,0,0,&ios,buff,0x1000,0,0);
    //qDebug()<<"2:"<<lpThreadParameter;
    if (!NT_SUCCESS(status)) break;
    RecvLen=ios.uInformation;
    if (RecvLen<0xC) break;
    p=pid;
    DWORD hook=*(DWORD*)buff;
    union{DWORD retn; DWORD cmd_type;};
    union{DWORD split; DWORD new_engine_addr;};
    retn=*((DWORD*)buff+1);
    split=*((DWORD*)buff+2);
    buff[RecvLen]=0;
    buff[RecvLen+1]=0;
    it=Filter(buff+0xC,RecvLen-0xC);
    RecvLen=RecvLen-(it-buff);
    if (RecvLen>>31) RecvLen=0;
    //qDebug() << cmd_type;
    if (hook+1==0)
    {
      switch (cmd_type)
      {
      case 2:
        man->GetProcessPath(pid,(LPWSTR)(buff+0xC));
        man->SetProcessEngineType(pid,*(DWORD*)(buff+0x8));
        //pfman->SetProfileEngine((LPWSTR)(buff+0xC),*(DWORD*)(buff+8));
        break;
      case 1:
        man->GetProcessPath(pid,(LPWSTR)buff);
        //pfman->RefreshProfileAddr(pid,(LPWSTR)buff);
        break;
      case 0:
        //entry_table->RegisterNewHook(new_engine_addr,(LPWSTR)(buff+0xC),pid);
        break;
      case -1:
        swprintf((LPWSTR)buff,L"%.4d:",pid);
        buff[0xA]=0x20;
        //ConsoleOutput((LPWSTR)buff);
        break;
      }
    }
    else {
      man->DispatchText(p, it,hook,retn,split,RecvLen);
    }
  }
  EnterCriticalSection(&detach_cs);
  hDisconnect=IthCreateEvent(0);
  if (STATUS_PENDING==NtFsControlFile(hTextPipe,hDisconnect,0,0,&ios,
    CTL_CODE(FILE_DEVICE_NAMED_PIPE,NAMED_PIPE_DISCONNECT,0,0),0,0,0,0))
    NtWaitForSingleObject(hDisconnect,0,0);
  NtClose(hDisconnect);
  DetachFromProcess(pid);
  man->UnRegisterProcess(pid);
  NtClearEvent(hDetachEvent);
  LeaveCriticalSection(&detach_cs);
  //if (running)
  //{
  //  swprintf((LPWSTR)buff,FormatDetach,pid);
  //  ConsoleOutput((LPWSTR)buff);
  //}
  //DOUT("succeed");
  delete buff;
  return 0;
}
DWORD WINAPI CmdThread(LPVOID lpThreadParameter)
{
  CommandQueue *q = (CommandQueue*)lpThreadParameter;
  while (running)
    q->SendCommand();
  return 0;
}

CommandQueue::CommandQueue():used(0),current(1)
{
  DOUT("CommandQueue:CommandQueue:enter");
  InitializeCriticalSection(&rw);
  NtCreateSemaphore(&hSemaphore,SEMAPHORE_ALL_ACCESS,0,0,QUEUE_MAX);
  hThread=IthCreateThread(CmdThread,(DWORD)this);
  DOUT("CommandQueue:CommandQueue:exit");
}
CommandQueue::~CommandQueue()
{
  DOUT("CommandQueue:~CommandQueue:enter");
  NtReleaseSemaphore(hSemaphore,1,0);

  DOUT("CommandQueue:~CommandQueue: FIXME: termating hanging hThread ...");
  // jichi:10/15/2011: hanged orz
  //NtWaitForSingleObject(hThread,0,0);
  NtTerminateThread(hThread, 0);

  NtClose(hSemaphore);
  NtClose(hThread);
  DeleteCriticalSection(&rw);
  DOUT("CommandQueue:~CommandQueue:exit");
}
void CommandQueue::AddRequest(const SendParam& sp, DWORD pid)
{
  if (current==used) {
    //ConsoleOutput(ErrorCmdQueueFull);
    DOUT("AddRequiest: command queue is full");
  }
  EnterCriticalSection(&rw);
  queue[current]=sp;
  if (pid)
    pid_associate[current++]=pid;
  else
  {
    pid=man->GetCurrentPID();
    if (pid)
      pid_associate[current++]=pid;
    else
    {
      //ConsoleOutput(ErrorNoAttach);
      DOUT("AddRequest:error: not attached ");
      goto _request_exit;
    }
  }
  current&=(QUEUE_MAX-1);
  NtReleaseSemaphore(hSemaphore,1,0);
_request_exit:
  LeaveCriticalSection(&rw);
}
void CommandQueue::SendCommand()
{
  NtWaitForSingleObject(hSemaphore,0,0);
  if (!running) return;
  EnterCriticalSection(&rw);
  SendParam sp;
  DWORD pid;
  HANDLE pipe;
  used=(used+1)&(QUEUE_MAX-1);
  sp=queue[used];
  pid=pid_associate[used];
  pipe=man->GetCmdHandleByPID(pid);
  if (pipe)
  {
    IO_STATUS_BLOCK ios;
    NtWriteFile(pipe,0,0,0,&ios,&sp,sizeof(SendParam),0,0);
  }
  LeaveCriticalSection(&rw);
}
bool CommandQueue::Empty()
{
  return ((used+1)&(QUEUE_MAX-1))==current;
}
void CommandQueue::Register(DWORD pid, DWORD hookman, DWORD module, DWORD engine)
{
  man->RegisterProcess(pid,hookman,module,engine);
}

// EOF
