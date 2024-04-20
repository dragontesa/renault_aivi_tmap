/*!
 * OfflineJobCacheProxy.h
 *
 */

#ifndef UI_DOMAIN_OFFLINEJOBCACHEPROXY_H_
#define UI_DOMAIN_OFFLINEJOBCACHEPROXY_H_

#include "AbstractProxy.h"
#include "SearchCacheDto.h"
#include "PoiCacheDto.h"
#include "PoiExtraCacheDto.h"
#include "searchrequest.h"

namespace SKT
{
T_SHAREABLE(OfflineJobCacheProxy)
typedef QSharedPointer<QList<TPOI_SEARCH_LIST>>  SearchCacheInfoListPtr;
typedef QSharedPointer<QList<PoiCacheInfo>>      PoiCacheInfoListPtr;
typedef QSharedPointer<QList<PoiExtraCacheInfo>> PoiExtraCacheInfoListPtr;

class OfflineJobCacheProxy: public AbstractProxy,
                            public TShareable<OfflineJobCacheProxy>
{
	Q_OBJECT
public:
	OfflineJobCacheProxy();
	virtual ~OfflineJobCacheProxy();

	PoiCacheInfoPtr popPoiCacheInfo(int poiId) const;
	SearchCacheInfoListPtr searchCache(const QString& request) const;
    SearchCacheInfoListPtr searchCache(const QString& request, int startIndex, int limit, int &totalCount, bool isLike=true) const;

private:
	void createDB(const QMetaObject* dto);
    void onRegister();
    bool handleJobNotification( const mvc::INotificationPtr& note );

    bool mIsOnline;
};

} /* namespace SKT */

Q_DECLARE_METATYPE(SKT::SearchCacheInfoListPtr)
Q_DECLARE_METATYPE(SKT::PoiCacheInfoListPtr)
Q_DECLARE_METATYPE(SKT::PoiExtraCacheInfoListPtr)
Q_DECLARE_METATYPE(SKT::PoiCacheInfoPtr)


#endif /* UI_DOMAIN_OFFLINEJOBCACHEPROXY_H_ */
