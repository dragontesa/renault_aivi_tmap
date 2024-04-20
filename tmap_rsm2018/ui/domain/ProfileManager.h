/*
 * ProfileManager.h
 */

#ifndef UI_DOMAIN_PROFILEMANAGER_H_
#define UI_DOMAIN_PROFILEMANAGER_H_

#define TMAP_UI_FAVORITE_MAX 100//250 (UID 1.68)
#define TMAP_UI_RECENTDESTINATION_MAX 100 // 250: UX 1.1
#define TMAP_UI_RECENTSEARCH_MAX 100

namespace SKT
{

class SessionManagerInterface;
class ProfileManager: public QObject
{
	Q_OBJECT
private:
    ProfileManager();
	public:
	static ProfileManager* instance();
	void initProfileManager(QObject* owner, int initProfileId=0, QObject* parent=0);

    int activeUserProfileId() const;
    int lastUserProfileId() const;
    QString activeUserProfileName() const;
    QString lastUserProfileName() const;


	public slots:
	// called from requester
    void onChangeProfileName(const QString& newName);
	void onResetUserData(int profileId);
	void onChangeUser(int profileId);
	void onCopyUserData(int srcId, int dstId);
	void onUserProfileCreated(int id);
	void onUserProfileDownloaded(int profileId, const QVariant& data);
    void onUserDataDownloaded(const QVariant& data);
    void onRequestUserDataDownload();
    void onUserDataDownloadDone();

	// called from ui
	void onUserDataModified(int dataType, const QVariant& data);

    // called session manager
	void onSessionChanged(int ownerId);

    // called navigation control proxy
    void onOnlineStateChanged(bool online);

	signals:
	void profileUpdated(); // 현재 사용중인 프로파일 변경 메세지
	void profileChanged(); // 프로파일 사용자 변경에 대한 메세지

	private:
	bool createProfile(int profileId);
    void syncUserData(const QVariant& userData);
    void uploadUserData(int dataType, const QVariant& data);
	void requestMyHomeOfficeFavorite();
	void requestRemoteFavorite(int seq=0);
	void requestRemoteRecentDestination(int seq=0);
	void requestRemoteSetting();

public:
    void requestRemoteRegisterSendToCar(const QVariantMap &reqData, int seq=0);
    void requestRemoteFindSendToCar(int seq=0);
    void requestRemoteRemoveSendToCar(const QVariantMap &reqData, int seq=0);

private slots:
    void onChannelServerPolicyFinished(bool success);
	void onResponsedMyHomeOfficeFavorite();
	void onResponsedRemoteFavorite();
	void onResponsedRemoteRecentDestination();
	void onResponsedRemoteSetting();
    void onResponsedRemoteRegisterSendToCar();
    void onResponsedRemoteFindSendToCar();
    void onResponsedRemoteRemoveSendToCar();

    void onUploadAllFavoriteResponsed();
    void onUploadAllRecentDestinationResponsed();
    void onRegistMultiFavoritesResponsed();
    void onRegistHomeOfficeResponsed();
    void onResetHomeOfficeResponsed();
    void onModifyMultiFavoritesResponsed();
    void onRemoveMultiFavoritesResponsed();
    void onRemoveMultiRecentDestinationsResponsed();

signals:
    void waitingSyncFinished();

private:
    int mActiveProfileId;
    int mLastProfileId;
    bool mChannelServerPolicySuccess;
    bool mNeedRemoteSync;
    bool mWaitingRemoteSync;  // TMCRSM-4167: 서버 동기화 중에는 업로드 작업 대기
    enum REMOTE_SYNC_STATUS {
        R_SYNC_NONE            = 0x0000,
        R_SYNC_HOMEOFFICE      = 0x0001,
        R_SYNC_FAVORITE        = 0x0002,
        R_SYNC_RECENTDEST      = 0x0004,
        R_SYNC_FAVORITE_FAIL   = 0x0100,
        R_SYNC_HOMEOFFICE_FAIL = 0x0200,
        R_SYNC_RECENTDEST_FAIL = 0x0400,
        R_SYNC_ALL_DONE        = 0x0007,
    };

    int  mRemoteSyncStatus;
    QString mActiveProfileName;
    QString mLastProfileName;
	SessionManagerInterface* mSessionManager;
    QVariantList mDownloadedRecentDests;
    QVariantList mDownloadedFavorites;
};

} /* namespace SKT */

#endif /* UI_DOMAIN_PROFILEMANAGER_H_ */
