#include "CswMediator.h"
#include "Csw.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "../../common/hardkey.h"
#include "PlatformHelper.h"

#define DO_NOT_APPLY_CSW_HISTORY // CSW history기능을 아직 적용하지 않음

namespace SKT
{

CSWMediator::CSWMediator(QObject *viewComponent)
    : AbstractMediator(viewComponent)
{
#ifndef DISABLE_CSW_IN_NAVI
    connect(&mCswManager, &CswManager::forceLostFocusTriggered, this, [&](){
        mCswGoBackPressed = false;
    });
#endif
}

CSWMediator::~CSWMediator()
{
}

QList<int> CSWMediator::listNotificationInterests()
{
#ifndef DISABLE_CSW_IN_NAVI
    static QList<int> noties = {
        NavigationControllerConstants::ApplyCSW,
        NavigationControllerConstants::ClearCSW,
        NavigationControllerConstants::CollectCSW,
        NavigationControllerConstants::RegistCSW,
        NavigationControllerConstants::RemoveCSW,
        NavigationControllerConstants::SelectCSW,
        NavigationControllerConstants::SelectCSWByObjectName,
        NavigationControllerConstants::ResetCSW,
        NavigationControllerConstants::SelectDefaultSoftkeyCSW,
        ZoneConstants::UpdateModal,
        KeyboardConstants::ShowKeyboard,
        KeyboardConstants::HideKeyboard
    };
    return noties;
#else
    return QList<int> {};
#endif
}

bool CSWMediator::handleNotification(const mvc::INotificationPtr &noti)
{
#ifndef DISABLE_CSW_IN_NAVI
    switch (noti->getId()) {
    case NavigationControllerConstants::ApplyCSW:
        procCSW(noti->getBody().toInt());
        sendDebugInfo();
        break;

    case NavigationControllerConstants::ClearCSW:
        mCswManager.clear();
        sendDebugInfo();
        break;

    case NavigationControllerConstants::CollectCSW:
        {
            auto *viewComponent = noti->getBody().value<QObject*>();
            mCswManager.collect(viewComponent);
            QString viewName = noti->getSource()->metaObject()->className();
            selectCswFromHistory(viewName);
            sendDebugInfo();
        }
        break;

    case NavigationControllerConstants::RegistCSW: {

            for (auto v : noti->getBody().toList()) {
                mCswManager.insert(v.value<Csw*>());
            }
            QString viewName = noti->getSource()->metaObject()->className();
            selectCswFromHistory(viewName);
            sendDebugInfo();
        }
        break;

    case NavigationControllerConstants::RemoveCSW:
        {
            auto m = noti->getBody().toMap();
            if (m.contains(CommonConstants::Section)) {
                auto section = m[CommonConstants::Section].toInt();
                if (section == Csw::Section_Softkey) {
                    mCswManager.removeSoftekyMenu();
                } else {
                    mCswManager.removeSection(section);
                }
            } else {
                if (m.contains("csw")) {
                    auto csw = m["csw"].value<Csw*>();
                    if (csw) {
                        mCswManager.remove(csw);
                    }
                } else {
                    if (m.contains("csws")) {
                        auto vlist = m["csws"].toList();
                        for (auto v : vlist) {
                            auto csw = v.value<Csw*>();
                            if (csw) {
                                mCswManager.remove(csw);
                            }
                        }
                    }
                }
            }
            sendDebugInfo();
        }
        break;

    case NavigationControllerConstants::SelectCSW:
        mCswManager.setCurrent(noti->getBody().value<Csw*>());
        sendDebugInfo();
        break;

    case NavigationControllerConstants::SelectCSWByObjectName:
        mCswManager.setCurrent(noti->getBody().toString());
        break;

    case NavigationControllerConstants::SelectDefaultSoftkeyCSW:
        mCswManager.selectDefaultSoftkeyMenu();
        break;

    case NavigationControllerConstants::ResetCSW:
        {
            auto m = noti->getBody().toMap();
            int section = -1;
            if (m.contains(CommonConstants::Section))
                section = m[CommonConstants::Section].toInt();

            int sequence = -1;
            if (m.contains(QStringLiteral("sequence")))
                sequence = m[QStringLiteral("sequence")].toInt();

            mCswManager.reset(section, sequence);

            bool isClear = true;
            if (m.contains(QStringLiteral("clear"))) {
                isClear = m[QStringLiteral("clear")].toBool();
            }

            if (isClear) {
                mCswManager.clear();
            }

            sendDebugInfo();
        }
        break;

    case KeyboardConstants::ShowKeyboard:
        {
            auto m = noti->getBody().toMap();
            if (m.contains("create")) {
                // 단순 KeyboardViewMediator를 생성하기 위한 용도
            } else {
                mCswManager.blockSection(Csw::Section_Softkey);
                mCswManager.reset(-1, -1);
                sendDebugInfo();
                mBlockSoftkey = true;
            }
        }
        break;

    case KeyboardConstants::HideKeyboard:
        mCswManager.blockSection(Csw::Section_Softkey, false);
        mCswManager.reset(-1, -1);
        sendDebugInfo();
        mBlockSoftkey = false;
        break;

    default:
        break;
    }
#endif

    return true;
}

void CSWMediator::procCSW(int key)
{
#ifndef DISABLE_CSW_IN_NAVI
    qDebug() << "CSWMediator::procCSW(" << key << ") : blockSoftkey ? " << mBlockSoftkey;

    if (key == HardKey::ItcJogDialUp) {
        if (false == mCswManager.movePrevSection()) {
            qDebug() << "Previous section key forwarding...";
            PlatformHelper::instance()->keyForwarding(key, HardKey::AcceptKeyState);
            sendNotification(NavigationControllerConstants::AppLostCswFocus);
        }
    } else if (key == HardKey::ItcJogDialDown) {
        if (false == mCswManager.moveNextSection()) {
            qDebug() << "Next section key forwarding...";
            PlatformHelper::instance()->keyForwarding(key, HardKey::AcceptKeyState);
            sendNotification(NavigationControllerConstants::AppLostCswFocus);
        }
    } else if (key == HardKey::ItcJogDialLeft) {
        mCswManager.moveLeft();
    } else if (key == HardKey::ItcJogDialRight) {
        mCswManager.moveRight();
    } else if (key == HardKey::ItcJogDialSpinCcw) {
        mCswManager.spinCcw();
    } else if (key == HardKey::ItcJogDialSpinCw) {
        mCswManager.spinCw();
    } else if (key == HardKey::ItcJogDialSelect) {
#ifndef DO_NOT_APPLY_CSW_HISTORY
        auto csw = mCswManager.currentCsw();
        if (csw) {
            auto section = csw->section();
            auto sequence = csw->sequence();
            auto name = csw->objectName();
            auto extra = csw->extra();
            if (0 <= section) {
                mHistoryManager.insert(mViewName, CswHistory{section, sequence, extra, name});
            }
        }
#endif
        mCswManager.select();
    } else if (key == HardKey::ItcJogDialBack) {
        mCswGoBackPressed = true;
        mHistoryManager.erase(mViewName);
        if (!mBlockSoftkey) {
            sendNotification(ZoneConstants::CswBackTriggered);
        }
    } else if (key == HardKey::ItcJogDialOption) {
        if (!mBlockSoftkey) {
            sendNotification(ZoneConstants::CswOptionTriggered);
        }
    }
#endif
}

void CSWMediator::sendDebugInfo()
{
#ifndef DISABLE_CSW_IN_NAVI
    QVariantMap m;
    m[CommonConstants::Caller] = mViewName;
    mCswManager.updateDebugInfo(m);
    sendNotification(NavigationControllerConstants::UpdateCswDebugInfo, m);
#endif
}

void CSWMediator::selectCswFromHistory(const QString &viewName)
{
#ifndef DISABLE_CSW_IN_NAVI
    if (!viewName.contains("Zone3ViewMediator")) {
        auto selected = false;
        mViewName = viewName;

        CswHistory history = mHistoryManager.value(viewName);
        if (0 <= history.section) {
            mCswManager.selectBySequence(history.section, history.sequence, history.extra);
            selected = true;
        } else if (!history.objectName.isEmpty()) {
            mCswManager.selectByObjectName(history.objectName, history.extra);
            selected = true;
        }

        if (selected) {
            mHistoryManager.erase(viewName);
        }
    }
#endif
}

}//SKT
