#ifndef NAVIBOOKMARKAGENTMEDIATOR_H
#define NAVIBOOKMARKAGENTMEDIATOR_H

#include "mediator/AbstractMediator.h"
#include "NavibookmarkProtocol.h"

namespace SKT {

struct NavibookBookmark
{
	QString name;
	QString address;
	int     mapX;
	int     mapY;
	int     destType; // 0: unknown, 1: office, 2: home
	int     poiID;
    int     navSeq;
    int     rpFlag;
    int     opiPoleCate;
    int     opiPrice;
    QString favInsTime;
};

class NavibookmarkAgentMediator : public AbstractMediator
{
	Q_OBJECT
public:
    NavibookmarkAgentMediator();
    ~NavibookmarkAgentMediator();

    // override
    QList<int> listNotificationInterests();
    bool handleNotification( const mvc::INotificationPtr& note );

    // interface

private:
    void    reset();
    QString newTransactionId(int transactionType);
    bool    registNewBookmark(const QString& caller);
    bool    isValidBookmark();


private:
    QString mTransactionId;
    QString mCallerId;
    NavibookmarkProtocol::NavibookTransactionType mTransactionType;
    NavibookmarkProtocol::NavibookBookmarkType    mBookmarkType;
    NavibookmarkProtocol::NavibookSearchType      mSearchType;
    QString mSearchKeyword;
    NavibookBookmark mBookmark;
};

}

#endif // NAVIBOOKMARKAGENTMEDIATOR_H
