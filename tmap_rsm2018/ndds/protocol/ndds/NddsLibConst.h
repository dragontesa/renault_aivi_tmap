// NddsLibConst.h

#ifndef __NDDSLIB_CONST_H
#define __NDDSLIB_CONST_H

namespace NDDSLIBCONST
{
	// Ndds Tmap
    static char		NDDS_TMAP_HOST_DEVELOPMENT[]		= "ntmapdev.tmap.co.kr";	// TNAPP 개발 URL
	static char		NDDS_TMAP_HOST_COMMERCE_1[]			= "ntmap.tmap.co.kr";	// TNAPP 상용 URL(보라매)
	static char		NDDS_TMAP_HOST_COMMERCE_2[]			= "ntmap2.tmap.co.kr";	// TNAPP 상용 URL(성수)
	static int		NDDS_TMAP_PORT						= 9443;			// TNAPP 개발/상용 포트

	static char		NDDS_TMAP_TRS_SERVERID_DEVELOPMENT[]		= "NDDS02";			// NDDS 개발
	static char		NDDS_TMAP_TRS_SERVERID_SAME_COMMERCE[]	= "NDDS01";			// NDDS 준상용
	static char		NDDS_TMAP_TRS_SERVERID_COMMERCE[]			= "NDDS00";			// NDDS 상용
}

#endif // __NDDSLIB_CONST_H
