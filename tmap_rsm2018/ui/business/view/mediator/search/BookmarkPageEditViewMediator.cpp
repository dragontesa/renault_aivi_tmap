#include "BookmarkPageEditViewMediator.h"

#include "ApplicationConstants.h"
#include "FavoriteDto.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "PlatformHelper.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "ViewRegistry.h"
#include "ZoneConstants.h"

#define MAX_EDIT_FAVORITE_TEXT  48  // 50 - 즐겨 찾기 명칭 버퍼 크기는 char[100].
                                    // 한글 50자로 하면 crash남.
                                    // 모바일 티맵과 동일하게 맞춤.
                                    // 19.10.31. ymhong

namespace SKT
{

BookmarkPageEditViewMediator::BookmarkPageEditViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{
    mKeyboardProxy.setMaxStringLength(MAX_EDIT_FAVORITE_TEXT);
}

void BookmarkPageEditViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

void BookmarkPageEditViewMediator::onAccepted(const QVariant &v)
{
    auto m = v.toMap();
    auto title = m["title"].toString();
    if (title == tr("전체삭제")) {
        // BookmarkPageViewMediator에서 실제 처리
        sendNotification(SearchConstants::InvokeClearFavorite);
        close();
    }
}

void BookmarkPageEditViewMediator::onCanceled(const QVariant &v)
{
    auto m = v.toMap();
    auto title = m["title"].toString();
    if (title == tr("전체삭제")) {
        close();

        // TMCRSM-3110
        // 전체 삭제 취소 시 편집 화면 유지 하도록 해야 함
        // 아래 nofification을 전송하면 편집 화면 종료
        // 19.05.08 ymhong.

        //sendNotification(SearchConstants::InvokeUncheckAllFavorite);
    }
}

void BookmarkPageEditViewMediator::showDeleteAllPopup()
{
    QVariantMap m;
    m["title"] = tr("전체삭제");
    m["message"] = tr("즐겨찾기 리스트를\n모두 삭제하시겠습니까?");
    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
    m[CommonConstants::ParentObjectName] = CommonConstants::ContentRootForCommonDialog;
    m[CommonConstants::Menus] = QVariantList { tr("취소"), tr("확인") };
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

void BookmarkPageEditViewMediator::resetKeyboard()
{
#ifdef BUILD_TARGET
    PlatformHelper::instance()->setSearchBarText("");
#endif
}

QList<int> BookmarkPageEditViewMediator::listNotificationInterests()
{
    return QList<int> {
        KeyboardConstants::ProcessKeyboard,
        KeyboardConstants::SystemKeyboardPressed,
        KeyboardConstants::SystemKeyboardBackPressed,
        KeyboardConstants::SystemKeyboardFinished,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        ZoneConstants::CswBackTriggered,
        ApplicationConstants::SystemPopupCreated,
        ApplicationConstants::SystemPopupClosed,
        NavigationControllerConstants::NavigationDisappeared,
    };
}

bool BookmarkPageEditViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case ShowKeyboard:
        {
            auto m = data.toMap();
            m[CommonConstants::Owner] = ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
            sendNotification(KeyboardConstants::ShowKeyboard, m);


#ifdef BUILD_TARGET
            auto text = m[CommonConstants::Text].toString();

            m.clear();
            m[CommonConstants::Visible] = true;
            m[CommonConstants::Text] = text;
            sendNotification(KeyboardConstants::UpdateKeyboard, m);

            mKeyboardProxy.setString(text);
#endif
        }
        break;

    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;

    case EditFavoriteAlias:
        sendNotification(SearchConstants::InvokeEditFavoriteAlias, data);
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal, data.toMap());
        break;

    case DeleteAllFavoriteItems:
        // BookmarkPageViewMediator에서 실제 처리
        sendNotification(SearchConstants::InvokeClearFavorite);
        break;

    case ShowDeleteAllPopup:
        showDeleteAllPopup();
        break;

