#include "KeyboardViewMediator.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "PreferenceConstants.h"
#include "PreferenceVo.h"
#include "CommonConstants.h"
#include "PlatformHelper.h"
#include "ViewRegistry.h"

using namespace SKT;

#define DONT_VISIBLE_PROPERTY    "dontVisible"

KeyboardViewMediator::KeyboardViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
      mOwner(CommonConstants::Empty)
{
#ifdef BUILD_TARGET
    // 타겟에서는 SW키보드가 노출 되지 않도록 한다.
    // 즐겨찾기 편집에서 (x)버튼을 눌러 명칭을 삭제 후
    // 취소, 확인 누르면 망가진 화면이 나옴.
    QQmlProperty::write(mViewComponent, DONT_VISIBLE_PROPERTY, true);
#endif
}

QList<int> KeyboardViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        KeyboardConstants::UpdateKeyboard,
        KeyboardConstants::ProcessSystemKeyboard,
        KeyboardConstants::SetText,
        KeyboardConstants::ShowKeyboardForMonkeyTest,
        KeyboardConstants::NotifyKeyboardOwner,
        PreferenceConstants::ApplyPreference,
        PreferenceConstants::ChangedPreference
    };

    return notes;
}

bool KeyboardViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        loadSwKeyboardIfNeeded();
        break;

    case Activated: {
#ifdef BUILD_TARGET
        qDebug() << "KeyboardViewMediator::Activated";
        qDebug() << QQmlProperty::read(mViewComponent, "owner").toString();
#endif
        break;
    }
    case ProcessKeyboard:
        if (mSwKeyboardLoad) {
            qDebug() << "############################################################";
            processKeyboard(data);
            qDebug() << "############################################################";
        }
        break;

#ifdef QT_DEBUG
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case RemoveCsw:
        sendNotification(NavigationControllerConstants::RemoveCSW, data);
        break;
#endif

    default:
        return false;
    }
    return true;
}

bool KeyboardViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {    
    case KeyboardConstants::UpdateKeyboard: {
            auto args = note->getBody().value<QVariantMap>();
            for(QVariantMap::const_iterator it = args.begin(); it != args.end(); ++it) {
                if (CommonConstants::Url == it.key() || CommonConstants::ObjectName == it.key())
                    continue;

                if (CommonConstants::Text == it.key() || CommonConstants::Owner == it.key()) {
                    if (!args[CommonConstants::Visible].toBool()) {
                        if (mOwner != args[CommonConstants::Owner].toString())
                            continue;
                    }
                }
                QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            auto forceRegistOwner = false;
#ifdef BUILD_TARGET
            // 타겟에서는 키보드뷰를 visible 시키지 않기 때문에
            // Owner등록을 강제로 하도록 한다.
            forceRegistOwner = true;
            if (args.contains("show") && args["show"].toBool() == false) {
                args[CommonConstants::Owner] = "";
            }

            auto visible = args["visible"].toBool();
            if (visible && args.contains(CommonConstants::Text)) {
                auto text = args["text"].toString();
                PlatformHelper::instance()->setSearchBarText(text);
            }
#endif
            if (args[CommonConstants::Visible].toBool() || forceRegistOwner) {
                if (args.contains(CommonConstants::Owner))
                    mOwner = args[CommonConstants::Owner].toString();
                sendNotification(KeyboardConstants::NotifyKeyboardOwner, QVariant::fromValue(args[CommonConstants::Owner]));
            }else {
                sendNotification(KeyboardConstants::NotifyKeyboardOwner, QVariant::fromValue(CommonConstants::Empty));
            }
        }
        break;

    case KeyboardConstants::ProcessSystemKeyboard:
        processKeyboard(note->getBody());
        break;

    case KeyboardConstants::SetText:
        {
            auto text = note->getBody().toString();
#ifdef BUILD_TARGET
            PlatformHelper::instance()->setSearchBarText(text);
#else
            QQmlProperty::write(mViewComponent, "text", text);
#endif
        }
        break;

    case KeyboardConstants::ShowKeyboardForMonkeyTest:
#ifdef BUILD_TARGET
        if (mSwKeyboardLoad) {
            sendNotification(KeyboardConstants::UpdateKeyboard, note->getBody());
        }
#endif
        break;

    case KeyboardConstants::NotifyKeyboardOwner:
        {
            QString source = note->getSource()->metaObject()->className();

            // KeyboardViewMediator를 제외한 뷰에서 보내는 경우 mOwner만 변경한다.
            if (!source.contains(ViewRegistry::objectName(ViewRegistry::KeyboardView))) {
                mOwner = note->getBody().toString();
            }
        }
        break;

    case PreferenceConstants::ApplyPreference:
        applyPreference(note->getBody());
        break;
    case PreferenceConstants::ChangedPreference:
        changedPreference(note->getBody());
        break;

    default:
        return false;
    }
    return true;
}

