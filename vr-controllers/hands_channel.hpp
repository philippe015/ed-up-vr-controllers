#pragma once
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <cwchar>
#include "virtual_hands.hpp"
namespace edup::vrinput::hands {
// Local diagnostic only; nonblocking writer, fixed-size payload, no files/network.
class Channel {
 HANDLE map_=nullptr,lock_=nullptr;Snapshot* ptr_=nullptr;bool writer_=false;
public:
 Channel()=default;Channel(const Channel&)=delete;Channel& operator=(const Channel&)=delete;~Channel(){close();}
 bool open(DWORD pid,bool writer){close();wchar_t m[96],l[96];swprintf(m,96,L"Local\\EDUPHands07_%lu",(unsigned long)pid);swprintf(l,96,L"Local\\EDUPHands07Mutex_%lu",(unsigned long)pid);writer_=writer;
  lock_=writer?CreateMutexW(nullptr,FALSE,l):OpenMutexW(SYNCHRONIZE|MUTEX_MODIFY_STATE,FALSE,l);if(!lock_)return false;
  map_=writer?CreateFileMappingW(INVALID_HANDLE_VALUE,nullptr,PAGE_READWRITE,0,sizeof(Snapshot),m):OpenFileMappingW(FILE_MAP_READ,FALSE,m);if(!map_){close();return false;}
  ptr_=static_cast<Snapshot*>(MapViewOfFile(map_,writer?FILE_MAP_WRITE:FILE_MAP_READ,0,0,sizeof(Snapshot)));if(!ptr_){close();return false;}return true;
 }
 void close(){if(ptr_)UnmapViewOfFile(ptr_);if(map_)CloseHandle(map_);if(lock_)CloseHandle(lock_);ptr_=nullptr;map_=lock_=nullptr;writer_=false;}
 bool write(Snapshot s){if(!ptr_||!writer_)return false;DWORD r=WaitForSingleObject(lock_,0);if(r!=WAIT_OBJECT_0&&r!=WAIT_ABANDONED)return false;s.tick=GetTickCount64();*ptr_=s;ReleaseMutex(lock_);return true;}
 bool read(Snapshot& s){if(!ptr_||writer_)return false;DWORD r=WaitForSingleObject(lock_,0);if(r!=WAIT_OBJECT_0&&r!=WAIT_ABANDONED)return false;bool ok=r==WAIT_OBJECT_0;if(ok)s=*ptr_;ReleaseMutex(lock_);auto now=GetTickCount64();return ok&&s.version==2&&s.tick&&now>=s.tick&&now-s.tick<=250;}
};
}
#endif