    case UncheckAllFavorite:
        sendNotification(SearchConstants::InvokeUncheckAllFavorite);
        break;

    case AboutToClose:
        sendNotification(ZoneConstants::SelectZone1TextInput, QVariantMap{{CommonConstants::Text,CommonConstants::Empty}});
        resetKeyboard();
        break;

    case ClearText:
        sendNotification(KeyboardConstants::SetText, "");
        mKeyboardProxy.clear();
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    }

    return true;
}

bool BookmarkPageEditViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId())
    {
    case KeyboardConstants::ProcessKeyboard: {
        // Donot follow this notification if the view is hidden(prevent it might be bug), by lckxx 2017.11.14
        if (!(QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()))
            break;

         auto m = note->getBody().toMap();
         // 17.10.25 hskim check owner
         auto owner = m[CommonConstants::Owner].toString();
         auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
         auto bookmarkPageView = ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
         if (owner != objectName && owner != bookmarkPageView)
             return false;
         auto key = m[CommonConstants::Key];
         auto text = m[CommonConstants::Text].toString();
         if (text.length() > MAX_EDIT_FAVORITE_TEXT) {
             text = text.left(MAX_EDIT_FAVORITE_TEXT);
#ifdef BUILD_TARGET
             PlatformHelper::instance()->setSearchBarText(text);
#endif
         } else {
#ifdef QT_DEBUG
             QMetaObject::invokeMethod(mViewComponent, "handleKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
#endif
         }
     }
     break;
#ifdef BUILD_TARGET
    case KeyboardConstants::SystemKeyboardPressed:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toString();
            mKeyboardProxy.keyPressed(key);
            auto text = mKeyboardProxy.string();
            const int textMaxLength = MAX_EDIT_FAVORITE_TEXT;
            if (text.length() > textMaxLength) {
                text = text.left(textMaxLength);
            }
            QMetaObject::invokeMethod(mViewComponent, "handleKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
        }
        break;
    case KeyboardConstants::SystemKeyboardBackPressed:
    case KeyboardConstants::SystemKeyboardFinished:
        {
            auto m = note->getBody().toMap();
            auto key = m["key"].toInt();
            auto text = m["text"].toString();
            QMetaObject::invokeMethod(mViewComponent, "handleKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
        }
        break;
#endif
    case KeyboardConstants::PullDownKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
        break;
    case KeyboardConstants::PullUpKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
        break;

    case SettingConstants::DialogAccepted:
        onAccepted(note->getBody());
        break;

    case SettingConstants::DialogCanceled:
        onCanceled(note->getBody());
        break;

    case ZoneConstants::CswBackTriggered:
        {
            auto activated = QQmlProperty::read(mViewComponent, "active").toBool();
            if (activated) {
                auto state = note->getBody().toInt();
                auto pressed = state == 1;
                if (pressed) {

                } else {
                    QMetaObject::invokeMethod(mViewComponent, "goBack");
                }
                sendNotification(ZoneConstants::CswBackAccepted);
            }
        }
        break;

    case ApplicationConstants::SystemPopupCreated:
        QMetaObject::invokeMethod(mViewComponent, "pause");
        break;

    case ApplicationConstants::SystemPopupClosed:
        QMetaObject::invokeMethod(mViewComponent, "resume");
        break;

    case NavigationControllerConstants::NavigationDisappeared:
        // TMCRSM-4642. 키보드 언어 변경 시 즐겨 찾기 명칭 편집 창 사라짐.
        // 1) 후방 카메라 동작 시 -> 내비 화면 복귀 시 키보드 표출 되며 뒤로 가기도 정상 동작
        // 2) 키보드 언어 변경 시 -> 내비 화면 복귀 시 키보드 표출 되며 편집창도 정상 표출
        // 20.03.09 ymhong.

        //close(); // 예전 후방 카메라 동작 시 키보드가 사라져서 뒤로 가기를 할 수 없어 편집 화면을 닫아서 대응한 코드.
        break;

    default:
        break;
    }

    return true;
}

}//SKT
