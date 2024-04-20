/*
 * ProfileManager.cpp
 *
 */

#include "ProfileManager.h"
#include "DatabaseHelper.h"
#include "SettingDto.h"
#include "RecentDestinationDto.h"
#include "RecentSearchDto.h"
#include "FavoriteDto.h"
#include "LastRouteDto.h"
#include "NavigationControllerConstants.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "UserProfileConstants.h"
#include "searchrequest.h"
#include "SearchHelper.h"
#include "SearchConstants.h"
#include "NavibookmarkAgentProxy.h"
#include "NavigationControllerProxy.h"
#include "ApplicationConstants.h"
#include "StringHelper.h"
#include <QTextCodec>
#include "../../common/navigationcontroller.h"
#include "../../common/sessionmanagerinterface.h"
#include "PoiHelper.h"

namespace SKT
{

ProfileManager::ProfileManager():
   QObject(nullptr)
{
}

ProfileManager* ProfileManager::instance()
{
	static ProfileManager* instance_  = nullptr;
	if (instance_ == nullptr)
	    instance_ = new ProfileManager();

	return instance_;
}

void ProfileManager::initProfileManager(
        QObject* owner,
		int initProfileId,
        QObject* parent)
{

    mActiveProfileId = initProfileId;
    mLastProfileId   = initProfileId;
    mChannelServerPolicySuccess = false;
    mNeedRemoteSync = false;
    mWaitingRemoteSync = false;
    mRemoteSyncStatus  = R_SYNC_HOMEOFFICE; // home,office 동기화 구분 없음, 서버지원 안함
    NavigationController* nc = qobject_cast<NavigationController*>(owner);
    mSessionManager = nc->sessionManager();

    connect(owner,SIGNAL(deleteUserOccur(int)),this, SLOT(onResetUserData(int)));
	connect(owner,SIGNAL(changeUserOccur(int)),this, SLOT(onChangeUser(int)));
	connect(owner,SIGNAL(copyUserDataOccur(int,int)),this, SLOT(onCopyUserData(int,int)));
	connect(owner,SIGNAL(createUserOccur(int)),this, SLOT(onUserProfileCreated(int)));
    connect(owner,SIGNAL(changeProfileNameOccur(const QString&)),this, SLOT(onChangeProfileName(const QString&)));
    connect(owner, SIGNAL(nddsServerPolicyFinished(bool)), this, SLOT(onChannelServerPolicyFinished(bool)));

    connect(owner, SIGNAL(changeUserOccur(int)), mSessionManager->qobject(), SLOT(changeSessionByProfileId(int)));
	connect(mSessionManager->qobject(), SIGNAL(sessionChanged(int)), this, SLOT(onSessionChanged(int)));

    #ifndef PLATFORM_GEN3
    // read last usre profile id
	QString lastFilePath(DatabaseHelper::instance()->getLastUserFilePath());

	if (QFile::exists(lastFilePath)) {
		QFile fl(lastFilePath);
		if (!fl.open(QIODevice::ReadOnly)) {
			T_DEBUG( QStringLiteral("failed to read last user"));
            mActiveProfileId = 0;
		}
		else {
			QTextStream ts(&fl);
            ts >> mActiveProfileId;
        }
	}
    // Desktop에서 세션매니저와 초기 프로파일 동기화
     QMetaObject::invokeMethod(mSessionManager->qobject(), "changeSessionByProfileId", Q_ARG(int, mActiveProfileId) );
    #endif

     DatabaseHelper::instance()->setCurrentUserProfileId(mActiveProfileId);
     QString profilePath = DatabaseHelper::instance()->getUserDBFilePath(mActiveProfileId);

     if (!QFile::exists(profilePath))
         createProfile(mActiveProfileId);

#ifndef PLATFORM_GEN3
//     QTimer::singleShot(1,this,[this](){
     // Desktop에서 UI 프로파일 동기화
     mvc::INotificationPtr note = t_new_shared<Notification>(this, NavigationControllerConstants::ChangedUserProfile, QVariant::fromValue(mActiveProfileId));
     Facade::instance()->notifyObservers(note);
//     });
#endif
}

int ProfileManager::activeUserProfileId() const
{
    return mActiveProfileId;
}

int ProfileManager::lastUserProfileId() const
{
    return mLastProfileId;
}

QString ProfileManager::activeUserProfileName() const
{
    return mActiveProfileName;
}

QString ProfileManager::lastUserProfileName() const
{
   return mLastProfileName;
}

void ProfileManager::onChangeProfileName(const QString& newName)
{
    qDebug() << "pm, app profile name changed to: " << newName;
    mActiveProfileName = newName;
}

void ProfileManager::onResetUserData(int profileId)
{
	// 삭제하고 새로 프로파일 생성
	QString profilePath = DatabaseHelper::instance()->getUserDBFilePath(profileId);
	if (QFile::exists(profilePath))
	{
		// free db connection
		DatabaseHelper::instance()->disconnect(profileId);
		QFileDevice::Permissions perm = QFile::permissions(profilePath);
		QFile::setPermissions(profilePath, perm|QFileDevice::WriteOwner);
		if (! QFile::remove(profilePath) ) {
			T_DEBUG(QStringLiteral("reset user profile failure:%1").arg(profilePath));
			QMetaObject::invokeMethod(sender(),"deleteUserDone", Q_ARG(bool,false));
			return;
		}
	}

	createProfile(profileId);
	DatabaseHelper::instance()->disconnect(profileId);

    if (profileId == mActiveProfileId)
	{
#ifdef TMAP_UI_PM_SIGNAL
		emit profileUpdated();
#else
		mvc::INotificationPtr note = t_new_shared<Notification>(this, NavigationControllerConstants::UpdatedUserProfile, QVariant::fromValue(profileId));
		Facade::instance()->notifyObservers(note);
#endif
	}

	// response to tmapcontroller
	QMetaObject::invokeMethod(sender(),"deleteUserDone", Q_ARG(bool,true));
}

void ProfileManager::onChangeUser(int profileId)
{
    qDebug() << "app-pm user changed to : " << profileId;
    // create profile if the user profile is not consist
	QString profilePath = DatabaseHelper::instance()->getUserDBFilePath(profileId);
	if (!QFile::exists(profilePath)) {
		createProfile(profileId);
	}
	DatabaseHelper::instance()->setCurrentUserProfileId(profileId);
	// fixed TMCRSM-2779, we don't gurentee all table consist so needs below 
	QSqlDatabase db = DatabaseHelper::instance()->connection(profileId);
	orm::TOrm::instance()->create<SettingDto>(db);
	orm::TOrm::instance()->create<RecentDestinationDto>(db);
	orm::TOrm::instance()->create<RecentSearchDto>(db);
	orm::TOrm::instance()->create<FavoriteDto>(db);
    orm::TOrm::instance()->create<LastRouteDto>(db);

    // don't stop the changing user from system, and that MUST guarantee active profile get sync with system
//    if (mActiveProfileId == profileId) return; // fix TMCRSM-3595

   mLastProfileId = mActiveProfileId;
   mLastProfileName = mActiveProfileName;
   mActiveProfileId = profileId;
   qDebug() << "app pm, last profile name: "<< mLastProfileName;
#ifdef TMAP_UI_PM_SIGNAL
	    emit profileChanged();
#else
		mvc::INotificationPtr note = t_new_shared<Notification>(this, NavigationControllerConstants::ChangedUserProfile, QVariant::fromValue(profileId));
		Facade::instance()->notifyObservers(note);
#endif

	// notify navigation property
	QVariantMap body;
	body.insert(QStringLiteral("type"),NavigationPropertyProtocol::TmapUserProfileChanged);
	body.insert(QStringLiteral("propertyValue"),QVariant::fromValue(profileId));

	mvc::INotificationPtr note2 = t_new_shared<Notification>(this, NavigationPropertyConstants::setNavigationProperty, body);
	Facade::instance()->notifyObservers(note2);

    // navigation properties there needs to be synchronized with system i/f like VR
    mvc::INotificationPtr note3 = t_new_shared<Notification>(this, SearchConstants::RequestFavorites, QVariantMap{{"service.sds",true}});
    mvc::INotificationPtr note4 = t_new_shared<Notification>(this, SearchConstants::RequestRecentDestination, QVariantMap{{"service.sds",true}});
    mvc::INotificationPtr note5 = t_new_shared<Notification>(this, SearchConstants::RequestHomeFavorite, QVariantMap{{"service.sds",true}});
    mvc::INotificationPtr note6 = t_new_shared<Notification>(this, SearchConstants::RequestOfficeFavorite, QVariantMap{{"service.sds",true}});
    Facade::instance()->notifyObservers(note3);
    Facade::instance()->notifyObservers(note4);
    Facade::instance()->notifyObservers(note5);
    Facade::instance()->notifyObservers(note6);
}

void ProfileManager::onCopyUserData(int srcId, int dstId)
{
	QString srcUserFile = DatabaseHelper::instance()->getUserDBFilePath(srcId);
	QString dstUserFile = DatabaseHelper::instance()->getUserDBFilePath(dstId);
	if (!QFile::exists(srcUserFile)) { // 프로파일이 없으면 빈 프로파일 생성
		if ( !createProfile(srcId) )
		{
			T_DEBUG(QStringLiteral("copy user data file not found: %1 ").arg(srcUserFile));
			QMetaObject::invokeMethod(sender(), "copyUserDataDone", Q_ARG(bool, false));
			return;
		}
	}
	if (QFile::exists(dstUserFile))	{
		// 대상 프로파일이 존재하면 삭제 하고 소스 프로파일 copy
		QFileDevice::Permissions perm = QFile::permissions(dstUserFile);
		QFile::setPermissions(dstUserFile, perm|QFileDevice::WriteOwner);
		if (!QFile::remove(dstUserFile)) {
			T_DEBUG(QStringLiteral("copy user data destination file deletion failure: %1").arg(dstUserFile));
			QMetaObject::invokeMethod(sender(), "copyUserDataDone", Q_ARG(bool, false));
			return;
		}
	}

	if ( ! QFile::copy(srcUserFile, dstUserFile) )	{
		T_DEBUG(QStringLiteral("copy user data failure: from %1 to %2").arg(srcUserFile).arg(dstUserFile));
	}

    if (dstId == mActiveProfileId)
	{
#ifdef TMAP_UI_PM_SIGNAL
		emit profileUpdated();
#else
		mvc::INotificationPtr note = t_new_shared<Notification>(this, NavigationControllerConstants::UpdatedUserProfile, QVariant::fromValue(dstId));
		Facade::instance()->notifyObservers(note);
#endif
	}

	QMetaObject::invokeMethod(sender(), "copyUserDataDone", Q_ARG(bool, true));
}

void ProfileManager::onUserProfileCreated(int id)
{
	bool success = createProfile(id);
	QMetaObject::invokeMethod(sender(), "createUserDone", Q_ARG(bool, success));
}

/**
 * called from session manager
 */
void ProfileManager::onSessionChanged(int ownerId)
{
    qDebug() << "pm, app session changed, 1 id:" << ownerId;
    // note! it must gurantee which current profile id is same as the session manager's one.
    if (ownerId != mActiveProfileId) return;
    //if (ownerId == 0) return; // don't have synchronization for anonymous

    QByteArray sessionKey = mSessionManager->sessionKey();

    // free recent cloud data
    DatabaseHelper::instance()->disconnectForCloud();
    
    qDebug() << "pm, app session changed, 2 id:" << ownerId << ", ak:"<<sessionKey;
	// 2019-4-26 donot authorize for guest,  Spec Frozen by Kimkangyoung
	if (ownerId > 0 && sessionKey.size() > 0) { // request sync to server if user has been authorized

        // 아직 연동이 되지 않았어도 session이 생성 되었다면 OTP유저이므로
        // 경로 이어하기 정보는 network연결 관계없이 Cloud유저용으로 읽기/쓰기가 되도록 한다.
        // 20.03.29 ymhong
        DatabaseHelper::instance()->setCloudDb();

        // do sync with cloud data on server if network is online
        auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
        if (proxy.isNull()) return;
        bool online = proxy->getOnlineState();
        if (online) {
            qDebug() << "pm, app session changed, 2-1, online, id:" << ownerId;
            // TODO: change working db from local to cloud user's -> TMCRSM-3025

            qDebug() << "[OTP] connection for cloud !!!";

            QSqlDatabase db = DatabaseHelper::instance()->connectionForCloud();
            orm::TOrm::instance()->create<SettingDto>(db);
            orm::TOrm::instance()->create<RecentDestinationDto>(db);
            orm::TOrm::instance()->create<RecentSearchDto>(db);
            orm::TOrm::instance()->create<FavoriteDto>(db);
            orm::TOrm::instance()->create<LastRouteDto>(db);

            // 서버 동기화 DB에 맵뷰 카바타 셋업.
            mvc::INotificationPtr note = t_new_shared<Notification>(this, NavigationControllerConstants::UpdateCavatarIcon, QVariant());
            Facade::instance()->notifyObservers(note);

            qDebug() << "[OTP] onSessionChanged(" << ownerId << "), channel policy->" << mChannelServerPolicySuccess;

            if (mChannelServerPolicySuccess) {
                qDebug() << "pm, app session changed, id:"<<ownerId<<", request sync to server .. ";

                // it needs to consider whether is routing for performance
				onRequestUserDataDownload();
            }
            else {
                mNeedRemoteSync = true;
            }
        }
	}
    if (ownerId > 0) {
        mvc::INotificationPtr note = t_new_shared<Notification>(this, ApplicationConstants::ChangedSession, QVariant());
        Facade::instance()->notifyObservers(note);
    }
}


/*!
 * user profile has been downloaded from server
 * this is reserved for further
 */
void ProfileManager::onUserProfileDownloaded(int profileId, const QVariant& data)
{
	// TODO: synchronize the profile with local's one
}

/*!
 * DONOT USE
 * user data such like favorite or recent destination downloaded from server
 */
void ProfileManager::onUserDataDownloaded(const QVariant& data)
{
    syncUserData(data);
}

/*!
 * request to download user data 
 * it must be called only when a user has been authorized
**/
void ProfileManager::onRequestUserDataDownload()
{
	if (mWaitingRemoteSync == false) {
        qDebug() << "pm, request download";
		mWaitingRemoteSync = true;
		mRemoteSyncStatus = R_SYNC_HOMEOFFICE;
		connect(this, &ProfileManager::waitingSyncFinished, this, &ProfileManager::onUserDataDownloadDone);
		//requestMyHomeOfficeFavorite();
		requestRemoteFavorite();
		requestRemoteRecentDestination();
        requestRemoteFindSendToCar();
	}
    qDebug() << "pm, request download end";
}

void ProfileManager::onUserDataDownloadDone()
{
    if ((mRemoteSyncStatus&R_SYNC_ALL_DONE) != R_SYNC_ALL_DONE) {
		qInfo() << "pm, server sync fail!";
		// switch to local user profile
        DatabaseHelper::instance()->disconnectForCloud();
	}
	else {
		qInfo() << "pm, server sync success!";
	}

}
void ProfileManager::requestMyHomeOfficeFavorite()
{
    SearchRequestBase* r = new SearchRequestBase(SearchRequestBase::SearchMyHomeOffice);
    SearchRequestBasePtr req(r);
    SearchReplyBaseInterface* reply = SearchHelper::instance()->request(req);
    if (reply) {
    	connect(reply->qobject(), SIGNAL(finished()), this, SLOT(onResponsedMyHomeOfficeFavorite()));
    }

}


void ProfileManager::requestRemoteFavorite(int seq)
{
    const int reqmax = D_MAX_RESULTLIST * 10;
	mDownloadedFavorites.clear();
    SearchRequestBase* r = new SearchRequestBase(SearchRequestBase::SearchFavorites);
    r->setStartIndex(seq);
    r->setCount(reqmax);
    SearchRequestBasePtr req(r);
    SearchReplyBaseInterface* reply = SearchHelper::instance()->request(req);
    if (reply) {
    	connect(reply->qobject(), SIGNAL(finished()), this, SLOT(onResponsedRemoteFavorite()));
    }
}

void ProfileManager::requestRemoteRecentDestination(int seq)
{
    const int reqmax = D_MAX_RESULTLIST * 10;
	mDownloadedRecentDests.clear();
    SearchRequest* r = new SearchRequest(SearchRequestBase::SearchRecentDestination);
    r->setStartIndex(seq);
    r->setCount(reqmax);
    SearchRequestBasePtr req(r);
    SearchReplyBaseInterface* reply = SearchHelper::instance()->request(req);
    if (reply) {
    	connect(reply->qobject(), SIGNAL(finished()), this, SLOT(onResponsedRemoteRecentDestination()));
    }
}

void ProfileManager::requestRemoteSetting()
{

}

void ProfileManager::requestRemoteRegisterSendToCar(const QVariantMap &reqData, int seq)
{
    const int reqmax = D_MAX_RESULTLIST * 10;
    SearchRequestBase* r = new SearchRequestBase(SearchRequestBase::SearchRegisterSendToCar);
    r->setStartIndex(seq);
    r->setCount(reqmax);
    r->setExtra(reqData);

    SearchRequestBasePtr req(r);
    SearchReplyBaseInterface* reply = SearchHelper::instance()->request(req);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(onResponsedRemoteRegisterSendToCar()));
    }
}

