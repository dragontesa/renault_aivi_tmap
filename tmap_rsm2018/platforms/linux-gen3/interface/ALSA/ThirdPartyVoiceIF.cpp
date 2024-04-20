/*
 * ThirdPartyVoiceIF.cpp
 *
 *  Created on: Jan 15, 2018
 *      Author: man1szh
 */

#include "ThirdPartyVoiceIF.h"

#include <dlt/dlt.h>

//DLT_IMPORT_CONTEXT(NavigationExt_Context)

namespace third{
namespace party{
namespace voice{

ThirdPartyVoiceIF::ThirdPartyVoiceIF ( )
{
   // TODO Auto-generated constructor stub

}

ThirdPartyVoiceIF::~ThirdPartyVoiceIF ( )
{
   // TODO Auto-generated destructor stub

}

void ThirdPartyVoiceIF::vOnConnectionReady()
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
}

void ThirdPartyVoiceIF::vOnAllcocateSignal(guchar arg_source, const gchar *arg_sink)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));

}

void ThirdPartyVoiceIF::vOnDeAllcocateSignal(guchar arg_source)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
}

void ThirdPartyVoiceIF::vOnMuteStateSignal(guchar arg_source)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));

}

void ThirdPartyVoiceIF::vOnSourceActivitySignal(guchar arg_source, guint16 arg_status)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
}


void ThirdPartyVoiceIF::vAudioRouteRequestCallback(bool bStatus)
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
}

void ThirdPartyVoiceIF::vChangedAudioStatus(guchar bStatus)
{

}

void ThirdPartyVoiceIF::vSourceAvailabilityCallback()
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
}

void ThirdPartyVoiceIF::vMuteStateCallback()
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
}

void ThirdPartyVoiceIF::vSourceActivityResultCallback()
{
   //DLT_LOGNavigationExt_Context,DLT_LOG_DEBUG,DLT_STRING(__FUNCTION__));
}

}
}
}
