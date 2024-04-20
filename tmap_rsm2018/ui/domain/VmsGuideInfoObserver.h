#ifndef VMSGUIDEINFOOBSERVER_H
#define VMSGUIDEINFOOBSERVER_H

#include "rprequest.h"

/*! VmsGuideInfoObserver
 * This class is to request VMS traffic information from NDDS and notify as a Toast.
 * This job has some restrict like following.
 * 1. Do not request for about the same ID until previous's one has done.
 * 2. Do not request a query while still doing.(The first rule is apart of this)
 */
namespace SKT {

T_SHAREABLE(VmsGuideInfoObserver);

class VmsGuideInfoObserver :
		public QObject
{
	Q_OBJECT
public:
    VmsGuideInfoObserver(int vmsId);
    ~VmsGuideInfoObserver();
    static void doDeleteLater(VmsGuideInfoObserver* it);

    inline static bool isPending() { return (pendingId != -1 || (queryElapsedTime.msecsTo(QTime::currentTime()) <= 10000));}
    int requestGuideInfo();

    protected slots:
	void onResponsed();


private:
    int mVmsId;
    RPRequestPtr rpRequest;
    static QTime queryElapsedTime;
    static int pendingId;
};

}
#endif // VMSGUIDEINFOOBSERVER_H