void ProfileManager::requestRemoteFindSendToCar(int seq)
{
    const int reqmax =D_MAX_RESULTLIST * 10;
    SearchRequestBase* r = new SearchRequestBase(SearchRequestBase::SearchFindSendToCar);
    r->setStartIndex(seq);
    r->setCount(reqmax);

    SearchRequestBasePtr req(r);
    SearchReplyBaseInterface* reply = SearchHelper::instance()->request(req);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(onResponsedRemoteFindSendToCar()));
    }
}

void ProfileManager::requestRemoteRemoveSendToCar(const QVariantMap &reqData, int seq)
{
    const int reqmax = D_MAX_RESULTLIST * 10;
    SearchRequestBase* r = new SearchRequestBase(SearchRequestBase::SearchRemoveSendToCar);
    r->setStartIndex(seq);
    r->setCount(reqmax);
    r->setExtra(reqData);

    SearchRequestBasePtr req(r);
    SearchReplyBaseInterface* reply = SearchHelper::instance()->request(req);
    if (reply) {
        connect(reply->qobject(), SIGNAL(finished()), this, SLOT(onResponsedRemoteRemoveSendToCar()));
    }
}


void ProfileManager::onChannelServerPolicyFinished(bool success)
{
    mChannelServerPolicySuccess = success;
    if (mNeedRemoteSync && success) {
        mNeedRemoteSync = false;
        // do synchronize with user's data on cloud
		onRequestUserDataDownload();
        qDebug() << "[OTP] onChannelServerPolicyFinished(" << success << ")";
    }
}

