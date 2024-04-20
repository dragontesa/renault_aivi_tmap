#ifndef POIHELPER_H
#define POIHELPER_H

namespace SKT
{

/*
 * ( artf257712 )
 *
 * 원도에서 전달되는 필드 중 [POI_ID]와 [NAV_SEQ] 필드 두개를 합쳐 검색에서 유니크한 값으로
 * 사용하고 있습니다.
 * Server POI ID가 원도의 [POI_ID]로만 사용된다면 검색 결과를 사용할 때 seq 값을 제외하고
 * 사용하시면 됩니다.
 *
 * 예로
 * "88공원"이란 POI가 있다면 원도상 POI_ID는 133, NAV_SEQ는 00 이므로
 * Local POIID는 13300 입니다.
 *
 * App에서 사용 시 local 과 같은 형식의 id를 사용하고
 * Search API 사용 시 서버의 경우 100을 나눈 값을 사용하도록 한다. 로컬은 그대로 사용.
 *
 * POI를 읽거나 쓰는 곳에서는 PoiHelper에서 제공하는 편의 함수들을 이용하도록 함.
 */
class PoiHelper
{
public:
#if (1)
    static int fromLocal(int id) {
        return id;
    }
    static int fromServer(int id, int navSeq) {
        return id * 100 + navSeq;
    }
    static int toLocal(int id) {
        return id;
    }
    static int toServer(int id) {
        return id / 100;
    }
    static int navSeq(int id) {
        return id % 100;
    }
#else
    static int fromLocal(int id) {
        return id / 100;
    }
    static int fromServer(int id) {
        return id;
    }
    static int toLocal(int id, int navseq) {
        return id * 100 + navseq;
    }
    static int toServer(int id) {
        return id;
    }
#endif
    static QString navSeqFromLocal(int id) {
        return QString::number(id % 100);
    }
    static QString navSeqFromServer(const char *szNavSeq) {
        return QString(szNavSeq);
    }
};

}//SKT

#endif // POIHELPER_H
