#include "Constants.h"

using namespace SKT;

/*!
    \enum Constants::NotificationK

    notification 그룹 id와 공통 notification 기술

    \value CreateView view 생성
    url: qml 소스 위치
    parent: parent object
    parentObjectName: parent의 objectName
    동일 view에 대한 중복 요청시 건너 뜀
    parentObjectName이 rootWindow인 경우 ApplicationWindow로 지정 됨
    parent가 지정되지 않은 경우 contentRoot가 parent가 됨
    인자에 visible false가 지정되어 있지 않으면 기본적으로 show 메소드 호출
    기타 추가 인자는 이전 방식처럼 전달 가능
    data['url'] = ...
    data['parent'] = ...
    data['caller'] = ...

    \value ShowView objectName으로 view를 찾고 View/PopView의 show 메소드 호출

    \value HideView objectName으로 view를 찾고 visible property를 false로 설정

    \value CloseView view의 View/PopView의 close 메소드 호출
*/

Constants::Constants()
{

}

QString Constants::enumToString( const char *enumerator, int value )
{
    QString ret;

    auto index = Constants::staticMetaObject.indexOfEnumerator( enumerator );
    ret = Constants::staticMetaObject.enumerator(index).valueToKey( value );

    return ret;
}

int Constants::stringToEnum( const char *enumerator, const QString& value )
{
    int ret;

    auto index = Constants::staticMetaObject.indexOfEnumerator( enumerator );
    ret = Constants::staticMetaObject.enumerator(index).keyToValue(value.T_CSTR());

    return ret;
}