void ProfileManager::onResponsedMyHomeOfficeFavorite()
{
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
    sender()->deleteLater();
	bool success = false;
    if (reply) {
    		SearchRequestBasePtr req = reply->request();
    		// Q_ASSERT(req.data());
    	if (!reply->error()) {
			success = true;
    		SearchMyHomeOfficeReplyInterface* reply = qobject_cast<SearchMyHomeOfficeReplyInterface*>(sender());
    		const TPOI_MYHOMEOFFICE_INFO* my = reply->results();
    		QVariantMap home,office;

    		home[QStringLiteral("alias")]  = StringHelper::instance()->fromCp949(my->baseInfo.szHomeCustName);
    		home[SearchConstants::Dest]    = StringHelper::instance()->fromCp949(my->szHomePoiName);
    		home[CommonConstants::Address] = StringHelper::instance()->fromCp949(my->szHomeAddress);
    		home[CommonConstants::X]  = my->baseInfo.Hometp.x;
    		home[CommonConstants::Y]  = my->baseInfo.Hometp.y;
    		home[SearchConstants::PoiID]   = my->baseInfo.nHomePoiID;
    		home[SearchConstants::NavSeq]  = QString(my->baseInfo.szHomeNavSeq).toInt();
    		home[SearchConstants::RpFlag]  = my->baseInfo.byHomeRpFlag;
    		home[SearchConstants::DestType] = 2;
    		home[QStringLiteral("extrainfo")] = StringHelper::instance()->fromCp949(my->baseInfo.szHomeAddInfo);
    		home[QStringLiteral("instime")]   = QString(my->baseInfo.szHomeInsDatetime);

    		office[QStringLiteral("alias")]  = StringHelper::instance()->fromCp949(my->baseInfo.szOfficeCustName);
    		office[SearchConstants::Dest]    = StringHelper::instance()->fromCp949(my->szOfficePoiName);
    		office[CommonConstants::Address] = StringHelper::instance()->fromCp949(my->szOfficeAddress);
    		office[CommonConstants::X]  = my->baseInfo.Officetp.x;
    		office[CommonConstants::Y]  = my->baseInfo.Officetp.y;
    		office[SearchConstants::PoiID]   = my->baseInfo.nOfficePoiID;
    		office[SearchConstants::NavSeq]  = QString(my->baseInfo.szOfficeNavSeq).toInt();
    		office[SearchConstants::RpFlag]  = my->baseInfo.byOfficeRpFlag;
    		office[SearchConstants::DestType] = 1;
    		home[QStringLiteral("extrainfo")] = StringHelper::instance()->fromCp949(my->baseInfo.szHomeAddInfo);
    		home[QStringLiteral("instime")]   = QString(my->baseInfo.szHomeInsDatetime);


            mvc::INotificationPtr note1 = t_new_shared<Notification>(this, SearchConstants::AddFavorite, home);
            mvc::INotificationPtr note2 = t_new_shared<Notification>(this, SearchConstants::AddFavorite, office);
            Facade::instance()->notifyObservers(note1);
            Facade::instance()->notifyObservers(note2);
    	} else {
             qInfo() << QStringLiteral("pm, remote homeoffice failed: %1").arg(reply->errorCause());
    	}
    } else {
        qInfo() << QStringLiteral("pm, remote homeoffice reply fail");
	}

    if (success) {
		mRemoteSyncStatus = mRemoteSyncStatus&(~R_SYNC_HOMEOFFICE_FAIL); // reset fail history
        mRemoteSyncStatus |= R_SYNC_HOMEOFFICE;
	}
	else {
		mRemoteSyncStatus = mRemoteSyncStatus&(~R_SYNC_HOMEOFFICE); // reset home/office history
		mRemoteSyncStatus |= R_SYNC_HOMEOFFICE_FAIL;
	}
	
    int done = mRemoteSyncStatus&0x000f;
	int fail = (mRemoteSyncStatus&0x0f00)>>8;
	int all  = done|fail;

    if (mWaitingRemoteSync && (all&R_SYNC_ALL_DONE) == R_SYNC_ALL_DONE) {
        mWaitingRemoteSync = false;
        emit waitingSyncFinished();
    }
}

