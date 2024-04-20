/**
 *  \file      ThirdPartyShareMemoryName.h
 *  \brief     Store unique name for POSIX shared memory helper
 *  \authors   Fang Pengfei (CM/ESC1-CN)<Pengfei.Fang@cn.bosch.com>
 *  \version   1.0
 *  \date      Dec 7, 2015
 *  \copyright (C) 2015 BOSCH Automotive Products (Suzhou) Co. Ltd.
 */
#ifndef THIRDPARTYSHAREMEMORYNAME_H_
#define THIRDPARTYSHAREMEMORYNAME_H_

/// \var ThirdPartySharedMemoryName
/// \brief Used for shared data
static const char* ThirdPartySharedMemoryName = "/ThirdPartyNaviSharedMemory.shm";
/// \var ThirdPartySharedMemorySyncName
/// \brief Used for process shared sync variables
static const char* ThirdPartySharedMemorySyncName = "/ThirdPartyNaviSharedMemory.sync";

#endif /* THIRDPARTYSHAREMEMORYNAME_H_ */
