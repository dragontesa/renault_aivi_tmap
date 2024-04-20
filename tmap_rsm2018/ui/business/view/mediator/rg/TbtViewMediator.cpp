#include "TbtViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "ResourceResolver.h"
#include "StringHelper.h"
#include "ViewRegistry.h"
#include "NavigationControllerConstants.h"
#include "CommonConstants.h"
#include "TestConstants.h"
#include "RgUtils.h"
#include "SimulatorConstants.h"
#include "ApplicationConstants.h"
#include "VSM_Enum.h"

using namespace SKT;

TbtViewMediator::TbtViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent ),
    mCurMapViewMode( MapViewMode::Normal ),
    mCurMapViewModeReason( MapViewMode::None ),
    mSummaryTbtListVisible( false ),
    mIsInHighway( false ),
    mCrossImageVisible( false )
{
    mFirst = findChild(QStringLiteral("first"));
    mSecond = findChild(QStringLiteral("second"));
    mLeftArrow = findChild(QStringLiteral("leftArrow"));
    mTest = -1;

#ifndef BUILD_TARGET
    QQmlProperty::write( mViewComponent, "isBuildTarget", false );
#endif
}

QList<int> TbtViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        RgConstants::UpdateHighwayRouteGuidance,
        RgConstants::UpdateRouteGuidance,
        RgConstants::RequestTbtStateUpdate,
        MapConstants::ApplyMapViewMode,
        RgConstants::UpdateView,
        NavigationControllerConstants::ChangeLanguage,
        TestConstants::RunTest,
        NavigationControllerConstants::GoToRunning,
        NavigationControllerConstants::GoToStandby,
        RgConstants::ReplyToUiState,
        RgConstants::ChangedUiState,
        RgConstants::StartRouteGuidance,
        RgConstants::StopRouteGuidance,
        RgConstants::TerminatedRouteGuidance,
        SimulatorConstants::StopSimulator,
        ApplicationConstants::ChangedGadgetState,
        ApplicationConstants::ReplyToGadgetState,
        MapConstants::RespondToGlobalMapViewStatus,
        MapConstants::GlobalMapViewMoveModeChanged,
    };

    return notes;
}

bool TbtViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        {
            sendNotification(MapConstants::NotifyMapViewMode);
            sendNotification(ApplicationConstants::RequestGadgetState);
            sendNotification(MapConstants::RequestGlobalMapViewStatus);
        }
        break;
    case ArrowClicked:
        {
            auto m = data.toMap();
            auto byCsw = m["byCsw"].toBool();

            if ( !mIsInHighway ) {
                // 17.7.21 hskim edited not to hide tbt view itself when the left arrow is clicked and hide the scecond tbt
                QQmlProperty::write( mLeftArrow, CommonConstants::Visible, false );
                QQmlProperty::write( mSecond, CommonConstants::Visible, false );
                sendNotification(RgConstants::UpdateView, QVariantMap{
                    {CommonConstants::ObjectName, ViewRegistry::objectName( ViewRegistry::SummaryTbtView )},
                    {CommonConstants::Visible, true},
                    {"cswFocus", byCsw}
                });
            }
        }
        break;
    case VisibleChanged:
        {
            auto visible = data.toMap()["visible"].toBool();
            sendNotification(RgConstants::ChangedUiState,
                QVariantMap{
                    {QStringLiteral("item"), RgConstants::TbtVisible},
                    {CommonConstants::Value, visible}
                }
            );
        }
        break;
    case RegistCsw:
        {
            sendNotification(NavigationControllerConstants::RegistCSW, data);
        }
        break;
    case RemoveCsw:
        {
            sendNotification(NavigationControllerConstants::RemoveCSW, data);
        }
        break;
    case SelectCsw:
        {
            sendNotification(NavigationControllerConstants::SelectCSW, data);
        }
        break;
    default:
        return false;
    }

    return true;
}

bool TbtViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case RgConstants::ReplyToUiState:
    case RgConstants::ChangedUiState:
        {
            auto m = note->getBody().toMap();
            auto item = m[QStringLiteral("item")].toInt();
            auto value = m[QStringLiteral("value")].toInt();

            if ( item == RgConstants::SummaryTbtVisible ) {
                mSummaryTbtListVisible = value;                
            }
            else if ( item == RgConstants::CrossImageVisible ) {
                //T_DEBUG( QString( "CrossImageVisible #1 %1" ).arg( value ) );
                if ( mCrossImageVisible != value ) {
                    mCrossImageVisible = value;
                    QQmlProperty::write( mLeftArrow, CommonConstants::Visible, !mCrossImageVisible );
                    //T_DEBUG( QString( "CrossImageVisible #2 %1" ).arg( mCrossImageVisible ) );
                }
            }

            updateSecondTbtAndArrowVisiblilty();
        }
        break;
    case RgConstants::UpdateHighwayRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();
            bool update = true;