void ProfileManager::onResponsedRemoteFavorite()
{
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
    bool syncmore = false;
	bool success = false;
	if (reply) {
			SearchRequestBasePtr req = reply->request();
			// Q_ASSERT(req.data());
		if (!reply->error()) {
			success = true;
			SearchFavoriteReplyInterface* favReply = qobject_cast<SearchFavoriteReplyInterface*>(sender());
			int cnt = favReply->resultCount();
			int total = favReply->totalCount();
			int seq = req->startIndex();
			const TPOI_FAVORITE_INFO* res = favReply->results();
			for (int i=0;i<cnt;i++) {
				FavoriteDto* dto = new FavoriteDto();
				dto->setDest(StringHelper::instance()->fromCp949(res[i].baseInfo.szCustName));
				dto->setAlias(StringHelper::instance()->fromCp949(res[i].baseInfo.szCustName));
				dto->setAddress(StringHelper::instance()->fromCp949(res[i].szAddress));
				dto->setMapX(res[i].baseInfo.tp.x);
				dto->setMapY(res[i].baseInfo.tp.y);
				dto->setPoiID(res[i].baseInfo.nPoiID);
                dto->setNavSeq(QString(res[i].baseInfo.szNavSeq).toInt());
                dto->setRpFlag(res[i].baseInfo.byRPFlag);
                dto->setDestType(0);
				dto->setInsTime(QString(res[i].baseInfo.szInsDatetime));

				mDownloadedFavorites.append(QVariant::fromValue(dto));

				#if 0 // push one by one, even bad performance: 2020.3.10 TMCRSM-4652 different list sorting issue than server
				QVariantMap favorite;
				favorite[QStringLiteral("alias")]  = StringHelper::instance()->fromCp949(res[i].baseInfo.szCustName);
                favorite[SearchConstants::Dest]    = StringHelper::instance()->fromCp949(res[i].baseInfo.szCustName);
				favorite[CommonConstants::Address] = StringHelper::instance()->fromCp949(res[i].szAddress);
				favorite[CommonConstants::X]  = res[i].baseInfo.tp.x;
				favorite[CommonConstants::Y]  = res[i].baseInfo.tp.y;
				favorite[SearchConstants::PoiID]   = res[i].baseInfo.nPoiID;
				favorite[SearchConstants::NavSeq]  = QString(res[i].baseInfo.szNavSeq).toInt();
				favorite[SearchConstants::RpFlag]  = res[i].baseInfo.byRPFlag;
				favorite[SearchConstants::DestType] = 0;
				favorite[QStringLiteral("extrainfo")] = StringHelper::instance()->fromCp949(res[i].baseInfo.szAddInfo);
				favorite[QStringLiteral("instime")]   = QString(res[i].baseInfo.szInsDatetime);
				favorite[QStringLiteral("silence")]  = true; // don't display UI for error message
				mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::AddFavorite, favorite);
				Facade::instance()->notifyObservers(note);
				#endif
			}

            if (seq+cnt < total) {
                syncmore = true;
				requestRemoteFavorite(seq+cnt);
            }
		} else {
             qInfo() << QStringLiteral("pm, remote favorit failed: %1").arg(reply->errorCause());
		}
	} else {
        qInfo() << QStringLiteral("pm, remote favorit reply fail");
	}


	if (!syncmore && mDownloadedFavorites.size() > 0) {
		// #1 reverse list order
		// #2 push items number of list size - 1 and pass last item via notification in order to update common feature like updating property
		QList<FavoriteDtoPtr> dtolist;
        QVariantList::const_iterator it = mDownloadedFavorites.constEnd();
		--it;
		while (it != mDownloadedFavorites.constBegin()) {
            dtolist.append(FavoriteDtoPtr((*it).value<FavoriteDto*>()));
			--it;
		}
		if (dtolist.size() > 0) {
			auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
			proxy->addFavorite(dtolist);
		}

        // #3. save last item
        FavoriteDto* fav = (*it).value<FavoriteDto*>();
		QVariantMap favorite;
		favorite[SearchConstants::Dest] = fav->dest();
		favorite[QStringLiteral("alias")] = fav->alias();
		favorite[CommonConstants::Address] = fav->address();
		favorite[CommonConstants::X] = fav->mapX();
		favorite[CommonConstants::Y] = fav->mapY();
		favorite[SearchConstants::PoiID] = fav->poiID();
		favorite[SearchConstants::NavSeq] = fav->navSeq();;
		favorite[SearchConstants::RpFlag] = fav->rpFlag();
		favorite[SearchConstants::DestType] = 0;
        favorite[QStringLiteral("instime")] = fav->insTime();
		favorite[QStringLiteral("silence")] = true; // don't display UI for error message
		mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::AddFavorite, favorite);
		Facade::instance()->notifyObservers(note);

		mDownloadedFavorites.clear();
	}
    if (!syncmore) {
		if (success) {
			mRemoteSyncStatus  = mRemoteSyncStatus&(~R_SYNC_FAVORITE_FAIL); // reset fail history
            mRemoteSyncStatus |= R_SYNC_FAVORITE;
		}
		else {
			mRemoteSyncStatus = mRemoteSyncStatus&(~R_SYNC_FAVORITE); // reset favorit history
			mRemoteSyncStatus |= R_SYNC_FAVORITE_FAIL;
		}
	}

    int done = mRemoteSyncStatus&0x000f;
	int fail = (mRemoteSyncStatus&0x0f00)>>8;
	int all  = done|fail;

    if (mWaitingRemoteSync && (all&R_SYNC_ALL_DONE) == R_SYNC_ALL_DONE) {
        mWaitingRemoteSync = false;
        emit waitingSyncFinished();
    }
}

void ProfileManager::onResponsedRemoteRecentDestination()
{
    qDebug() << "pm, app remote responsed,  recent dest 1";
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
    bool syncmore = false;
	bool success = false;
	if (reply) {
		SearchRequestBasePtr req = reply->request();
        // Q_ASSERT(req.data());
        qDebug() << "pm, app remote responsed,  recent dest 1-1A";
		if (!reply->error()) {
			success = true;
			SearchRecentDestinationReplyInterface* searchReply = qobject_cast<SearchRecentDestinationReplyInterface*>(sender());
			int cnt = searchReply->resultCount();
			int total = searchReply->totalCount();
			int seq = req->startIndex();
			const TPOI_RECENTDESTINATION_INFO* res = searchReply->results();
            qDebug() << "pm, app remote responsed,  recent dest 1-1-1A, :"<<cnt<<"/"<<total;
			for (int i=0;i<cnt;i++) {
				RecentDestinationDto* dest = new RecentDestinationDto();
				dest->setDest(StringHelper::instance()->fromCp949(res[i].baseInfo.szName));
				dest->setAddress(StringHelper::instance()->fromCp949(res[i].szAddress));
                dest->setMapX(res[i].baseInfo.tp.x);
                dest->setMapY(res[i].baseInfo.tp.y);
				dest->setPoiID(res[i].baseInfo.nPoiID);
				dest->setNavSeq(QString(res[i].baseInfo.szNavSeq).toInt());
				dest->setRpFlag(res[i].baseInfo.byRPFlag);
				mDownloadedRecentDests.append(QVariant::fromValue(dest));
#if 0 // push one by one, even bad performance: 2020.3.10 TMCRSM-4652 different list sorting issue than server
				QVariantMap recent;
                recent[SearchConstants::Dest]    = StringHelper::instance()->fromCp949(res[i].baseInfo.szName);
				recent[CommonConstants::Address] = StringHelper::instance()->fromCp949(res[i].szAddress);
				recent[CommonConstants::X]  = res[i].baseInfo.tp.x;
				recent[CommonConstants::Y]  = res[i].baseInfo.tp.y;
				recent[SearchConstants::PoiID]   = res[i].baseInfo.nPoiID;
				recent[SearchConstants::NavSeq]  = QString(res[i].baseInfo.szNavSeq).toInt();
				recent[SearchConstants::RpFlag]  = res[i].baseInfo.byRPFlag;
				mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::AddRecentDestination, recent);
				Facade::instance()->notifyObservers(note);
#endif
			}

            if (seq+cnt < total) {
                syncmore = true;
				requestRemoteRecentDestination(seq+cnt);
            }
		} else {
            qInfo() << QStringLiteral("pm, get remote recent destination failed [%1]").arg(reply->errorCause());
		}
    }else {
        qInfo() << "pm, app remote reply fail";
    }

    // save
    if (!syncmore && mDownloadedRecentDests.size() > 0) {
		 // #1 reverse list order
		 // #2 push items number of list size - 1 and pass last item via notification in order to update common feature like updating property
         QList<RecentDestinationDtoPtr> dtolist;
         QVariantList::const_iterator it = mDownloadedRecentDests.constEnd();
         --it;
         while (it != mDownloadedRecentDests.constBegin()) {
             dtolist.append(RecentDestinationDtoPtr((*it).value<RecentDestinationDto*>()));
             --it;
		 }
		 if (dtolist.size() >0) {
			 auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
			 proxy->addRecentDestination(dtolist);
		 }

		 // #3 save last item
         RecentDestinationDto* dest = (*it).value<RecentDestinationDto*>();
		 QVariantMap recent;
		 recent[SearchConstants::Dest] = dest->dest();
		 recent[CommonConstants::Address] = dest->address();
		 recent[CommonConstants::X] = dest->mapX();
		 recent[CommonConstants::Y] = dest->mapY();
		 recent[SearchConstants::PoiID] = dest->poiID();
		 recent[SearchConstants::NavSeq] = dest->navSeq();
		 recent[SearchConstants::RpFlag] = dest->rpFlag();
		 mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::AddRecentDestination, recent);
		 Facade::instance()->notifyObservers(note);

		 mDownloadedRecentDests.clear();
	}
    if (!syncmore) {
		if (success) {
		   mRemoteSyncStatus  = mRemoteSyncStatus&(~R_SYNC_RECENTDEST_FAIL); // reset fail history
		   mRemoteSyncStatus |= R_SYNC_RECENTDEST;
		}
		else
		{
			mRemoteSyncStatus = mRemoteSyncStatus&(~R_SYNC_RECENTDEST); // reset recent dest history
			mRemoteSyncStatus |= R_SYNC_RECENTDEST_FAIL;
		}
		
	}

	int done = mRemoteSyncStatus&0x000f;
	int fail = (mRemoteSyncStatus&0x0f00)>>8;
	int all  = done|fail;

    if (mWaitingRemoteSync && (all&R_SYNC_ALL_DONE) == R_SYNC_ALL_DONE) {
        mWaitingRemoteSync = false;
        emit waitingSyncFinished();
    }
}


