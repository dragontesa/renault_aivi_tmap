/**
 *  \file      SharedMemory.cpp
 *  \brief     POSIX shared memory helper
 *  \authors   Fang Pengfei (CM/ESC1-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Dec 2, 2015
 *  \copyright (C) 2015 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */
#include "SharedMemory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h>    /* For O_* constants */
#include <errno.h>    /* For errno */
#include <limits.h>
#include <error.h>

#if 1
#define DEBUG_TRACE(format, ...) fprintf(stderr, "%s:%d:%s: " format, __FILE__, __LINE__, __PRETTY_FUNCTION__, ##__VA_ARGS__)
#else
#define DEBUG_TRACE(...) ((void)0)
#endif

namespace ThirdPartyToolSet
{

#include "ThirdPartyShareMemoryName.h"

SharedMemoryBase::SharedMemoryBase ( )
{
   _shm_fd = -1;
   _p_shm_sync_info = NULL;
   _sync_installed = SyncInit();
}

SharedMemoryBase::~SharedMemoryBase ( )
{
   Close();
   SyncUninit();
}

bool SharedMemoryBase::SyncInit ( )
{
   //open the shared memory for sync
   int shm_sync_fd = shm_open(ThirdPartySharedMemorySyncName, O_RDWR | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)/*0644*/);
   if (shm_sync_fd == -1)
   {
      DEBUG_TRACE("shm_open error:%s\n", strerror(errno));
      return false;
   }
   struct stat sync_stat;
   int ret = fstat(shm_sync_fd, &sync_stat);
   if (ret)
   {
      DEBUG_TRACE("fstat error, %s\n", strerror(errno));
      return false;
   }
   bool init_needed = false;
   if (sync_stat.st_size == 0)  // 0 means newly created
   {
      DEBUG_TRACE("sync_stat.st_size == 0\n");
      if (ftruncate(shm_sync_fd, sizeof(SHM_SYNC_INFO)) < 0)
      {
         DEBUG_TRACE("ftruncate error:%s\n", strerror(errno));
         return false;
      }
      init_needed = true;
   }
   /* map memory to process */
   _p_shm_sync_info = (SHM_SYNC_INFO *) mmap(NULL, sizeof(SHM_SYNC_INFO), PROT_READ | PROT_WRITE, MAP_SHARED, shm_sync_fd, 0);
   if (_p_shm_sync_info == MAP_FAILED)
   {
      DEBUG_TRACE("mmap error:%s\n", strerror(errno));
      return false;
   }
   close(shm_sync_fd);

   if (init_needed)
   {
      DEBUG_TRACE("init_needed\n");
      pthread_mutexattr_t mutexattr;
      pthread_condattr_t condattr;
      if (pthread_mutexattr_init(&mutexattr))
      {
         DEBUG_TRACE("error init mutex\n");
         return false;
      }

      if (pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED))
      {
         DEBUG_TRACE("error mutex set shared\n");
         return false;
      }
      if (pthread_mutexattr_setrobust(&mutexattr, PTHREAD_MUTEX_ROBUST))
      {
         DEBUG_TRACE("error mutex set robust\n");
         return false;
      }

      if (pthread_condattr_init(&condattr))
      {
         DEBUG_TRACE("error init condition var\n");
         return false;
      }
      if (pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED))
      {
         DEBUG_TRACE("error condition set shared\n");
         return false;
      }
      if (pthread_condattr_setclock(&condattr, CLOCK_MONOTONIC))
      {
         DEBUG_TRACE("error condition set clock monotonic\n");
         return false;
      }

      _p_shm_sync_info->refrenceCounter = 0;
      _p_shm_sync_info->signalCounter = 0;

      if (pthread_mutex_init(&_p_shm_sync_info->mutex, &mutexattr))
      {
         DEBUG_TRACE("error mutex init\n");
         return false;
      }
      if (pthread_cond_init(&_p_shm_sync_info->cond, &condattr))
      {
         DEBUG_TRACE("error condition init \n");
         return false;
      }
   }
   pthread_mutex_lock(&_p_shm_sync_info->mutex);
   _p_shm_sync_info->refrenceCounter++;
   pthread_mutex_unlock(&_p_shm_sync_info->mutex);
   return true;
}

