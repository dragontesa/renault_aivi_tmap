/******************************************************************
*COPYRIGHT: (C) 2017 Robert Bosch GmbH
*The reproduction, distribution and utilization of this file as
*well as the communication of its contents to others without express
*authorization is prohibited. Offenders will be held liable for the
*payment of damages. All rights reserved in the event of the grant
*of a patent, utility model or design.
******************************************************************/
#ifndef HMIBASE_GADGET_SYNCBLOCK2_PRODUCERTYPE_H
#define HMIBASE_GADGET_SYNCBLOCK2_PRODUCERTYPE_H

namespace hmibase {
namespace gadget {
namespace syncblock2 {

enum ProducerType
{
   INVALID_PRODUCER = 0,
   SHM_PRODUCER     = 1,
   SOCKET_PRODUCER  = 2
};


} // namespace
} // namespace
} // namespace

#endif