// Ubuntu 일때 소프트키보드 활성화
// 타겟일 때는 몽키 테스트가 활성화 되었을 때 로드한다.
void KeyboardViewMediator::loadSwKeyboardIfNeeded()
{
    auto isLoadSwKeyboard = false;
#ifdef QT_DEBUG
    isLoadSwKeyboard = true;
#endif
    if (isLoadSwKeyboard) {
        loadSwKeyboard();
    } else {
        sendNotification( PreferenceConstants::RequestPreference,
            QVariantMap{
                { CommonConstants::Caller, ViewRegistry::objectName(ViewRegistry::KeyboardView) },
                { QStringLiteral("preferencesName"), PreferenceConstants::Test },
                { CommonConstants::Section, QStringLiteral("MonkeyTest") }
            } );
    }
}

void KeyboardViewMediator::processKeyboard(const QVariant &v)
{
    auto m = v.toMap();

#ifdef BUILD_TARGET
    auto key = m["key"].toInt();
    if (key == -3) {
        qDebug() << "[@--@] Back key owner --> " << mOwner;
    }

    auto text = m["text"].toString();
    qDebug() << "processKeyboard(key:" << key << ", string:" << text << ", owner: " << mOwner << ")";
    m["owner"] = mOwner;
#else
    auto owner = QQmlProperty::read(mViewComponent, "owner").toString();
    m["owner"] = owner;
#endif

    sendNotification(KeyboardConstants::ProcessKeyboard, m);
}

void KeyboardViewMediator::applyPreference(const QVariant &v)
{
    auto result = v.value<QVariantMap>();
    auto req = result[QStringLiteral("request")].value<QVariantMap>();
    auto caller = req[CommonConstants::Caller].toString();
    if (caller == ViewRegistry::objectName(ViewRegistry::KeyboardView)) {
        if ( req[QStringLiteral("preferencesName")].toString() == PreferenceConstants::Test ) {
            if ( req[CommonConstants::Section] == QStringLiteral("MonkeyTest") ) {
                auto resp = result[QStringLiteral("response")].value<QMap<QString,PreferenceVo>>();
                auto enableMonkeyTest = resp["enable"].value().toBool();
                if (enableMonkeyTest) {
                    loadSwKeyboard();
                }
            }
        }
    }
}

void KeyboardViewMediator::changedPreference(const QVariant &v)
{
    auto result = v.value<QVariantMap>();
    if ( result[QStringLiteral("preferencesName")].toString() == PreferenceConstants::Test ) {
        if ( result[CommonConstants::Section] == QStringLiteral("MonkeyTest") ) {
            auto enableMonkeyTest = result[CommonConstants::Value].toBool();
            if (enableMonkeyTest) {
                loadSwKeyboard();
            }
        }
    }
}

void KeyboardViewMediator::loadSwKeyboard()
{
    if ( !mSwKeyboardLoad ) {
        // 타겟이어도 몽키 테스트가 활성화 되어 있으면 소프트키보드 표시를 위해 처리.
        QQmlProperty::write(mViewComponent, DONT_VISIBLE_PROPERTY, false);

        QMetaObject::invokeMethod(mViewComponent, "loadSwKeyboard");
        mSwKeyboardLoad = true;
    }
}
