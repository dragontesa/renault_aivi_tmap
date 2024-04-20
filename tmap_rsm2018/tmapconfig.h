#ifndef TMAPCONFIG_H
#define TMAPCONFIG_H

#define TMAP_VERSION_MAJOR          "1"
#define TMAP_VERSION_MINOR          "20"
#define TMAP_VERSION_MICRO          "137"
#define TMAP_VERSION_BUILD_SEQ      "A"

/*
 *  Build Year : 2-digit
 *  Build Week : 2-digit (01~52)
 *  Build Day  : decimal (1~7, Mon(1), Tue(2), Wed(3)....Fri(5), Sat(6), Sun(7), etc(0)
 *  Build EVENTCODE : it's just something by bosch. if you question about it, ask to Bosch(shelah.kim).
 */
#define TMAP_VERSION_YEAR			"20"
#define TMAP_VERSION_WEEK			"13"
#define TMAP_VERSION_DAY			  "7"
#define TMAP_VERSION_EVENTCODE  	"v3"

#ifndef TMAP_BUILD_ID
#define TMAP_BUILD_ID               "0000001"
#endif

#endif // TMAPCONFIG_H
