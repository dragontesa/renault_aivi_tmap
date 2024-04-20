#ifndef BOOKMARKREGISTVIEWMEDIATOR_H
#define BOOKMARKREGISTVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"
#include <qglobal.h>

namespace SKT {

class BookmarkRegistViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS(EventK)

public:
    enum EventK {
        Mediated = 0,
        Activated,
        Deactivated,
        CancelClicked,
        SearchItemClicked,
        ModalChanged,
        CaptureScreen
    };

    Q_INVOKABLE BookmarkRegistViewMediator(QObject *viewComponent = nullptr);
    virtual ~BookmarkRegistViewMediator();

    QList<int> listNotificationInterests() Q_DECL_OVERRIDE;

protected:
    bool onEvent(int event, const QVariant &data) Q_DECL_OVERRIDE;
    bool handleNotification(const mvc::INotificationPtr &note) Q_DECL_OVERRIDE;

private:
    void onFavoriteItemClicked(const QVariant &);
    void parseViewList(const QVariant& v);
    void closeView();
    void closeView(const QString& viewName);
    void closeBookmark(int searchType);

    void updateModal(const QVariant& v);

private:
    QList<int> mViewList;
    bool mPopupCreated = false;
};

} //SKT

#endif // BOOKMARKREGISTVIEWMEDIATOR_H