void ProfileManager::onResponsedRemoteSetting()
{

}

void ProfileManager::onResponsedRemoteRegisterSendToCar()
{
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
    sender()->deleteLater();

    QVariantMap resData;
    int cnt = 0, total = 0, seq = 0, errNo = 0;
    if (reply) {
        SearchRequestBasePtr req = reply->request();
        if (!reply->error()) {
            SearchSendToCarReplyInterface* s2cReply = qobject_cast<SearchSendToCarReplyInterface*>(sender());
            cnt = s2cReply->resultCount();
            total = s2cReply->totalCount();
            seq = req->startIndex();
            const TPOI_SENDTOCAR_INFO* res = s2cReply->results();
            if (res) {
                for (int i=0;i<cnt;i++) {
                    resData[CommonConstants::Alias]   = StringHelper::instance()->fromCp949(res[i].baseInfo.szCustName);
                    resData[SearchConstants::Dest]    = StringHelper::instance()->fromCp949(res[i].szPoiName);
                    resData[CommonConstants::Address] = StringHelper::instance()->fromCp949(res[i].szAddress);
                    resData[CommonConstants::X]  = res[i].baseInfo.tp.x;
                    resData[CommonConstants::Y]  = res[i].baseInfo.tp.y;
                    resData[SearchConstants::PoiID]   = res[i].baseInfo.nPoiID;
                    resData[SearchConstants::NavSeq]  = QString(res[i].baseInfo.szNavSeq).toInt();
                    resData[SearchConstants::RpFlag]  = res[i].baseInfo.byRPFlag;
                    resData[SearchConstants::DestType] = 0;
                    break;
                }
            }
        } else {
             errNo = reply->errorCause();
             T_DEBUG(QStringLiteral("get remote registerSendToCar failed: %1").arg(errNo) );
        }
    }
    resData[CommonConstants::Count]  =  cnt;
    resData[CommonConstants::Total]  =  total;
    resData[CommonConstants::Error]  =  seq;
    resData[CommonConstants::Index]  =  errNo;
    mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::ResponseRegisterSendToCar, resData);
    Facade::instance()->notifyObservers(note);
}

void ProfileManager::onResponsedRemoteFindSendToCar()
{
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
    sender()->deleteLater();

    QVariantMap resData;
    int cnt = 0, total = 0, seq = 0, errNo = 0;
    if (reply) {
         SearchRequestBasePtr req = reply->request();
        if (!reply->error()) {
            SearchSendToCarReplyInterface* s2cReply = qobject_cast<SearchSendToCarReplyInterface*>(sender());
            cnt = s2cReply->resultCount();
            total = s2cReply->totalCount();
            seq = req->startIndex();
            const TPOI_SENDTOCAR_INFO* res = s2cReply->results();
            if (res) {
                for (int i=0;i<cnt;i++) {
                    // 서버로 부터 전달 받은 POIID를 RSM에서 사용하는 POIID로 표준화 한다.
                    // 20.01.28 ymhong
                    auto poiId = res[i].baseInfo.nPoiID;
                    auto navSeq = QString(res[i].baseInfo.szNavSeq).toInt();
                    poiId = PoiHelper::fromServer(poiId, navSeq);

                    resData[CommonConstants::Alias]   = QByteArray(res[i].baseInfo.szCustName,strlen(res[i].baseInfo.szCustName));
                    resData[SearchConstants::Dest]    = QByteArray(res[i].szPoiName,strlen(res[i].szPoiName));
                    resData[CommonConstants::Address] = QByteArray(res[i].szAddress, strlen(res[i].szAddress));
                    resData[CommonConstants::X]  = res[i].baseInfo.tp.x;
                    resData[CommonConstants::Y]  = res[i].baseInfo.tp.y;
                    resData[SearchConstants::PoiID]   = poiId;
                    resData[SearchConstants::NavSeq]  = navSeq;
                    resData[SearchConstants::RpFlag]  = res[i].baseInfo.byRPFlag;
                    resData[SearchConstants::DestType] = 0;
                    break;
                }
            }
        } else {
             errNo = reply->errorCause();
             T_DEBUG(QStringLiteral("get remote findSendToCar failed: %1").arg(errNo) );
        }
    }    

    resData[CommonConstants::Count]  =  cnt;
    resData[CommonConstants::Total]  =  total;
    resData[CommonConstants::Error]  =  seq;
    resData[CommonConstants::Index]  =  errNo;
    mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::ResponseFindSendToCar, resData);
    Facade::instance()->notifyObservers(note);
}

void ProfileManager::onResponsedRemoteRemoveSendToCar()
{
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
    sender()->deleteLater();

    // 2019.11.19 lckxx
    // @leechunjae ResponseRemoveSendToCar 노티를 보내면 다시 RouteSearchMediator에서 Find Send 2 Car를 합닏
    return;

    QVariantMap resData;
    int cnt = 0, total = 0, seq = 0, errNo = 0;
    if (reply) {
         SearchRequestBasePtr req = reply->request();
        if (!reply->error()) {
            SearchSendToCarReplyInterface* s2cReply = qobject_cast<SearchSendToCarReplyInterface*>(sender());
            cnt = s2cReply->resultCount();
            total = s2cReply->totalCount();
            seq = req->startIndex();
            const TPOI_SENDTOCAR_INFO* res = s2cReply->results();
            if (res) {
                for (int i=0;i<cnt;i++) {
                    resData[CommonConstants::Alias]   = QByteArray(res[i].baseInfo.szCustName,strlen(res[i].baseInfo.szCustName));
                    resData[SearchConstants::Dest]    = QByteArray(res[i].szPoiName,strlen(res[i].szPoiName));
                    resData[CommonConstants::Address] = QByteArray(res[i].szAddress, strlen(res[i].szAddress));
                    resData[CommonConstants::X]  = res[i].baseInfo.tp.x;
                    resData[CommonConstants::Y]  = res[i].baseInfo.tp.y;
                    resData[SearchConstants::PoiID]   = res[i].baseInfo.nPoiID;
                    resData[SearchConstants::NavSeq]  = QString(res[i].baseInfo.szNavSeq).toInt();
                    resData[SearchConstants::RpFlag]  = res[i].baseInfo.byRPFlag;
                    resData[SearchConstants::DestType] = 0;
                    break;
                }
            }
        } else {
             errNo = reply->errorCause();
             T_DEBUG(QStringLiteral("get remote removeSendToCar failed: %1").arg(errNo) );
        }
    }
    resData[CommonConstants::Count]  =  cnt;
    resData[CommonConstants::Total]  =  total;
    resData[CommonConstants::Error]  =  seq;
    resData[CommonConstants::Index]  =  errNo;

    // 2019.11.19 lckxx
    // 아래 ResponseRemoveSendToCar noti 서브체인에서 다시 RequestFindSendToCar 를 호출 하는데 이 타이밍이 통신 상태에 따라 서버에서 Remove 하기 전에 다시
    // Send to car 정보를 보내주게 되어 이 시퀀스가 endless 상황에 빠지는 현상이 데스크탑에서는 빈번하게 발생.
    // 이로 인해 다른 동기화가 정상 동작 못하는 경우 발생. 현재 send to car 를 동기화 이후에 호출 하는 식으로 변경하고,
    // 동기화 중에는 remove 요청을 잠시 대기 시키도록 변경

    if (mWaitingRemoteSync) {
        connect(this, &ProfileManager::waitingSyncFinished, [this,resData]() {
            mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::ResponseRemoveSendToCar, resData);
            Facade::instance()->notifyObservers(note);
            qWarning() << "pm, remove send2car #1";
        });
    } else {
        mvc::INotificationPtr note = t_new_shared<Notification>(this, SearchConstants::ResponseRemoveSendToCar, resData);
        Facade::instance()->notifyObservers(note);
        qWarning() << "pm, remove send2car #2";
    }
}