bool SharedMemoryBase::SyncUninit ( )
{
   bool unlink_needed = false;
   pthread_mutex_lock(&_p_shm_sync_info->mutex);
   _p_shm_sync_info->refrenceCounter--;
   unlink_needed = _p_shm_sync_info->refrenceCounter == 0 ? true : false;
   pthread_mutex_unlock(&_p_shm_sync_info->mutex);
   int ret = munmap(_p_shm_sync_info, sizeof(SHM_SYNC_INFO));
   if (-1 == ret)
   {
      DEBUG_TRACE("munmap error, %s\n", strerror(errno));
      return false;
   }
   if (unlink_needed)
   {
      DEBUG_TRACE("unlink_needed\n");
      ret = shm_unlink(ThirdPartySharedMemorySyncName);
      if (ret == -1)
      {
         DEBUG_TRACE("shm_unlink error, %s\n", strerror(errno));
         return false;
      }
   }
   return true;
}

void SharedMemoryBase::Lock ( )
{
   pthread_mutex_lock(&_p_shm_sync_info->mutex);
}

void SharedMemoryBase::unLock ( )
{
   pthread_mutex_unlock(&_p_shm_sync_info->mutex);
}

void SharedMemoryBase::Signal ( )
{
   pthread_mutex_lock(&_p_shm_sync_info->mutex);
   _p_shm_sync_info->signalCounter++;
   pthread_cond_signal(&_p_shm_sync_info->cond);
   pthread_mutex_unlock(&_p_shm_sync_info->mutex);
}
bool SharedMemoryBase::WaitSignal ( )
{
   pthread_mutex_lock(&_p_shm_sync_info->mutex);
   bool ret = (pthread_cond_wait(&_p_shm_sync_info->cond, &_p_shm_sync_info->mutex) == 0) ? true : false;
   pthread_mutex_unlock(&_p_shm_sync_info->mutex);
   return ret;
}

int SharedMemoryBase::Open ( SHM_TYPE type )
{
   if (!_sync_installed && !SyncInit())
      return -1;
   //open the shared memory for data
   switch (type)
   {
   case SHM_TYPE_WRITE:
      _shm_fd = shm_open(ThirdPartySharedMemoryName, O_RDWR | O_CREAT | O_TRUNC, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)/*0644*/);
      break;
   case SHM_TYPE_READ:
      _shm_fd = shm_open(ThirdPartySharedMemoryName, O_RDONLY | O_CREAT, (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)/*0644*/);
      break;
   }
   if (_shm_fd == -1)
   {
      DEBUG_TRACE("shm_open error:%s\n", strerror(errno));

      return errno;
   }
   return 0;
}

int SharedMemoryBase::Close ( )
{
   int ret = 0;
   if (_shm_fd > 0)
      ret = close(_shm_fd);
   if (-1 == ret)
   {
      DEBUG_TRACE("close error, %s\n", strerror(errno));
      return errno;
   }
   _shm_fd = 0;

   return 0;
}

size_t SharedMemoryBase::Read ( void* outbuff, size_t buffsize )
{
   size_t size_read = 0;
   if (_shm_fd <= 0)
   {
      DEBUG_TRACE("Shared memory is not opened!\n");
      return 0;
   }

   struct stat shm_stat;
   Lock();
   int ret = fstat(_shm_fd, &shm_stat);
   if (0 == ret)
   {
      if (shm_stat.st_size > 0)
      {
         void* map_ptr = mmap(NULL, shm_stat.st_size, PROT_READ, MAP_SHARED, _shm_fd, 0);  // map to shared memory
         if (MAP_FAILED == map_ptr)
         {
            DEBUG_TRACE("mmap error, %s\n", strerror(errno));
         }
         else
         {
            size_read = buffsize >= (size_t) shm_stat.st_size ?
                                                                (size_t) shm_stat.st_size :
                                                                buffsize;
            memcpy(outbuff, map_ptr, size_read);
            ret = munmap(map_ptr, size_read);
            if (-1 == ret)
            {
               DEBUG_TRACE("munmap error, %s\n", strerror(errno));
            }
         }
      }
   }
   else
   {
      DEBUG_TRACE("fstat error, %s\n", strerror(errno));
   }

   unLock();
   return size_read;
}