#ifdef TEST_GADGET_RG
            auto isMapViewSimul = mGlobalMapViewMoveMode == VSM_MOVE_MODE_SIMULATION;
            update = mIsGadgetShown ? !vo.isSimul() : (isMapViewSimul ? vo.isSimul() : !vo.isSimul());
#endif
            if (update) {
                if ( !vo.isNull() ) {
                    if ( vo.nShowHighway() == 1 ) {
                        mIsInHighway = true;
                    }
                    else {
                        mIsInHighway = false;
                    }
                }
            }
        }
        break;
    case RgConstants::UpdateRouteGuidance:
        {
            auto vo = note->getBody().value<RgDataVo>();            
            bool update = true;
#ifdef TEST_GADGET_RG
            auto isMapViewSimul = mGlobalMapViewMoveMode == VSM_MOVE_MODE_SIMULATION;
            update = mIsGadgetShown ? !vo.isSimul() : (isMapViewSimul ? vo.isSimul() : !vo.isSimul());
#endif
            if (update) {
                //T_DEBUG(QString("[Gadget]moveMode: %1 mIsGadgetShown: %2 isSimul: %3").arg(mGlobalMapViewMoveMode).arg(mIsGadgetShown).arg(vo.isSimul()));
                updateView( vo );
            }
        }
        break;
    case RgConstants::RequestTbtStateUpdate:
        {
            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtFirstTurnType},
                {QStringLiteral("value"),mLastState.firstTurnType}
            });
            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtFirstDistance},
                {QStringLiteral("value"),mLastState.firstDistance}
            });
            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtFirstPointName},
                {QStringLiteral("value"),mLastState.firstPointName}
            });

            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtSecondTurnType},
                {QStringLiteral("value"),mLastState.secondTurnType}
            });
            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtSecondDistance},
                {QStringLiteral("value"),mLastState.secondDistance}
            });

			// TODO 복수이면 위젯이 요청하면 mUpdate를 여러벌로( widget id )
            mUpdate = NotifiableUpdate();
        }
        break;
    case MapConstants::ApplyMapViewMode:
        {
            auto data = note->getBody().value<QVariantMap>();
            mCurMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
            mCurMapViewModeReason = (MapViewMode::ReasonK)data[QStringLiteral("reason")].toInt();
        }
        break;
    case RgConstants::UpdateView:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto objectName = data[CommonConstants::ObjectName].toString();
            if ( objectName == ViewRegistry::objectName( ViewRegistry::TbtView ) ) {
                auto visible = data[CommonConstants::Visible].toBool();
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, visible );
                auto cswFocus = data["cswFocus"].toBool();
                if (cswFocus) {
                    sendNotification(NavigationControllerConstants::SelectCSWByObjectName, "leftArrowCsw");
                }
            }
        }
        break;
    case NavigationControllerConstants::ChangeLanguage:
        {
            auto imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), mFirstImg );
            QQmlProperty::write( mFirst, QStringLiteral("image"), imgPath );

            imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), mSecondImg );
            QQmlProperty::write( mSecond, QStringLiteral("image"), imgPath );

            QMetaObject::invokeMethod( mViewComponent, "retranslateUi" );
        }
        break;
    case TestConstants::RunTest:
        {
            auto data = note->getBody().value<QVariantMap>();
            mTest = data["test"].toInt();
        }
        break;
    case NavigationControllerConstants::GoToRunning:
        {
            QQmlProperty::write( mViewComponent, QStringLiteral("appRunning"), true );
        }
        break;
    case NavigationControllerConstants::GoToStandby:
        {
            QQmlProperty::write( mViewComponent, QStringLiteral("appRunning"), false );
        }
        break;    
    case RgConstants::StartRouteGuidance:
//    case RgConstants::StopRouteGuidance:
//    case RgConstants::TerminatedRouteGuidance:
//    case SimulatorConstants::StopSimulator:
        {
            //T_DEBUG( QString( "StartRouteGuidance #1" ) );
            mLastDistances = QStringList();
            mFirstImg = "";
            mSecondImg = "";
            mLastState = NotifiableUpdate();
            mUpdate = NotifiableUpdate();
            QMetaObject::invokeMethod(mViewComponent, "reset");
            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtReset}
            });
        }
        break;
    case ApplicationConstants::ChangedGadgetState:
    case ApplicationConstants::ReplyToGadgetState:
        {
            auto m = note->getBody().toMap();
            mIsGadgetShown = m["visible"].toBool();            
        }
        break;
    case MapConstants::GlobalMapViewMoveModeChanged:
        {
            mGlobalMapViewMoveMode = note->getBody().toInt();
        }
        break;
    case MapConstants::RespondToGlobalMapViewStatus:
        {
            auto m = note->getBody().toMap();
            mGlobalMapViewMoveMode = m["moveMode"].toInt();
        }
        break;
    default:
        return false;
    }

    return true;
}