/*!
 * called from UI
 * the current profile should be updated to the server
 */
void ProfileManager::onUserDataModified(int dataType, const QVariant& data)
{
    if (!mSessionManager->sessionKey().isEmpty())
        uploadUserData(dataType, data);
}


bool ProfileManager::createProfile(int profileId)
{
	QString profilePath = DatabaseHelper::instance()->getUserDBFilePath(profileId);
	if (QFile::exists(profilePath))
	{
		T_DEBUG(QStringLiteral("profile exists already: %1").arg(profileId));
		return false;
	}

	QSqlDatabase db = DatabaseHelper::instance()->connection(profileId);
	orm::TOrm::instance()->create<SettingDto>(db);
	orm::TOrm::instance()->create<RecentDestinationDto>(db);
	orm::TOrm::instance()->create<RecentSearchDto>(db);
	orm::TOrm::instance()->create<FavoriteDto>(db);
    orm::TOrm::instance()->create<LastRouteDto>(db);
    // 설정 DB에 맵뷰 카바타 셋업.
    mvc::INotificationPtr note = t_new_shared<Notification>(this, NavigationControllerConstants::UpdateCavatarIcon, QVariant());
    Facade::instance()->notifyObservers(note);

	return true;
}

class UploadWorker: public QObject
{
	Q_OBJECT
public:
     UploadWorker(ProfileManager* host,
                  UserProfileConstants::UserProfileDataUploadActionK type,
                  const QVariant& job,
                  bool waitingSync=false,
                  QObject* parent=0 ) :
      QObject(parent),
      mHost(host),
      mDataActionType(type),
      mJob(job),
      mT(0),
      mWaitingSync(waitingSync)
	 {
         // TMCRSM-4167: 서버 동기화 중에는 업로드 작업 대기
         if (waitingSync)
             connect(host, &ProfileManager::waitingSyncFinished, this, &UploadWorker::onWaitingSyncFinished);
	 }

	 void start() {
        QThread* t = new QThread;
        mT = t;
        moveToThread(t);
        connect(t, &QThread::started, this, &UploadWorker::lateWork);
        connect(this, &UploadWorker::finished, t, &QThread::quit);
        connect(t, &QThread::finished, this, &QObject::deleteLater);
        connect(t, &QThread::finished, t, &QObject::deleteLater);
        t->start();
	 }
public slots:
     // TMCRSM-4167: 서버 동기화 끝나면 업로드 시작
     void onWaitingSyncFinished() {
         start();
     }

     void lateWork() {
         work(mJob);
     }

