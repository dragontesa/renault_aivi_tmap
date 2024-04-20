/* ***************************************************************************************
* FILE:          mc_trace_mock.h
* SW-COMPONENT:  HMI-BASE
*  DESCRIPTION:  mc_trace_mock.h is part of HMI-Base framework Library
*    COPYRIGHT:  (c) 2015-2016 Robert Bosch Car Multimedia GmbH
*
* The reproduction, distribution and utilization of this file as well as the
* communication of its contents to others without express authorization is
* prohibited. Offenders will be held liable for the payment of damages.
* All rights reserved in the event of the grant of a patent, utility model or design.
*
*************************************************************************************** */

#ifndef HMIBASE_UTIL_MC_TRACE_MOCK__H
#define HMIBASE_UTIL_MC_TRACE_MOCK__H

#ifndef TR_COMP_UI   // if mc_trace.h not included
#define TR_COMP_UI                     0x2800
#define TR_TTFIS_UI_APP_HMI_MASTER     276
#define TR_TTFIS_UI_APP_HMI_PLAYGROUND 277
#define TR_TTFIS_UI_APP_HMI_00         277 // == TR_TTFIS_UI_APP_HMI_PLAYGROUND
#define TR_TTFIS_UI_APP_HMI_01         278
#define TR_TTFIS_UI_APP_HMI_02         279
#define TR_TTFIS_UI_APP_HMI_03         280
#define TR_TTFIS_UI_APP_HMI_04         281
#define TR_TTFIS_UI_APP_HMI_05         282
#define TR_TTFIS_UI_APP_HMI_06         283
#define TR_TTFIS_UI_APP_HMI_07         284
#define TR_TTFIS_UI_APP_HMI_08         285
#define TR_TTFIS_UI_APP_HMI_09         286
#define TR_TTFIS_UI_APP_HMI_10         287
#define TR_TTFIS_UI_APP_HMI_11         288
#define TR_TTFIS_UI_APP_HMI_12         289
#define TR_TTFIS_UI_APP_HMI_13         290
#define TR_TTFIS_UI_APP_HMI_14         291
#define TR_TTFIS_UI_APP_HMI_15         292
#define TR_TTFIS_UI_APP_HMI_16         293
#define TR_TTFIS_UI_APP_HMI_17         294
#define TR_TTFIS_UI_APP_HMI_18         295

#define TR_COMP_UI_APP_HMI_MASTER      ((256 * 21) +   0)
#define TR_COMP_UI_APP_HMI_00          ((256 * 21) +  64)
#define TR_COMP_UI_APP_HMI_01          ((256 * 21) + 128)
#define TR_COMP_UI_APP_HMI_02          ((256 * 21) + 192)
#define TR_COMP_UI_APP_HMI_03          ((256 * 22) +   0)
#define TR_COMP_UI_APP_HMI_04          ((256 * 22) +  64)
#define TR_COMP_UI_APP_HMI_05          ((256 * 22) + 128)
#define TR_COMP_UI_APP_HMI_06          ((256 * 22) + 192)
#define TR_COMP_UI_APP_HMI_07          ((256 * 23) +   0)
#define TR_COMP_UI_APP_HMI_08          ((256 * 23) +  64)
#define TR_COMP_UI_APP_HMI_09          ((256 * 23) + 128)
#define TR_COMP_UI_APP_HMI_10          ((256 * 23) + 192)
#define TR_COMP_UI_APP_HMI_11          ((256 * 27) +   0)
#define TR_COMP_UI_APP_HMI_12          ((256 * 27) +  64)
#define TR_COMP_UI_APP_HMI_13          ((256 * 27) + 128)
#define TR_COMP_UI_APP_HMI_14          ((256 * 27) + 192)
#define TR_COMP_UI_APP_HMI_15          ((256 * 78) +   0)
#define TR_COMP_UI_APP_HMI_16          ((256 * 78) +  64)
#define TR_COMP_UI_APP_HMI_17          ((256 * 78) + 128)
#define TR_COMP_UI_APP_HMI_18          ((256 * 78) + 192)
#endif // TR_COMP_UI

#endif // guard HMIBASE_UTIL_MC_TRACE_MOCK__H