QVariantMap TbtViewMediator::makeModel(
    const QString& img, const QString& name,
    const QVariantList& distance, const QString& progress
    )
{
    QVariantMap model;

    model[QStringLiteral("img")] = img;
    model[QStringLiteral("name")] = name;
    model[QStringLiteral("distanceLength")] = distance.length();
    model[QStringLiteral("distance")] = distance;
    model[QStringLiteral("progress")] = progress;

    return model;
}

void TbtViewMediator::updateView( const RgDataVo& rgDataVo )
{
    if ( rgDataVo.isNull() ) {
        return;
    }

    if ( !MapViewMode::isInRouteGuidance( mCurMapViewMode ) ) {
        return;
    }

    QString img1;
    QString img2;
    QString imgPath;
    QString distNumber;

    auto turnType1 = mTurnType1 = rgDataVo.stGuidePoint_nTBTTurnType();
    auto turnType2 = mTurnType2 = rgDataVo.stGuidePointNext_nTBTTurnType();

    // 1st

    if ( turnType1 == 0 ) {
        turnType1 = RgTbtCode::TURN_STR;        
    }
    img1 = RgUtils::tbtFirstImage(turnType1);
    if ( !img1.isEmpty() )  {
        mFirstImg = img1;
        imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img1 );
    }
    else {
        mFirstImg = imgPath = QString();
        qDebug() << QString( "[RG][TBT]Unexpected First Turn Code %1" ).arg( turnType1 );
    }

    //T_DEBUG( QString( "imgPath: %1" ).arg( imgPath ) );

    QQmlProperty::write( mViewComponent, CommonConstants::Visible, !mFirstImg.isEmpty() );

    QQmlProperty::write( mFirst, CommonConstants::Visible, true );
    QQmlProperty::write( mFirst, QStringLiteral("image"), imgPath );

    mLastState.firstTurnType = turnType1;
    if ( mUpdate.firstTurnType != turnType1 ) {
        mUpdate.firstTurnType = turnType1;
        sendNotification(RgConstants::UpdateTbtState, QVariantMap{
            {QStringLiteral("item"),RgConstants::TbtFirstTurnType},
            {QStringLiteral("value"),mUpdate.firstTurnType}
        });
    }

    auto dist = rgDataVo.stGuidePoint_nTBTDist();
    distNumber = RgUtils::formatDistance( dist );

    //T_DEBUG( QString( "dist: %1 distNumbe: %2" ).arg( dist ).arg( distNumber ) );

    QStringList images;
    for(int i = 0; i < distNumber.length(); i++){
        auto ch = distNumber.at(i);
        imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), RgUtils::tbtFirstDistanceNumberImage(ch) );
        images.append( imgPath );
        if ( ch == QChar('k') ) {
            i++;
        }
        //T_DEBUG( QString( "first image: %1" ).arg( imgPath ) );
    }
    QMetaObject::invokeMethod( mViewComponent, "setFirstDistNumberImages", Q_ARG(QVariant,images) );

    mLastState.firstDistance = distNumber;
    if ( mUpdate.firstDistance != distNumber ){
        mUpdate.firstDistance = distNumber;
        sendNotification(RgConstants::UpdateTbtState, QVariantMap{
            {QStringLiteral("item"),RgConstants::TbtFirstDistance},
            {QStringLiteral("value"),mUpdate.firstDistance}
        });
    }

    QStringList dirNames;

    QString pointName;
    if ( !rgDataVo.stGuidePoint_szCrossName().isEmpty() ) {
        pointName = rgDataVo.stGuidePoint_szCrossName();
        dirNames.append( pointName );
    }
    if ( !rgDataVo.stGuidePoint_szNearDirName().isEmpty() ) {
        pointName = rgDataVo.stGuidePoint_szNearDirName();
        dirNames.append( pointName );
    }
    if ( !rgDataVo.stGuidePoint_szMidDirName().isEmpty() ) {
        pointName = rgDataVo.stGuidePoint_szMidDirName();
        dirNames.append( pointName );
    }
    if ( !rgDataVo.stGuidePoint_szFarDirName().isEmpty() ) {
        pointName = rgDataVo.stGuidePoint_szFarDirName();
        dirNames.append( pointName );
    }

    if ( pointName.isEmpty() ) {
        pointName = rgDataVo.stGuidePoint_szTBTMainText();
        dirNames.append( pointName );
        qDebug() << QString( "[RG][TBT]Coudld not get any point name for the first lane. Alternatively, use TBTMainText, '%1'." ).arg( pointName );
    }

    if ( mDirNames != dirNames ) {
        mDirNames = dirNames;
        mCurrentDirNameIndex = 0;
        mDirNameToggleTimer.restart();
    }
    else {
        auto elapsed = mDirNameToggleTimer.elapsed();
        if ( elapsed >= DIRNAME_TOGGLE_DELAY ) {
            mCurrentDirNameIndex++;
            mCurrentDirNameIndex %= mDirNames.count();
            mDirNameToggleTimer.restart();
        }
    }

    pointName = mDirNames[mCurrentDirNameIndex];

    QQmlProperty::write( mFirst, QStringLiteral("pointName"), pointName );

    mLastState.firstPointName = pointName;
    if ( mUpdate.firstPointName != pointName ){
        mUpdate.firstPointName = pointName;
        sendNotification(RgConstants::UpdateTbtState, QVariantMap{
            {QStringLiteral("item"),RgConstants::TbtFirstPointName},
            {QStringLiteral("value"),mUpdate.firstPointName}
        });
    }

    // 2nd

    if ( turnType2 == 0 ) {
        turnType2 = RgTbtCode::TURN_STR;
    }
    if ( turnType2 != 0) {        
        //T_DEBUG( QString( "turnType2 != 0" ) );

        img2 = RgUtils::tbtSecondImage(turnType2);
        if ( !img2.isEmpty() ) {
            mSecondImg = img2;
            imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), img2 );
        }
        else {
            //mSecondImg = imgPath = QString();
            T_DEBUG( QString( "Unexpected Second Turn Code %1" ).arg( turnType2 ) );
        }

        mLastState.secondTurnType = turnType2;
        if ( mUpdate.secondTurnType != turnType2 ) {
            mUpdate.secondTurnType = turnType2;
            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtSecondTurnType},
                {QStringLiteral("value"),mUpdate.secondTurnType}
            });
        }

        if ( !mSummaryTbtListVisible ) {
            QQmlProperty::write( mSecond, CommonConstants::Visible, true );
        }
        QQmlProperty::write( mSecond, QStringLiteral("image"), imgPath );

        auto dist = rgDataVo.stGuidePointNext_nSvcLinkDist();
        distNumber = RgUtils::formatDistance( dist );

        //T_DEBUG( QString( "dist: %1 distNumbe: %2" ).arg( dist ).arg( distNumber ) );

        QStringList images;
        for(int i = 0; i < distNumber.length(); i++){
            auto ch = distNumber.at(i);
            imgPath = ResourceResolver::instance()->imageUrl( QStringLiteral("tbt"), RgUtils::tbtSecondDistanceNumberImage(ch) );
            images.append( imgPath );
            if ( ch == QChar('k') ) {
                i++;
            }
        }
        QMetaObject::invokeMethod( mViewComponent, "setSecondDistNumberImages", Q_ARG(QVariant,images) );

        mLastState.secondDistance = distNumber;
        if ( mUpdate.secondDistance != distNumber ){
            mUpdate.secondDistance = distNumber;
            sendNotification(RgConstants::UpdateTbtState, QVariantMap{
                {QStringLiteral("item"),RgConstants::TbtSecondDistance},
                {QStringLiteral("value"),mUpdate.secondDistance}
            });
        }
    }

    updateSecondTbtAndArrowVisiblilty();
}

void TbtViewMediator::updateSecondTbtAndArrowVisiblilty()
{
    bool secondVisible = true;
    bool secondAndLeftArrowVisible = true;
    if (mTurnType1 == RgTbtCode::GOAL) {
        secondVisible = false;
        secondAndLeftArrowVisible = false;
    }
    else {
        if ( mIsInHighway ) {
            //secondVisible = false;
            secondAndLeftArrowVisible = false;
        }
        else {
            if ( mSummaryTbtListVisible ) {
                secondVisible = false;
                secondAndLeftArrowVisible = false;
            }
            else if ( mCrossImageVisible) {
                secondAndLeftArrowVisible = false;
            }
        }
    }

    QQmlProperty::write( mSecond, CommonConstants::Visible, secondVisible );

    secondAndLeftArrowVisible = secondAndLeftArrowVisible ? mTurnType2 != 0 : secondAndLeftArrowVisible;
    QQmlProperty::write( mLeftArrow, CommonConstants::Visible, secondAndLeftArrowVisible );
}