    void work(const QVariant& job) {
	// 1. reset home / office
	// 2. regist home / office
    // 3. reset favorite route (needs spec) `
	// 4. regist favorite route (needs spec)
	// 5. preference
	// 6. POI Favorite List
	// 7. Recent Destination List
	// 8. Route Favorite list (not support)

        QString option;
    	QTextCodec* textcodec = QTextCodec::codecForName(QByteArray("windows-949"));
    	if (!textcodec)
    		textcodec = QTextCodec::codecForName(QByteArray("EUC-KR"));

    	switch (mDataActionType) {
    		case UserProfileConstants::RegistFavoriteK: {
    			TPOI_REGIST_FAVORITE* fav = new TPOI_REGIST_FAVORITE;
                QList<FavoriteDtoPtr> data = job.value<QList<FavoriteDtoPtr> >();

                fav->favorite.tp.x = data.at(0)->mapX().toInt();
                fav->favorite.tp.y = data.at(0)->mapY().toInt();
    			fav->favorite.nPoiID = data.at(0)->poiID().toInt();
    			fav->favorite.byRPFlag = (char)data.at(0)->rpFlag().toInt();
    			qstrncpy(fav->favorite.szCustName, textcodec->fromUnicode(data.at(0)->alias().toString()).constData(), D_MAXLEN_SISULNAME+1);
    			QString poiName = data.at(0)->dest().toString();
    			if (poiName.isEmpty())
    				poiName = data.at(0)->address().toString();

    			QString navseq = QString("%1").arg(data.at(0)->navSeq().toInt());
    			qstrncpy(fav->favorite.szCustName, textcodec->fromUnicode(poiName).constData(), D_MAXLEN_SISULNAME+1);
    			qstrncpy(fav->favorite.szNavSeq, navseq.toLatin1().constData(), 7);
				// copy additional info
				// copy original custom name
				// copy icon info

				RegistMultiFavoritesRequest* req = new RegistMultiFavoritesRequest(1); // regist only one favorite at once
				req->setItemData(fav);
				SearchRequestBasePtr hreq(req);
				SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

				if (reply) {
					connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onRegistMultiFavoritesResponsed()));
				}
			}
			break;
			case UserProfileConstants::RegistMultiFavoritesK:
			break;
			case UserProfileConstants::ModifyFavoriteK: {
				TPOI_MODIFY_FAVORITE* fav = new TPOI_MODIFY_FAVORITE;
                QList<FavoriteDtoPtr> data = job.value<QList<FavoriteDtoPtr>> ();
				if (data.count() < 2) break;
				FavoriteDtoPtr oldfav = data.at(0);
				FavoriteDtoPtr newfav = data.at(1);
                fav->favorite.tp.x = newfav->mapX().toInt();
                fav->favorite.tp.y = newfav->mapY().toInt();
                QString oldFavName, newFavName;
                oldFavName = oldfav->alias().toString();
                newFavName = newfav->alias().toString();
                if (oldFavName.isEmpty()) oldFavName = oldfav->dest().toString();
                if (newFavName.isEmpty()) newFavName = newfav->dest().toString();
                qstrncpy(fav->favorite.szCustName, textcodec->fromUnicode(oldFavName).constData(), D_MAXLEN_SISULNAME+1);
                qstrncpy(fav->favorite.szNewCustName, textcodec->fromUnicode(newFavName).constData(), D_MAXLEN_SISULNAME+1);
				// copy icon info
				ModifyFavoriteRequest* req = new ModifyFavoriteRequest;
				req->setItemData(fav);
				SearchRequestBasePtr hreq(req);
				SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

				if (reply) {
					connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onModifyMultiFavoritesResponsed()));
				}

			}
			break;
			case UserProfileConstants::RegistHomeOfficeK: {
			// PoiMyFavorite
			// home
			// 1. custom name
			// 2. coord X
			// 3. coord Y
			// 4. poi ID
			// 5. rpFlag
			// 6. additional info
			// 7. insert date
		     // office
			// 8. custom name
			// 9. coord X
			// 10. coord Y
			// 11. poi ID
			// 12. rpFlag
			// 13. additional info
			// 14. insert date

				option = QStringLiteral("MS");
				QList<FavoriteDtoPtr> homeoffice = job.value<QList<FavoriteDtoPtr> >();
				if (homeoffice.size() == 0) return;

				UploadAllFavoriteRequest* req = new UploadAllFavoriteRequest( 0,  option );
				TPOI_UPLOAD_HOMEOFFICE* uploadHomeOffice = req->homeOffice();
				FavoriteDtoPtr home = homeoffice.at(0);
				FavoriteDtoPtr office = homeoffice.at(1);
				if (!home.isNull()) {
					uploadHomeOffice->homeoffice.nHomePoiID = home->poiID().toInt();
                    uploadHomeOffice->homeoffice.Hometp.x   = home->mapX().toInt();
                    uploadHomeOffice->homeoffice.Hometp.y   = home->mapY().toInt();
                    uploadHomeOffice->homeoffice.byHomeRpFlag = (char)home->rpFlag().toInt();
					QString navseq = QString("%1").arg(home->navSeq().toInt());
                    QString homeName = home->alias().toString();
                    if (homeName.isEmpty()) homeName = home->dest().toString();
                    qstrncpy(uploadHomeOffice->homeoffice.szHomeCustName, textcodec->fromUnicode(homeName).constData(), D_MAXLEN_SISULNAME+1);
					qstrncpy(uploadHomeOffice->homeoffice.szHomeNavSeq, navseq.toLatin1().constData(), 7);
				}

				if (!office.isNull()) {
					uploadHomeOffice->homeoffice.nOfficePoiID = office->poiID().toInt();
                    uploadHomeOffice->homeoffice.Officetp.x   = office->mapX().toInt();
                    uploadHomeOffice->homeoffice.Officetp.y   = office->mapY().toInt();
                    uploadHomeOffice->homeoffice.byOfficeRpFlag = (char)office->rpFlag().toInt();
					QString navseq = QString("%1").arg(office->navSeq().toInt());
                    QString officeName = office->alias().toString();
                    if (officeName.isEmpty()) officeName = office->dest().toString();
                    qstrncpy(uploadHomeOffice->homeoffice.szOfficeCustName, textcodec->fromUnicode(officeName).constData(), D_MAXLEN_SISULNAME+1);
					qstrncpy(uploadHomeOffice->homeoffice.szOfficeNavSeq, navseq.toLatin1().constData(), 7);
				}
				SearchRequestBasePtr hreq(req);
				SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

				if (reply) {
					connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onRegistHomeOfficeResponsed()));
				}

			}
			break;
			case UserProfileConstants::ResetHomeOfficeK: {
				option = QStringLiteral("MC");
				UploadAllFavoriteRequest* req = new UploadAllFavoriteRequest( 0,  option );
				SearchRequestBasePtr hreq(req);
				SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

				if (reply) {
					connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onResetHomeOfficeResponsed()));
				}

			}
			break;
			case UserProfileConstants::ModifyMultiFavoritesK:
			break;
			case UserProfileConstants::MultiFavoritesK:
			option = QStringLiteral("FI");
			case UserProfileConstants::FavoritListAndResetHomeOfficeK:
			option = QStringLiteral("FI_MC");
			case UserProfileConstants::FavoritListAndRegistHomeOfficeK: {
				option = QStringLiteral("FI_MS");
			// TODO: upload POI favorite list
		    // PoiFavoritesInfo ( it needs to acquire poi information with the poi ID )
		    // 1. custom name
		    // 2. coord X
		    // 3. coord Y
		    // 4. poi ID
		    // 5. lcd Name
		    // 6. mcd Name
		    // 7. scd Name
		    // 8. repClsName
		    // 9. clsAName
		    // 10. clsBName
		    // 11. clsCName
		    // 12. clsDName
		    // 13. rpFlag
		    // 14. additional Info
		    // 15. insert date
				auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
				auto favorites = proxy->favoriteList(100,0,0);
				int cnt = favorites.count();
				if (cnt > 0) {
					UploadAllFavoriteRequest* req = new UploadAllFavoriteRequest( cnt,  option );
					TPOI_UPLOAD_HOMEOFFICE* uploadHomeOffice = req->homeOffice();
					TPOI_UPLOAD_FAVORITE* uploadFavorites = new TPOI_UPLOAD_FAVORITE[cnt];
					for (int i=0;i<cnt;i++) {
						FavoriteDtoPtr dto = favorites.at(i);
						int favType = dto->destType().toInt();
                        uploadFavorites[i].favorite.tp.x = dto->mapX().toInt();
                        uploadFavorites[i].favorite.tp.y = dto->mapY().toInt();
						uploadFavorites[i].favorite.nPoiID = dto->poiID().toInt();
						uploadFavorites[i].favorite.byRPFlag = dto->rpFlag().toInt();
                        QString favName = dto->alias().toString();
                        if (favName.isEmpty()) favName = dto->dest().toString();
                        qstrncpy(uploadFavorites[i].favorite.szCustName, textcodec->fromUnicode(favName).constData(), D_MAXLEN_SISULNAME+1);
						qstrncpy(uploadFavorites[i].favorite.szNavSeq, QString("%1").arg(dto->navSeq().toInt()).toLatin1().constData(),7);
						qstrncpy(uploadFavorites[i].favorite.szOrgCustName, textcodec->fromUnicode(dto->dest().toString()).constData(),D_MAXLEN_SISULNAME+1);
						qstrncpy(uploadFavorites[i].favorite.szInsDatetime, dto->insTime().toString().toLatin1().constData(),D_MAXLEN_SVCDATE+1);

						// home or office
						if ( favType == 2 ) {
							uploadHomeOffice->homeoffice.nHomePoiID = dto->poiID().toInt();
                            uploadHomeOffice->homeoffice.Hometp.x = dto->mapX().toInt();
                            uploadHomeOffice->homeoffice.Hometp.y = dto->mapY().toInt();
							uploadHomeOffice->homeoffice.byHomeRpFlag  = (char)dto->rpFlag().toInt();
							qstrncpy(uploadHomeOffice->homeoffice.szHomeNavSeq, QString("%1").arg(dto->navSeq().toInt()).toLatin1().constData(), 7);
                            qstrncpy(uploadHomeOffice->homeoffice.szHomeCustName, textcodec->fromUnicode(favName).constData(), D_MAXLEN_SISULNAME+1);
						}
						else if ( favType == 1) {
							uploadHomeOffice->homeoffice.nOfficePoiID = dto->poiID().toInt();
                            uploadHomeOffice->homeoffice.Officetp.x = dto->mapX().toInt();
                            uploadHomeOffice->homeoffice.Officetp.y = dto->mapY().toInt();
							uploadHomeOffice->homeoffice.byOfficeRpFlag  = (char)dto->rpFlag().toInt();
							qstrncpy(uploadHomeOffice->homeoffice.szOfficeNavSeq, QString("%1").arg(dto->navSeq().toInt()).toLatin1().constData(), 7);
                            qstrncpy(uploadHomeOffice->homeoffice.szOfficeCustName, textcodec->fromUnicode(favName).constData(), D_MAXLEN_SISULNAME+1);
						}
	             // todo: addInfo, iconInfo,
					}

					req->setItemData(uploadFavorites);
					SearchRequestBasePtr hreq(req);
					SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

					if (reply) {
						connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onUploadAllFavoriteResponsed()));
					}
				}
			}
			break;

			case UserProfileConstants::RemoveMultiFavoritesK: {
				auto favsDeleted = job.value< QList<FavoriteDtoPtr> > ();
				int cnt = favsDeleted.size();
				if (cnt > 0) {
					TPOI_REMOVE_FAVORITE* favs = new TPOI_REMOVE_FAVORITE[cnt];
					for (int i=0;i<cnt;i++) {
                        QString favName = favsDeleted.at(i)->alias().toString();
                        if (favName.isEmpty()) favName = favsDeleted.at(i)->dest().toString();
                        qstrncpy(favs[i].favorite.szCustName, textcodec->fromUnicode(favName).constData(), D_MAXLEN_SISULNAME+1);
                        favs[i].favorite.tp.x = favsDeleted.at(i)->mapX().toInt();
                        favs[i].favorite.tp.y = favsDeleted.at(i)->mapY().toInt();
					}

					RemoveMultiFavoritesRequest* req = new RemoveMultiFavoritesRequest(cnt);
					req->setItemData(favs);
					SearchRequestBasePtr hreq(req);
					SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

					if (reply) {
						connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onRemoveMultiFavoritesResponsed()));
					}
				}
			}
			break;
			case UserProfileConstants::ClearFavoritesK: {
				// TODO:

			}
			break;
			case UserProfileConstants::MultiRecentDestsK: {
					// PoiRecentUploadsInfo ( it needs to acquire poi information with the poi ID )
			// 1. name
			// 2. coord X
			// 3. coord Y
			// 4. poi ID
			// 5. rpFlag
			// 6. totalCnt
	        // 7. service date
                QList<RecentDestinationDtoPtr> dests = job.value<QList<RecentDestinationDtoPtr> >();
                if (mWaitingSync) {
                    auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
                    dests = proxy->recentDestListBySortType(TMAP_UI_RECENTDESTINATION_MAX);
                }
                else
                    dests = job.value<QList<RecentDestinationDtoPtr> >();

				int cnt = dests.count();
				TPOI_UPLOAD_RECENTDESTINATION* uploadRecents = new TPOI_UPLOAD_RECENTDESTINATION[cnt];
				memset(uploadRecents, 0, sizeof(TPOI_UPLOAD_RECENTDESTINATION) * cnt);
				for (int i=0; i<cnt; i++) {
				RecentDestinationDtoPtr dto = dests.at(i);
				QString poiName = dto->dest().toString();
				QString address = dto->address().toString();
                uploadRecents[i].recentdest.tp.x = dto->mapX().toInt();
                uploadRecents[i].recentdest.tp.y = dto->mapY().toInt();
				uploadRecents[i].recentdest.nPoiID = dto->poiID().toInt();
				uploadRecents[i].recentdest.byRPFlag = dto->rpFlag().toInt();
				int navseq = dto->navSeq().toInt();

				if (!poiName.isEmpty())
					qstrncpy(uploadRecents->recentdest.szName, textcodec->fromUnicode(poiName).constData(), D_MAXLEN_SISULNAME+1);
				else if (!address.isEmpty())
					qstrncpy(uploadRecents->recentdest.szName, textcodec->fromUnicode(address).constData(), D_MAXLEN_SISULNAME+1);

				qstrncpy(uploadRecents->recentdest.szNavseq, QString("%1").arg(navseq).toLatin1().constData(),7);
	             // todo: addInfo, iconInfo,
			}

				UploadAllRecentDestinationRequest* req = new UploadAllRecentDestinationRequest( cnt );
				req->setItemData(uploadRecents);
				SearchRequestBasePtr hreq(req);
				SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

				if (reply) {
					connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onUploadAllRecentDestinationResponsed()));
				}
			}
			break;

			case UserProfileConstants::RemoveMultiRecentDestsK: {
				auto destsDeleted = job.value<QList<RecentDestinationDtoPtr>>();
				int cnt = destsDeleted.size();
				if (cnt > 0) {
					TPOI_REMOVE_RECENTDESTINATION* dests = new TPOI_REMOVE_RECENTDESTINATION[cnt];
					for (int i=0;i<cnt;i++) {
						qstrncpy(dests[i].recentdest.szName, textcodec->fromUnicode(destsDeleted.at(i)->dest().toString()).constData(), D_MAXLEN_SISULNAME+1);
                        dests[i].recentdest.tp.x = destsDeleted.at(i)->mapX().toInt();
                        dests[i].recentdest.tp.y = destsDeleted.at(i)->mapY().toInt();
					}

					RemoveMultiRecentDestinationsRequest* req = new RemoveMultiRecentDestinationsRequest(cnt);
					req->setItemData(dests);
					SearchRequestBasePtr hreq(req);
					SearchReplyBaseInterface* reply = SearchHelper::instance()->request(hreq);

					if (reply) {
						connect(reply->qobject(), SIGNAL(finished()), mHost, SLOT(onRemoveMultiRecentDestinationsResponsed()));
					}
				}
			}
			break;
			case UserProfileConstants::PreferenceK:
			break;
			default: break;
		}

		emit finished();
	}


