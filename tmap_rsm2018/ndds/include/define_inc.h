// define_inc.h

// 작성자 : 우경일
// 작성일 : 2008.05.22 
// 설  명 : 프로젝트에 연결된 모든 모듈에서 공용으로 사용되는 #define을  
//			정의한다. 
//			(참고 : FineMap의 NdDefineHeader.h와 역활이 동일함.)

#ifndef __DEFINE_INC_H
#define __DEFINE_INC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#ifdef _LINUX
#include <pthread.h>
#endif

#include "disable_warning_inc.h"
#include "global.h"
#include "debug.h"

// NOFP 실수 연산 안하게 처리
#define NOFP_DEG	10000.0f

#if 0 //상공회의소
#define	BASE_POS_X	66573602
#define	BASE_POS_Y	19695617
#else // T tower
#define	BASE_POS_X	66571920
#define	BASE_POS_Y	19691179
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ABS
#define ABS(x)				(((x) > 0) ? (x) : -(x))
#endif

#ifndef MIN
#define MIN(a, b)			(((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)			(((a) > (b)) ? (a) : (b))
#endif

#ifndef DEG2RAD
#define DEG2RAD(a)			(((a) * PI) / 180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG(a)			(((a) * 180.0f) / PI)
#endif

#endif // __DEFINE_INC_H
