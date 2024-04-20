#include <QEvent>
#include <QKeyEvent>
#include <QDebug>

#include "debugkeyfilter.h"
#include "tmapcontroller.h"
#include "packagemanager.h"
#include "environment.h"
#include "gpsmanagerinterface.h"
#include "sessionmanagerinterface.h"
#include "VSM_Map.h"
#include <QDebug>
#include "ui/util/PlatformHelper.h"

namespace SKT {

DebugKeyFilter::DebugKeyFilter(TmapController* controller, QObject* parent)
    : QObject(parent)
    , m_controller(controller)
    , m_enableShift(false)
    , m_enableControl(false)
    , m_usbMount(false)
{

}

bool DebugKeyFilter::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched)

#ifdef QT_DEBUG
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

        m_enableControl = keyEvent->modifiers() & Qt::ControlModifier;
        m_enableShift = keyEvent->modifiers() & Qt::ShiftModifier;

        switch (keyEvent->key()) {
        case Qt::Key_Comma: {
            m_controller->gpsManager()->sendExtraCommand("rewind");
            return true;
        }
        case Qt::Key_Period: {
            m_controller->gpsManager()->sendExtraCommand("fast-forward");
            return true;
        }
        case Qt::Key_B: {
            m_controller->setAppForeground(!m_controller->appForeground());
            return true;
        }
        case Qt::Key_C: {
            QMetaObject::invokeMethod(m_controller, "showDestinationFreeTextSearchInputOccur", Q_ARG(QString, "뚀뽷ㄹ고놫ㄷ"));
            return true;
        }
        case Qt::Key_P:
        case Qt::Key_R:
        case Qt::Key_D:
        case Qt::Key_S:
            m_controller->debugKeyPressed(keyEvent->key());
            return true;

        case Qt::Key_T: {
            if(m_enableControl){
                char isShow = true;
                VSMMap_GetShowTrafficInfoOnMap(VSM_NULL, isShow);
                isShow = isShow == 0 ? 1 : 0;
                m_controller->setTrafficInfoScreen(isShow == 1);
            }
            else {
                m_controller->setNetworkOnline(!m_controller->networkOnline());
            }
            return true;
        }
        case Qt::Key_U: {
            if (!m_usbMount) {
                m_usbMount = true;
                m_controller->m_packageManager->onMediaMounted(Environment::getMapDataPath() + "/../usb");
            } else {
                m_usbMount = false;
                m_controller->m_packageManager->onMediaUnmounted();
            }
            return true;
        }
        case Qt::Key_O: {
            if(m_enableControl){
                ViewLevelInfo level = m_controller->naviProperty()->getTmapViewLevel();
                int lv = level.mLevel;
                lv = --lv < 0 ? 0 : lv;
                m_controller->setViewLevel(lv,0);
            }
            else{
                auto online = m_controller->online();
                // 실패없는 온라인/오프라인 전환(우분투용)
                m_controller->setOnline(!online);
            }
            return true;
        }
        case Qt::Key_L:
            m_isKorean = !m_isKorean;
            m_controller->setLanguageId(m_isKorean ? 31 : 14);
            return true;

        case Qt::Key_M:
            QMetaObject::invokeMethod(m_controller, "showMonkeyTest");
            return true;

        case Qt::Key_N: {
            NavigationController::SunState s = m_controller->sunState();
            switch (s) {
            case NavigationController::SunDawn:
                s = NavigationController::SunDay;
                break;
            case NavigationController::SunDay:
                s = NavigationController::SunTwilight;
                break;
            case NavigationController::SunTwilight:
                s = NavigationController::SunNight;
                break;
            case NavigationController::SunNight:
                s = NavigationController::SunDawn;
                break;
            default:
                s = NavigationController::SunDay;
                break;
            }
            m_controller->setSunState(s);
            return true;
        }
        case Qt::Key_W: {
            NavigationController::WeatherState w = m_controller->weatherState();
            switch (w) {
            case NavigationController::WeatherSunny:
                w = NavigationController::WeatherRainy;
                break;
            case NavigationController::WeatherRainy:
                w = NavigationController::WeatherSnowy;
                break;
            case NavigationController::WeatherSnowy:
                w = NavigationController::WeatherSunny;
                break;
            default:
                w = NavigationController::WeatherSunny;
                break;
            }
            m_controller->setWeatherState(w);
            return true;
        }
        case Qt::Key_I: {
            if(m_enableControl){
                ViewLevelInfo level = m_controller->naviProperty()->getTmapViewLevel();
                int lv = level.mLevel;
                lv = ++lv > 12 ? 12 : lv;
                m_controller->setViewLevel(lv,0);
            }
            return true;
        }
        case Qt::Key_V: {
            if(m_enableControl){
                int mode = m_controller->naviProperty()->getTmapViewMode();
                mode = ++mode > 2 ? 0 : mode;
                m_controller->setMapViewMode(mode);
            }
            else{

            }
            return true;
        }
        case Qt::Key_1: {
            if(m_enableControl){
                m_controller->moveToCurrentPosition();
                return true;
            }
        }
        break;
        case Qt::Key_2: {
            if(m_enableControl){
                auto testCase = 9;

                switch (testCase) {
                case 0:
                    //경유지 편집 화면으로 이동
                    m_controller->goViaPointEditScreen();
                    break;
                case 1: {
                    //경유지 설정
                    TS_ROUTE_POS pos;
                    memset(&pos, 0, sizeof(pos));
                    pos.tp.x = 66513375;
                    pos.tp.y = 19670485;
                    m_controller->setWaypoint(pos, 2);
                    break;
                }//case 1
                case 2: {
                    //경유지 추가
                    TS_ROUTE_POS pos;
                    memset(&pos, 0, sizeof(pos));
                    pos.tp.x = 66513375;
                    pos.tp.y = 19670485;
                    m_controller->insertWaypoint(pos, 1);
                    break;
                }//case 2
                case 3:
                    //경유지 삭제
                    m_controller->deleteWaypoints(1, 2);
                    break;
                case 4: {
                    //주어진 경유지 정보로 경로 안내
                    // 테스트
                    // 양천구 신정1동
                    // x : 66513375, y : 19670485
                    // 꼬마대통령 목동점
                    // x : 66517919, y : 19668723
                    // 마포트라팰리스2차
                    // x : 66562236, y : 19686738
                    QList<TS_ROUTE_POS> plist;
                    TS_ROUTE_POS pos;
                    memset(&pos, 0, sizeof(TS_ROUTE_POS));
                    pos.tp = { 66513375, 19670485 };
                    plist.push_back(pos);
                    pos.tp = { 66517919, 19668723 };
                    plist.push_back(pos);
                    pos.tp = { 66562236, 19686738 };
                    plist.push_back(pos);
                    int option = 1;
                    auto apply = false;
                    m_controller->requestRoute(plist, option, apply);
                    break;
                }//case 4
                case 5: {
                    //목적지를 지정하여 경로 안내(집으로, 회사로, ...)
                    int option = 1;
                    auto apply = false;
                    TS_ROUTE_POS pos;
                    pos.tp = { 66513375, 19670485 };
                    m_controller->requestRoute(pos, option, apply);
                    break;
                }//case 5
                case 6: {
                    //해당 인덱스의 경유지를 현재 위치로 설정
                    m_controller->replaceWaypoint(1);
                    break;
                }
                case 7: {
                    //Contact gadget 에 검색 결과가 하나 나올 때 경로 탐색 테스트
                    m_controller->showDestinationFreeTextSearchInputOccur("asdog");
                    break;
                }
                case 8: {
                    TS_ROUTE_POS pos;
                    pos.tp = { 66513375, 19670485 };
                    m_controller->showAdjustCurrentLocationScreen(pos);
                    break;
                case 9:
                    m_controller->insertWaypoint(1);
                    break;
                }

                default:
                    break;
                }//switch
                return true;
            }
        }
        break;

        case Qt::Key_F1:
        case Qt::Key_F2:
        case Qt::Key_F3:
        case Qt::Key_F4:
        case Qt::Key_F5:
        case Qt::Key_F6:
        case Qt::Key_F7:
        case Qt::Key_F8:
        case Qt::Key_F9:
        case Qt::Key_F11:
        case Qt::Key_F12:
            {
//               int key = keyEvent->key();
//               if (m_enableControl)
//                   key+= 0x00000007;
                m_controller->simulationGadget(m_enableShift, keyEvent->key());
            }
            return true;

        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Q:
        case Qt::Key_E:
        case Qt::Key_Return:
        case Qt::Key_Backspace:
        case Qt::Key_Backslash:
            m_controller->simulationHardKey(keyEvent->key());
            return true;

        case Qt::Key_F10:
            m_controller->setAppForeground(true);
            break;
        }

        // Keys for User Profile Test
        if (keyEvent->modifiers() & Qt::ControlModifier) {
            switch (keyEvent->key()) {
            case Qt::Key_3:
                m_controller->changeUserOccur(0);
                return true;
            case Qt::Key_4:
                m_controller->changeUserOccur(1);
                return true;
            case Qt::Key_5:
                m_controller->changeUserOccur(2);
                return true;
            case Qt::Key_6:
                m_controller->copyUserDataOccur(1,2);
                return true;
            }

            if (keyEvent->modifiers() & Qt::ShiftModifier) {
                switch (keyEvent->key()) {
                case Qt::Key_NumberSign:
                    m_controller->deleteUserOccur(0);
                    return true;
                case Qt::Key_Dollar:
                    m_controller->deleteUserOccur(1);
                    return true;
                case Qt::Key_Percent:
                    m_controller->deleteUserOccur(2);
                    return true;
                }
            }
        }

        // factory reset
        if (keyEvent->modifiers() & Qt::ControlModifier &&
            keyEvent->modifiers() & Qt::AltModifier &&
            keyEvent->key() == Qt::Key_R) {
            m_controller->resetToFactorySettings();
            return true;
        }

        // session creation (passthrough authorization)
        if (keyEvent->modifiers() & Qt::ControlModifier && 
            keyEvent->key() == Qt::Key_A) {
                  QMetaObject::invokeMethod(m_controller->sessionManager()->qobject(), "sessionCreated", Qt::DirectConnection, Q_ARG(bool, true), Q_ARG(int, 0), Q_ARG(int, 0));
                  return true;
        } else if (keyEvent->key() == Qt::Key_A) {
            // 통합 검색 테스트
            //PlatformHelper::instance()->requestFreeTextSearch("서울시 중구 을지로3가");

            // Patrol 테스트
            // 0: 주유소 (SettingConstants::CategoryGasSt)
            // 1: 충전소 (SettingConstants::CategoryChargingSt)
            //10: 전기차 충전소 (SettingConstants::CategoryChargingEvSt)
            //PlatformHelper::instance()->requestAroundSearchPatrol(10);

            // Parking 테스트
            //PlatformHelper::instance()->requestAroundSearchParking();

            // 가젯
            const int GADGET_TOUCH_RELEASED = 2;
            PlatformHelper::instance()->gadgetTouched(GADGET_TOUCH_RELEASED, 1, 1, 0, UIConstants::GadgetIconAqi);

            return true;
        }
    }
#endif

    return false;
}

}
