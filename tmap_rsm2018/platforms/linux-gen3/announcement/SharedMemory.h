/**
 *  \file      SharedMemory.h
 *  \brief     POSIX shared memory helper
 *  \authors   Fang Pengfei (CM/ESC1-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Dec 2, 2015
 *  \copyright (C) 2015 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */
#ifndef SHAREDMEMORY_H_
#define SHAREDMEMORY_H_
#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

namespace ThirdPartyToolSet
{

/// \struct SHM_SYNC_INFO
/// \brief Struct for store process shared sync variables
struct SHM_SYNC_INFO
{
   pthread_cond_t cond;
   pthread_mutex_t mutex;
   uint32_t refrenceCounter;
   uint64_t signalCounter;
} ;

/// \class SharedMemoryBase
/// \brief Base class for shared memory
class SharedMemoryBase
{
public:
   SharedMemoryBase ( );
   virtual ~SharedMemoryBase ( );

protected:
   int _shm_fd;

   typedef enum _SHM_TYPE
   {
      SHM_TYPE_WRITE,
      SHM_TYPE_READ,
   } SHM_TYPE;

   SHM_SYNC_INFO* _p_shm_sync_info;

   int Open ( SHM_TYPE type);
   int Close ( );
   size_t Read ( void* outbuff, size_t buffsize );
   size_t Write ( const void* inbuff, size_t buffsize );

   size_t GetSize ( );
   void Erase();

   bool _sync_installed;
   bool SyncInit();
   bool SyncUninit();

   inline void Lock();
   inline void unLock();
   inline void Signal();
   inline bool WaitSignal();
};

/// \fn int DeleteSharedMemory ( )
/// \brief Delete shared memory, maybe need not to call
/// \warning This will delete shared memory from system
///    may break other process which is using the same name.
int DeleteSharedMemory ( );

/// \class SharedMemoryWriter
/// \brief Write data to shared memory
class SharedMemoryWriter: public SharedMemoryBase
{
public:
   SharedMemoryWriter ( );
   virtual ~SharedMemoryWriter ( );

   int Open ( );
   int Close ( );

   /// \brief Write data to shared memory
   /// \param[in] inbuff Pointer of data to write
   /// \param[in] size Data size
   /// \return Wrote size
   size_t Write ( const char* inbuff, size_t size );

   /// \brief Erase all data in shared memory
   void Erase();
};

/// \class SharedMemoryReader
/// \brief Read data from shared memory
class SharedMemoryReader: public SharedMemoryBase
{
public:
   SharedMemoryReader ( );
   virtual ~SharedMemoryReader ( );

   int Open ( );
   int Close ( );

   /// \brief Wait for writing signal
   bool Wait();

   /// \brief Get data size
   size_t GetSize ( );
   /// \brief Read(Copy) shared data to the outbuff
   /// \param[out] outbuff Pointer of data to read
   /// \param[in] buffsize Output buffer size
   /// \return Read size
   size_t Read ( void* outbuff, size_t buffsize );

};

} /* namespace ThirdPartyToolSet */
#endif /* SHAREDMEMORY_H_ */
