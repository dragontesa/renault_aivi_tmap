/* ***************************************************************************************
* FILE:          TraceUtils.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  TraceUtils.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef TRACEUTILS_H
#define TRACEUTILS_H

#ifdef __cplusplus

#include <string>
#include "hmibase/util/ItemRegistry.h"

namespace hmibase {
namespace trace {

std::string getAppName();

typedef hmibase::util::ItemValRegistry<std::string, int> SimInputDeviceId; // Map the input names to project specific ids

class TraceUtils
{
   public:
      static TraceUtils& getInstance();

      std::string getAppName()
      {
         return _appName;
      }

   private:
      TraceUtils();
      std::string _appName;

      static TraceUtils* _theInstance;
};


namespace widget {
void setUserTraceClass(unsigned short traceclassid);
unsigned short getApplicationTraceClass();
}


typedef struct CGITraceGroup
{
   unsigned short traceClass_DataBinding;    // TR_CLASSOFFSET_APPHMI_CGI_DATABINDING
   unsigned short traceClass_Visualization;  // TR_CLASSOFFSET_APPHMI_CGI_VISUALIZATION
   unsigned short traceClass_Messaging;      // TR_CLASSOFFSET_APPHMI_CGI_MESSAGING
   unsigned short traceClass_CanderaAsset;   // TR_CLASSOFFSET_APPHMI_CGI_CANDERAASSET
   unsigned short traceClass_Debug;          // TR_CLASSOFFSET_APPHMI_CGI_DEBUG, trace all
   unsigned short traceClass_Widget;         // TR_CLASSOFFSET_APPHMI_WIDGET
   unsigned short traceClass_CourierPayload_DataBinding; // TR_CLASSOFFSET_APPHMI_COURIER_PAYLOAD_DATABINDING

   CGITraceGroup() :
      traceClass_DataBinding(0),
      traceClass_Visualization(0),
      traceClass_Messaging(0),
      traceClass_CanderaAsset(0),
      traceClass_Debug(0),
      traceClass_Widget(0),
      traceClass_CourierPayload_DataBinding(0)    {}

   CGITraceGroup(unsigned short db, unsigned short candera, unsigned short messaging, unsigned short asset, unsigned short debug, unsigned short widget, unsigned short courierPayloadDB) :
      traceClass_DataBinding(db),
      traceClass_Visualization(candera),
      traceClass_Messaging(messaging),
      traceClass_CanderaAsset(asset),
      traceClass_Debug(debug),
      traceClass_Widget(widget),
      traceClass_CourierPayload_DataBinding(courierPayloadDB) {}
} sCGITraceGroup;
}


}


#endif

#define APPHMI_STANDARD_TRACE_SET(n, x) \
      static const unsigned short  TR_CLASS_APPHMI_##x##_MAIN                             = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_MAIN             );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_HALL                             = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_HALL             );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_GUI_DATABINDING                  = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_DATABINDING  );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_GUI_VISUALIZATION                = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_VISUALIZATION);  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_GUI_MESSAGING                    = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_MESSAGING    );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_GUI_DEBUG                        = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_DEBUG        );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_SM                               = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_SM               );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_APPCTRL_PROXY                    = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_APPCTRL_PROXY    );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_GUI                              = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI              );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_WIDGET                           = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_WIDGET           );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_DM                               = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_DM               );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_MSG_PAYLOAD_MODEL_COMP           = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_MODEL        );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_MSG_PAYLOAD_VIEW_COMP            = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_VIEW         );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_MSG_PAYLOAD_CTRL_COMP            = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_CONTROLLER   );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_MSG_PAYLOAD_DATABINDING          = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_DATABINDING  );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_GUI_ASSETRESOURCE                = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_ASSETRESOURCE);  \
 /* deprecated since 07/2017 */ \
      static const unsigned short  TR_CLASS_APPHMI_##x##_CGI_DATABINDING            /*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_DATABINDING);    \
      static const unsigned short  TR_CLASS_APPHMI_##x##_CGI_VISUALIZATION          /*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_VISUALIZATION);  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_CGI_MESSAGING              /*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_MESSAGING    );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_CGI_DEBUG                  /*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_GUI_DEBUG        );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_COURIER_PAYLOAD_MODEL_COMP /*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_MODEL);          \
      static const unsigned short  TR_CLASS_APPHMI_##x##_COURIER_PAYLOAD_VIEW_COMP  /*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_VIEW         );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_COURIER_PAYLOAD_CTRL_COMP  /*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_CONTROLLER   );  \
      static const unsigned short  TR_CLASS_APPHMI_##x##_COURIER_PAYLOAD_DATABINDING/*deprecated*/ = (TR_COMP_UI_APP_HMI_##n +TR_CLASSOFFSET_APPHMI_CPL_DATABINDING  );  \
\
static const hmibase::trace::sCGITraceGroup COURIER_APPLICATION_TRACE_GROUP(	\
      TR_CLASS_APPHMI_##x##_GUI_DATABINDING,          \
      TR_CLASS_APPHMI_##x##_GUI_VISUALIZATION,        \
      TR_CLASS_APPHMI_##x##_GUI_MESSAGING,            \
      TR_CLASS_APPHMI_##x##_GUI_ASSETRESOURCE,        \
      TR_CLASS_APPHMI_##x##_GUI_DEBUG,                \
      TR_CLASS_APPHMI_##x##_WIDGET,                   \
      TR_CLASS_APPHMI_##x##_MSG_PAYLOAD_DATABINDING); \
		\
      static const unsigned short  TR_TTFIS_APPHMI_##x = TR_TTFIS_UI_APP_HMI_##n;
#define APP_TRACECLASS_ID() hmibase::trace::widget::getApplicationTraceClass()   // e.g. TR_CLASS_APPHMI_TUNER_WIDGET, TR_CLASS_APPHMI_DEMOWIDGETS_WIDGET, ....

#endif // TRACEUTILS_H
