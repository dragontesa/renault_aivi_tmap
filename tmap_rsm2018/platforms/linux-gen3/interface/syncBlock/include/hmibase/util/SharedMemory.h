/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_UTIL_SHAREDMEMORY_H
#define HMIBASE_UTIL_SHAREDMEMORY_H

#include <stdint.h>
#include <stddef.h>
#include "hmibase/util/SimpleString.h"

namespace hmibase {
namespace util {

class SharedMemory
{
   public:
      /** Create an instance of SharedMemory.
        * If isMaster, this instance will create and own the memory.
        * If not isMaster, this instance will connect to existing memory */
      SharedMemory(const char* name, const size_t numBytes, bool isMaster = false);

      /** If isMaster, this instance will unlink the memory upon destruction. */
      virtual ~SharedMemory();

      /** Return the size of the shared memory */
      size_t getNumBytes() const
      {
         return mNumBytes;
      }

      /** Return a pointer to the shared memory */
      void*  getDataPtr() const
      {
         return mDataPtr ;
      }

      /** If your process was killed and the destructor of the master instance was not called,
        * the created shared memory will continue to exist (in /dev/shm).
        * Purge will clean up the garbage semaphore from a previous process run. */
      static void purge(const char* name);

   private:
      SimpleString mName;
      bool         mIsOwner;
      int          mFileDescriptor;
      size_t       mNumBytes;
      void*        mDataPtr;

      SimpleString createErrorMessage(const SimpleString& message) const;
      static SimpleString fixName(const SimpleString& name);

      // Thou shalt not copy
      SharedMemory(const SharedMemory&);
      SharedMemory& operator=(const SharedMemory&);
};


} // namespace
} // namespace
#endif