signals:
    void finished();

	private:
		ProfileManager* mHost;
		UserProfileConstants::UserProfileDataUploadActionK mDataActionType;
        QVariant mJob;
		QThread* mT;
        bool     mWaitingSync;
};

void ProfileManager::uploadUserData(int dataType, const QVariant& data)
{
    UploadWorker* worker = new UploadWorker(this,
                                            (UserProfileConstants::UserProfileDataUploadActionK)dataType,
                                            data,
                                            mWaitingRemoteSync);
    // TMCRSM-4167: 서버 동기화 중에는 업로드 작업 대기
    if (!mWaitingRemoteSync)
       worker->start();
//    QMetaObject::invokeMethod(worker, "work", Qt::QueuedConnection, Q_ARG(QVariant,data));
}


/** !! DONOT USE
 * @brief ProfileManager::syncUserData
 * @param userData
 */
void ProfileManager::syncUserData(const QVariant& userData)
{
	// 1. Home/Office
	// 2. Favorite Route
	// 3. Recent Destination List
	// 4. POI Favorite List
	// 5. Route Favorite List (not supported)

	auto proxy = Facade::instance()->retrieveProxy<NavibookmarkAgentProxy>();
	proxy->clearFavorites();
	proxy->clearRecentDestination();
	// TODO: copy new data into the local db
}


void ProfileManager::onUploadAllFavoriteResponsed()
{
          SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
         sender()->deleteLater();
         if (reply) {

         }
}

void ProfileManager::onUploadAllRecentDestinationResponsed()
{
  SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
     sender()->deleteLater();
     if (reply) {

     }
}

void ProfileManager::onRegistMultiFavoritesResponsed()
{
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
	if (reply) {

	}	
}

void ProfileManager::onRegistHomeOfficeResponsed() {
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
	if (reply) {

	}
}

void ProfileManager::onResetHomeOfficeResponsed()
{
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
	if (reply) {

	}
}

void ProfileManager::onModifyMultiFavoritesResponsed()
{
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
	if (reply) {

	}
}

void ProfileManager::onRemoveMultiFavoritesResponsed()
{
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
	if (reply) {

	}
}

void ProfileManager::onRemoveMultiRecentDestinationsResponsed()
{
	SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(sender());
	sender()->deleteLater();
	if (reply) {

	}
}

void ProfileManager::onOnlineStateChanged(bool online)
{
    qDebug() << "pm, app online changed to " << online;
    // close using profile db
    if (online) DatabaseHelper::instance()->disconnect(mActiveProfileId);
    else  DatabaseHelper::instance()->disconnectForCloud();

  if (online) {
      if (!mSessionManager->sessionKey().isEmpty()) {
          qDebug() << "pm, app ak : " << mSessionManager->sessionKey();
          // change user db to cloud
          qDebug() << "[OTP] connection for cloud !!!";


          QSqlDatabase db = DatabaseHelper::instance()->connectionForCloud();
          orm::TOrm::instance()->create<SettingDto>(db);
          orm::TOrm::instance()->create<RecentDestinationDto>(db);
          orm::TOrm::instance()->create<RecentSearchDto>(db);
          orm::TOrm::instance()->create<FavoriteDto>(db);

          // 서버 동기화 DB에 맵뷰 카바타 셋업.
          mvc::INotificationPtr note = t_new_shared<Notification>(this, NavigationControllerConstants::UpdateCavatarIcon, QVariant());
          Facade::instance()->notifyObservers(note);

          if (mChannelServerPolicySuccess) {
              qDebug() << "[OTP] ProfileManager. create cloud DB";
              // do synchronize with user's data on cloud
			  onRequestUserDataDownload();
          }
          else {
              mNeedRemoteSync = true;
          }
      }
  }
}

} /* namespace SKT */

#include "ProfileManager.moc"