size_t SharedMemoryBase::Write ( const void* inbuff, size_t buffsize )
{
   size_t size_write = 0;

   if (_shm_fd <= 0)
   {
      DEBUG_TRACE("Shared memory is not opened!\n");
      return 0;
   }

   struct stat shm_stat;
   Lock();
   int ret = fstat(_shm_fd, &shm_stat);
   if (0 == ret)
   {
      ret = ftruncate(_shm_fd, buffsize);  // modify shared memory size
      if (-1 == ret)
      {
         DEBUG_TRACE("ftruncate error, %s\n", strerror(errno));
      }
      else
      {
         void* map_ptr = mmap(NULL, buffsize, PROT_WRITE, MAP_SHARED, _shm_fd, 0);  // map to shared memory
         if (MAP_FAILED == map_ptr)
         {
            DEBUG_TRACE("mmap error, %s\n", strerror(errno));
         }
         else
         {
            memcpy(map_ptr, inbuff, buffsize);
            size_write = buffsize;
            ret = munmap(map_ptr, buffsize);
            if (-1 == ret)
            {
               DEBUG_TRACE("munmap error, %s\n", strerror(errno));
            }
         }
      }
   }
   else
   {
      DEBUG_TRACE("fstat error, %s\n", strerror(errno));
   }
   unLock();
   //When finish writing PCM data to shared memory, third party navigation does not need to send Signal!!!!!
   //Because the Wrapper will do it internally.
   //if (size_write)
      //Signal();
   return size_write;
}

size_t SharedMemoryBase::GetSize ( )
{
   if (_shm_fd <= 0)
   {
      DEBUG_TRACE("Shared memory is not opened!\n");
      return -1;
   }
   struct stat shm_stat;
   Lock();
   int ret = fstat(_shm_fd, &shm_stat);
   if (ret)
   {
      DEBUG_TRACE("fstat error, %s\n", strerror(errno));
      return 0;
   }
   unLock();
   return shm_stat.st_size;
}

void SharedMemoryBase::Erase ( )
{
   if (_shm_fd <= 0)
   {
      DEBUG_TRACE("Shared memory is not opened!\n");
      return;
   }
   Lock();
   int ret = ftruncate(_shm_fd, 0);
   if (-1 == ret)
   {
      DEBUG_TRACE("ftruncate error, %s\n", strerror(errno));
   }
   unLock();
}

int DeleteSharedMemory ( const char* szShmName )
{
   int ret = shm_unlink(ThirdPartySharedMemoryName);
   if (ret == -1)
   {
      DEBUG_TRACE("shm_unlink error, %s\n", strerror(errno));
      return errno;
   }
   ret = shm_unlink(ThirdPartySharedMemorySyncName);
   if (ret == -1)
   {
      DEBUG_TRACE("shm_unlink error, %s\n", strerror(errno));
      return errno;
   }
   return 0;
}

SharedMemoryWriter::SharedMemoryWriter ( )
{
}

SharedMemoryWriter::~SharedMemoryWriter ( )
{
}

int SharedMemoryWriter::Open ( )
{
   return SharedMemoryBase::Open(SHM_TYPE_WRITE);
}

int SharedMemoryWriter::Close ( )
{
   return SharedMemoryBase::Close();
}

size_t SharedMemoryWriter::Write ( const char* inbuff, size_t size )
{
   return SharedMemoryBase::Write(inbuff, size);
}

void SharedMemoryWriter::Erase ( )
{
   SharedMemoryBase::Erase();
}

SharedMemoryReader::SharedMemoryReader ( )
{
}

SharedMemoryReader::~SharedMemoryReader ( )
{
}

int SharedMemoryReader::Open ( )
{
   return SharedMemoryBase::Open(SHM_TYPE_READ);
}

int SharedMemoryReader::Close ( )
{
   return SharedMemoryBase::Close();
}

size_t SharedMemoryReader::GetSize ( )
{
   return SharedMemoryBase::GetSize();
}

size_t SharedMemoryReader::Read ( void* outbuff, size_t buffsize )
{
   return SharedMemoryBase::Read(outbuff, buffsize);
}

bool SharedMemoryReader::Wait ( )
{
   return SharedMemoryBase::WaitSignal();
}

} /* namespace ThirdPartyToolSet */
