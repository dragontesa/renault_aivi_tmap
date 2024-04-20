#include "VmsGuideInfoObserver.h"
#include "RpnSearchHelper.h"
#include "CommonConstants.h"
#include "ToastsConstants.h"
#include "../common/rprequest.h"
#include "../util/StringHelper.h"
#include "RouteSearchProxy.h"

namespace SKT
{
int VmsGuideInfoObserver::pendingId = -1;
QTime VmsGuideInfoObserver::queryElapsedTime = QTime::currentTime();

VmsGuideInfoObserver::VmsGuideInfoObserver(int vmsId) :
		mVmsId(vmsId)
{
    QVariantMap data;
    data.insert( QStringLiteral(RPREQ_KEY_RequestType), RPRequest::RequestVMSInfo );
    data.insert( QStringLiteral(RPREQ_KEY_VMSResID), vmsId);
    data.insert( QStringLiteral(RPREQ_KEY_Online), Facade::instance()->retrieveProxy<RouteSearchProxy>()->isRpServerOnline());

    QVariantMapPtr reqDataPtr(new QVariantMap(data));
    rpRequest.reset(new RPRequest(reqDataPtr));
}

VmsGuideInfoObserver::~VmsGuideInfoObserver()
{
	pendingId = -1;
}

void VmsGuideInfoObserver::doDeleteLater(VmsGuideInfoObserver* it)
{
	if (!isPending())
		it->deleteLater();
	else return;
}

int VmsGuideInfoObserver::requestGuideInfo()
{
	if (pendingId == mVmsId) return 1; // to prevent duplicated query for the same id

	pendingId = mVmsId; // this is the reason that you must checkup with the isPending() before call this.
	queryElapsedTime = QTime::currentTime();

    auto rpnSearch = RpnSearchHelper::instance();
    RPRequestHandle* req = rpnSearch->createRequest(rpRequest);
    connect(req->qobject(), SIGNAL(finished()), this, SLOT(onResponsed()), Qt::QueuedConnection);
    rpnSearch->requestCommand(req);

	return 0;
}

void VmsGuideInfoObserver::onResponsed()
{
    RPRequestHandle* req = qobject_cast<RPRequestHandle*>(sender());
    if (!req) {
        return;
    }

    req->qobject()->deleteLater();

    QVMSGuideInfoPtr res = req->request()->getVMSGuideInfo();

    if (!res.isNull())
	{
		// send toast noti
		QVariantMap body;
        body.insert(CommonConstants::Position,ToastsConstants::ToastUp);
        body.insert(CommonConstants::Duration, 10000); // toast duration 10 secs
		QString msg;

        if (res->szContent[0] != 0)
        {
            msg = res->szContent;

            body.insert(CommonConstants::Message, msg);

    //	    if (Facade::instance()->hasCommand(ToastsConstants::ShowToast))  // this can be skipped because of the command must be always.
            {
                mvc::INotificationPtr note = t_new_shared<Notification>(this, ToastsConstants::ShowToast, body);
    //			note->setSource(QObjectPtr(this)); // to prevent using TNotification's deleter built in
                Facade::instance()->notifyObservers(note);
            }
        }
#ifdef FOR_TEST
        else
        {
            msg = QStringLiteral("10km 구간 교통상황 원활 -demo");

            body.insert(CommonConstants::Message, msg);

    //	    if (Facade::instance()->hasCommand(ToastsConstants::ShowToast))  // this can be skipped because of the command must be always.
            {
                mvc::INotificationPtr note = t_new_shared<Notification>(this, ToastsConstants::ShowToast, body);
    //			note->setSource(QObjectPtr(this)); // to prevent using TNotification's deleter built in
                Facade::instance()->notifyObservers(note);
            }
        }
#endif
	}

    rpRequest.clear();
	pendingId = -1; // free pending status

	deleteLater();
}
}
