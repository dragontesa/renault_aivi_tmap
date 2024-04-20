#include "AddressMediator.h"
#include <QVariantMap>
#include "Constants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "NavibookmarkConstants.h"
#include "ToastsConstants.h"
#include "AddressConstants.h"
#include "NavibookmarkProtocol.h"
#include "ViewRegistry.h"


namespace SKT
{

AddressMediator::AddressMediator(QObject *addressManager)
{
    mAddressManager = addressManager;
    connect(mAddressManager, SIGNAL(replied(int,int,QString)), this, SLOT(reply(int,int,QString)));
}

AddressMediator::~AddressMediator()
{

}

QList<int> AddressMediator::listNotificationInterests()
{
    return QList<int> {
        AddressConstants::RequestAddress,
        SearchConstants::AddItemFailure,
        SearchConstants::AddItemOk
    };
}

bool AddressMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case AddressConstants::RequestAddress:
        request(note->getBody().toMap());
        break;


    case SearchConstants::AddItemOk:
        onAddItemOk(note->getBody().toMap());
        break;

    case SearchConstants::AddItemFailure:
        onAddItemFailure(note->getBody().toMap());
        break;

    default:
        break;
    }

    return true;
}

void AddressMediator::request(const QVariantMap &req)
{
    auto x = req[CommonConstants::X].toInt();
    auto y = req[CommonConstants::Y].toInt();

    mRequested.append({ QPoint(x, y), req });

    QMetaObject::invokeMethod(mAddressManager, "request", Q_ARG(int, x), Q_ARG(int, y));
}

void AddressMediator::replyToFavorite(int x, int y, const QString& name, const QString &address, int destType)
{
    QString address2;
    if (name.isEmpty() && address.isEmpty()) {
        address2 = getAddressFromRequestedCommand(x, y);
    }

    QVariantMap m;
    m.insert(CommonConstants::Type,(int)NavibookmarkProtocol::TransactionMap); // transaction type
    sendNotification(NavibookmarkConstants::NewNavibookSession, m);

    m.clear();
    m[CommonConstants::Name] = name;
    m[CommonConstants::Address] = address.length() ? address : address2;
    m[CommonConstants::X] = x;
    m[CommonConstants::Y] = y;
    m[SearchConstants::DestType] = destType;

    sendNotification(NavibookmarkConstants::SelectNavibookAddress, m);

    m.clear();
    m[CommonConstants::BookmarkType] = NavibookmarkProtocol::BookmarkFavorite;
    sendNotification(NavibookmarkConstants::RegistNavibookBookmark, m);
}

void AddressMediator::onAddItemOk(const QVariantMap &m)
{
    auto itemType = m["itemType"].toString();
    QString caller;

    auto x = m[QStringLiteral("x")].toInt();
    auto y = m[QStringLiteral("y")].toInt();
    auto item = requestedItem(QPoint(x, y));
    if (!item.pt.isNull()) {
        caller = item.req[CommonConstants::Caller].toString();
    }
    auto owner = item.req[CommonConstants::Owner].toString();

    if (itemType == QStringLiteral("favorite")) {
        // 주행 중 맵을 터치하여 즐겨 찾기 추가 시 caller는 MapControlView
        // 즐겨 찾기 추가에서 지도로 추가 시 caller는 MapSearchView
        // 네비메뉴에서 집, 회사로 즐겨찾기 추가되는 경우에는 owner가 NaviMenuView

        if (owner == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
            QVariantMap m2;
            m2[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::NaviMenuView);
            sendNotification(Constants::CreateView, m2);
        } else if (caller == ViewRegistry::objectName(ViewRegistry::MapSearchView)) {
            QVariantMap m2;

            // TMCRSM-377. 지도에서 즐겨찾기 추가 시 다시 지도화면으로 되돌아 오도록 수정. 18.2.7
            // TMCRSM-459. 즐겨찾기 > 즐겨찾기 추가 > 지도 검색 > 지도 검색 등록 후 즐겨찾기 화면으로 이동 안되는 현상. 18.2.21

            m2[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchTabView);
            m2[SearchConstants::Page] = 2;
            sendNotification(Constants::CreateView, m2);
        }

        if (caller == ViewRegistry::objectName(ViewRegistry::MapSearchView)) {
            // TMCRSM-792.
            // 지도 검색 화면 닫는 시점을 이쪽으로 옮김.
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, caller}});
        }
        removeRequestedItem(QPoint(x, y));
    }
}

void AddressMediator::onAddItemFailure(const QVariantMap &m)
{
    auto itemType = m["itemType"].toString();

    if (itemType == QStringLiteral("favorite")) {
        auto x = m[QStringLiteral("x")].toInt();
        auto y = m[QStringLiteral("y")].toInt();
        removeRequestedItem(QPoint(x, y));
    }
}

void AddressMediator::removeRequestedItem(const QPoint &pt)
{
    for (auto iter = mRequested.begin(); iter != mRequested.end(); ++iter) {
        if (iter->pt == pt) {
            mRequested.erase(iter);
            break;
        }
    }
}

AddressMediator::Item AddressMediator::requestedItem(const QPoint &pt) const
{
    for (auto item : mRequested) {
        if (item.pt == pt) {
            return item;
        }
    }
    return Item {};
}

QString AddressMediator::getAddressFromRequestedCommand(int x, int y) const
{
    for (auto item : mRequested) {
        if (item.pt == QPoint(x, y)) {
            auto address = item.req[CommonConstants::Address].toString();
            return address;
        }
    }
    return QString{};
}

void AddressMediator::reply(int x, int y, const QString &address)
{
    auto responseAddress = false;

    for (auto iter = mRequested.begin(); iter != mRequested.end(); ++iter) {
        if (iter->pt == QPoint(x, y)) {
            auto req = iter->req;

            auto type = req[CommonConstants::Type].toString();
            if (type.contains(CommonConstants::Favorite)) {
                int destType = 0;
                if (type.contains("/home"))
                    destType = 2;
                else if (type.contains("/office"))
                    destType = 1;
                else if (type.contains("/poi"))
                    destType = 3;

                auto name = req[SearchConstants::Dest].toString();
                if (name.isEmpty())
                    name = address;

                replyToFavorite(x, y, name, address, destType);
            } else {
                mRequested.erase(iter);
                responseAddress = true;
            }
            break;
        }
    }
    if (responseAddress) {
        sendNotification(AddressConstants::ResponseAddress, address);
    }
}

}//SKT
