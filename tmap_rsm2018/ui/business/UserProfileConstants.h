/*
 * UserProfileConstants.h
 *
 *  Created on: 2017. 10. 10.
 */

#ifndef UI_BUSINESS_USERPROFILECONSTANTS_H_
#define UI_BUSINESS_USERPROFILECONSTANTS_H_


namespace SKT {

class UserProfileConstants {
public:
	static const QString FavoriteHome; // 집으로
	static const QString FavoriteOffice; // 회사로
	static const QString FavoriteRoute; // 자주가는 경로
	static const QString FavoriteList; // 즐겨찾기 POI 목록
	static const QString FavoriteRouteList; // 즐겨찾기 경로 목록(Not support )
	static const QString RecentDestList; // 최근 목적지 POI 목록(최근길 목록)
	static const QString RecentRouteList; // 최근 경로 목록 (Not support)
	static const QString Preference; // 환경설정
	static const QString ResetHomeOffice; // 집으로/회사로 초기화 (업로드)
	static const QString RegistHomeOffice; // 집으로/회사로 설정 (업로드)
	static const QString ResetFavoriteRoute; // 자주가는 경로 초기화 (업로드)
	static const QString RegistFavoriteRoute; // 자주가는 경로 설정 (업로드)
	static const QString FavoritListAndResetHomeOffice; // POI 즐겨찾기 목록 (업로드) + 집으로회사로 초기화
	static const QString FavoritListAndRegistHomeOffice; // POI 즐겨찾기 목록 (업로드) + 집으로회사로 설정
	static const QString FavoritRouteListAndResetFavoriteRoute; // 경로 즐겨찾기 목록 (업로드) + 자주가는 경로 초기화 [ Not support ]
	static const QString FavoritRouteListAndRegistFavoriteRoute; // 경로 즐겨찾기 목록 (업로드) + 자주가는 경로  설정 [ Not support ]



	enum UserProfileDataUploadActionK {
		FavoriteHomeK = 0x0001, // upload the home favorite 
		FavoriteOfficeK, // upload the office favorite
		FavoriteRouteK, // upload a favorite route  [not supported]
		RegistFavoriteK, // regist a favorite
		RegistMultiFavoritesK, // regist multi favorites, QA: what's the difference with uploading all favorites?
		ModifyFavoriteK, // modify a favorite
		ModifyMultiFavoritesK,
		MultiFavoritesK, // upload multi favorites
		FavoriteAllRoutesK, // get all favorite routes [not supported]
		MultiRecentDestsK, // upload multi recent destinations
		RecentRouteListK, // upload all recent route lists [not supported]
		PreferenceK, // upload user setting 
		ResetHomeOfficeK, // reset both home and office
		RegistHomeOfficeK, // regist both home and office
		RemoveMultiFavoritesK, // delete multi favorites
		ClearFavoritesK, // clear all favorites
		ResetFavoriteRouteK, // reset a favorite route [not supported]
		RegistFavoriteRouteK, // regist a favorite route [not supported]
		FavoritListAndResetHomeOfficeK, //  upload all favorite lists and reset home or office
		FavoritListAndRegistHomeOfficeK, // upload all favorites and regist home or office
		FavoritRouteListAndResetFavoriteRouteK, // upload all favorite routes and reset a favorite route [not supported]
        FavoritRouteListAndRegistFavoriteRouteK, // upload all favorite and regist a favorite route [not supported]
        RemoveMultiRecentDestsK, // deletes multi recent destinations
        ClearRecentDestinationsK, // clear all recent destinations


    };

};

}

#endif /* UI_BUSINESS_USERPROFILECONSTANTS_H_ */
