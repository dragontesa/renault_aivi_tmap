#ifndef COMMONCONSTANTS_H
#define COMMONCONSTANTS_H

#include <QString>
#include <QObject>
#include "ToastsConstants.h"

namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT CommonConstants : public QObject
{
    Q_OBJECT

    Q_ENUMS( ToastK)
    Q_ENUMS( BoolK )
    Q_ENUMS( IntK )
    Q_ENUMS( ResultViewListK )
    Q_ENUMS( MaxK )

    Q_PROPERTY(QString ObjectName                   MEMBER  ObjectName               CONSTANT)
    Q_PROPERTY(QString Parent                       MEMBER  Parent                   CONSTANT)
    Q_PROPERTY(QString ParentObjectName             MEMBER  ParentObjectName         CONSTANT)
    Q_PROPERTY(QString Name                         MEMBER  Name                     CONSTANT)
    Q_PROPERTY(QString Url                          MEMBER  Url                      CONSTANT)
    Q_PROPERTY(QString Caller                       MEMBER  Caller                   CONSTANT)
    Q_PROPERTY(QString Owner                        MEMBER  Owner                    CONSTANT)
    Q_PROPERTY(QString Relay                        MEMBER  Relay                    CONSTANT)
    Q_PROPERTY(QString X                            MEMBER  X                        CONSTANT)
    Q_PROPERTY(QString Y                            MEMBER  Y                        CONSTANT)
    Q_PROPERTY(QString Z                            MEMBER  Z                        CONSTANT)
    Q_PROPERTY(QString Width                        MEMBER  Width                    CONSTANT)
    Q_PROPERTY(QString Height                       MEMBER  Height                   CONSTANT)
    Q_PROPERTY(QString Modal                        MEMBER  Modal                    CONSTANT)
    Q_PROPERTY(QString Empty                        MEMBER  Empty                    CONSTANT)
    Q_PROPERTY(QString Properties                   MEMBER  Properties               CONSTANT)
    Q_PROPERTY(QString W                            MEMBER  W                        CONSTANT)
    Q_PROPERTY(QString H                            MEMBER  H                        CONSTANT)
    Q_PROPERTY(QString Visible                      MEMBER  Visible                  CONSTANT)
    Q_PROPERTY(QString Mode                         MEMBER  Mode                     CONSTANT)
    Q_PROPERTY(QString Object                       MEMBER  Object                   CONSTANT)
    Q_PROPERTY(QString Index                        MEMBER  Index                    CONSTANT)
    Q_PROPERTY(QString Arg                          MEMBER  Arg                      CONSTANT)
    Q_PROPERTY(QString Image                        MEMBER  Image                    CONSTANT)
    Q_PROPERTY(QString Up                           MEMBER  Up                       CONSTANT)
    Q_PROPERTY(QString Down                         MEMBER  Down                     CONSTANT)
    Q_PROPERTY(QString Normal                       MEMBER  Normal                   CONSTANT)
    Q_PROPERTY(QString Press                        MEMBER  Press                    CONSTANT)
    Q_PROPERTY(QString Pressed                      MEMBER  Pressed                  CONSTANT)
    Q_PROPERTY(QString Focus                        MEMBER  Focus                    CONSTANT)
    Q_PROPERTY(QString Hover                        MEMBER  Hover                    CONSTANT)
    Q_PROPERTY(QString Select                       MEMBER  Select                   CONSTANT)
    Q_PROPERTY(QString Transparent                  MEMBER  Transparent              CONSTANT)
    Q_PROPERTY(QString Undefined                    MEMBER  Undefined                CONSTANT)
    Q_PROPERTY(QString Gray                         MEMBER  Gray                     CONSTANT)
    Q_PROPERTY(QString White                        MEMBER  White                    CONSTANT)
    Q_PROPERTY(QString Black                        MEMBER  Black                    CONSTANT)
    Q_PROPERTY(QString Blue                         MEMBER  Blue                     CONSTANT)
    Q_PROPERTY(QString Green                        MEMBER  Green                    CONSTANT)
    Q_PROPERTY(QString Red                          MEMBER  Red                      CONSTANT)
    Q_PROPERTY(QString Dark                         MEMBER  Dark                     CONSTANT)
    Q_PROPERTY(QString SharpBlack                   MEMBER  SharpBlack               CONSTANT)
    Q_PROPERTY(QString SharpWhite                   MEMBER  SharpWhite               CONSTANT)
    Q_PROPERTY(QString Total                        MEMBER  Total                    CONSTANT)
    Q_PROPERTY(QString Error                        MEMBER  Error                    CONSTANT)
    Q_PROPERTY(QString ButtonText                   MEMBER  ButtonText               CONSTANT)
    Q_PROPERTY(QString Var                          MEMBER  Var                      CONSTANT)
    Q_PROPERTY(QString DontUseEditBox               MEMBER  DontUseEditBox           CONSTANT)
    Q_PROPERTY(QString System                       MEMBER  System                   CONSTANT)
    Q_PROPERTY(QString ReFocus                      MEMBER  ReFocus                  CONSTANT)
    Q_PROPERTY(QString IsClosed                     MEMBER  IsClosed                 CONSTANT)
    Q_PROPERTY(QString Show                         MEMBER  Show                     CONSTANT)
    Q_PROPERTY(QString PullUp                       MEMBER  PullUp                   CONSTANT)
    Q_PROPERTY(QString MessageType                  MEMBER  MessageType              CONSTANT)
    Q_PROPERTY(QString Active                       MEMBER  Active                   CONSTANT)


    //setting
    Q_PROPERTY(QString Section                      MEMBER  Section                  CONSTANT)
    Q_PROPERTY(QString Key                          MEMBER  Key                      CONSTANT)
    Q_PROPERTY(QString Value                        MEMBER  Value                    CONSTANT)
    Q_PROPERTY(QString Setting                      MEMBER  Setting                  CONSTANT)
    Q_PROPERTY(QString Settings                     MEMBER  Settings                 CONSTANT)
    Q_PROPERTY(QString Type                         MEMBER  Type                     CONSTANT)
    Q_PROPERTY(QString NoMapData                    MEMBER  NoMapData                CONSTANT)
    Q_PROPERTY(QString IncompleteMapData            MEMBER  IncompleteMapData        CONSTANT)
    Q_PROPERTY(QString IncompatibleMapData          MEMBER  IncompatibleMapData      CONSTANT)
    Q_PROPERTY(QString ErrorMapData                 MEMBER  ErrorMapData             CONSTANT)
    Q_PROPERTY(QString ResponseNotificationId       MEMBER  ResponseNotificationId   CONSTANT)
    Q_PROPERTY(QString RouteSearch                  MEMBER  RouteSearch              CONSTANT)


    //search
    Q_PROPERTY(QString Command                      MEMBER  Command                   CONSTANT)
    Q_PROPERTY(QString RequestCommandPtr            MEMBER  RequestCommandPtr         CONSTANT)
    Q_PROPERTY(QString Text                         MEMBER  Text                      CONSTANT)
    Q_PROPERTY(QString ViewId                       MEMBER  ViewId                    CONSTANT)
    Q_PROPERTY(QString Address                      MEMBER  Address                   CONSTANT)
    Q_PROPERTY(QString Industry                     MEMBER  Industry                  CONSTANT)
    Q_PROPERTY(QString Around                       MEMBER  Around                    CONSTANT)
    Q_PROPERTY(QString Detail                       MEMBER  Detail                    CONSTANT)
    Q_PROPERTY(QString EvDetail                     MEMBER  EvDetail                  CONSTANT)
    Q_PROPERTY(QString Lowest                       MEMBER  Lowest                    CONSTANT)
    Q_PROPERTY(QString BookmarkType                 MEMBER  BookmarkType              CONSTANT)
    Q_PROPERTY(QString Favorite                     MEMBER  Favorite                  CONSTANT)
	Q_PROPERTY(QString RecentDestination            MEMBER  RecentDestination         CONSTANT)
    Q_PROPERTY(QString Level                        MEMBER  Level                     CONSTANT)
    Q_PROPERTY(QString SubLevel                     MEMBER  SubLevel                  CONSTANT)
    Q_PROPERTY(QString Slot                         MEMBER  Slot                      CONSTANT)
    Q_PROPERTY(QString Keyword                      MEMBER  Keyword                   CONSTANT)
    Q_PROPERTY(QString Max                          MEMBER  Max                       CONSTANT)
    Q_PROPERTY(QString Offset                       MEMBER  Offset                    CONSTANT)
    Q_PROPERTY(QString Alias                        MEMBER  Alias                     CONSTANT)
    Q_PROPERTY(QString Count                        MEMBER  Count                     CONSTANT)
    Q_PROPERTY(QString FocusIndex                   MEMBER  FocusIndex                CONSTANT)
    Q_PROPERTY(QString Kind                         MEMBER  Kind                      CONSTANT)
    Q_PROPERTY(QString From                         MEMBER  From                      CONSTANT)
    Q_PROPERTY(QString To                           MEMBER  To                        CONSTANT)
    Q_PROPERTY(QString T                            MEMBER  T                         CONSTANT)
    Q_PROPERTY(QString B                            MEMBER  B                         CONSTANT)
    Q_PROPERTY(QString L                            MEMBER  L                         CONSTANT)
    Q_PROPERTY(QString R                            MEMBER  R                         CONSTANT)
    Q_PROPERTY(QString Fuel                         MEMBER  Fuel                      CONSTANT)
    Q_PROPERTY(QString Code                         MEMBER  Code                      CONSTANT)
    Q_PROPERTY(QString Request                      MEMBER  Request                   CONSTANT)
    Q_PROPERTY(QString Response                     MEMBER  Response                  CONSTANT)
    Q_PROPERTY(QString Result                       MEMBER  Result                    CONSTANT)
    Q_PROPERTY(QString Notification                 MEMBER  Notification              CONSTANT)
    Q_PROPERTY(QString Sync                         MEMBER  Sync                      CONSTANT)

    //toast
    Q_PROPERTY(QString Position                     MEMBER  Position                  CONSTANT)
    Q_PROPERTY(QString Duration                     MEMBER  Duration                  CONSTANT)
    Q_PROPERTY(QString Message                      MEMBER  Message                   CONSTANT) //Dialog에서도 사용

    //keyboard
    Q_PROPERTY(QString SearchBtnAutoEnable          MEMBER  SearchBtnAutoEnable       CONSTANT)
    Q_PROPERTY(QString SearchBtnAlwaysEnable        MEMBER  SearchBtnAlwaysEnable     CONSTANT)


    //soft key
    Q_PROPERTY(QString NoOption                     MEMBER  NoOption                  CONSTANT)

    //Application
    Q_PROPERTY(QString ContentRoot                  MEMBER  ContentRoot               CONSTANT)
    Q_PROPERTY(QString ContentWrap                  MEMBER  ContentWrap               CONSTANT)
    Q_PROPERTY(QString ZoneWrap                     MEMBER  ZoneWrap                  CONSTANT)
    Q_PROPERTY(QString MapViewWrap                  MEMBER  MapViewWrap               CONSTANT)
    Q_PROPERTY(QString ToastWrap                    MEMBER  ToastWrap                 CONSTANT)
    Q_PROPERTY(QString AlertWrap                    MEMBER  AlertWrap                 CONSTANT)
    Q_PROPERTY(QString ProgressWrap                 MEMBER  ProgressWrap              CONSTANT)
    Q_PROPERTY(QString WidgetWrap                   MEMBER  WidgetWrap                CONSTANT)
    Q_PROPERTY(QString TestWrap                     MEMBER  TestWrap                  CONSTANT)
    Q_PROPERTY(QString ContentRootWithoutSoftkey    MEMBER  ContentRootWithoutSoftkey CONSTANT)
    Q_PROPERTY(QString ContentRootForCommonDialog   MEMBER  ContentRootForCommonDialog CONSTANT)


    //Dialog
    Q_PROPERTY(QString ContentWidth                 MEMBER  ContentWidth              CONSTANT)
    Q_PROPERTY(QString ContentHeight                MEMBER  ContentHeight             CONSTANT)
    Q_PROPERTY(QString Title                        MEMBER  Title                     CONSTANT)
    Q_PROPERTY(QString Ok                           MEMBER  Ok                        CONSTANT)
    Q_PROPERTY(QString Cancel                       MEMBER  Cancel                    CONSTANT)
    Q_PROPERTY(QString Menus                        MEMBER  Menus                     CONSTANT)
    Q_PROPERTY(QString Message2                     MEMBER  Message2                  CONSTANT)
    Q_PROPERTY(QString TypeA                        MEMBER  TypeA                     CONSTANT)
    Q_PROPERTY(QString TypeB                        MEMBER  TypeB                     CONSTANT)
    Q_PROPERTY(QString TypeC                        MEMBER  TypeC                     CONSTANT)

    //Lang
    Q_PROPERTY(QString Lang                         MEMBER  Lang                      CONSTANT)
    Q_PROPERTY(QString Ko                           MEMBER  Ko                        CONSTANT)
    Q_PROPERTY(QString En                           MEMBER  En                        CONSTANT)


public:
    Q_INVOKABLE QString objectName( int viewId );
    Q_INVOKABLE QString url( int viewId );    
    Q_INVOKABLE bool target();
    Q_INVOKABLE QString getElideText(QVariant fontName, int fontPixelSize, int lineWidth, QVariant string);
    CommonConstants();

    enum BoolK {
        False,
        True,
    };

    enum IntK  {
        MinusOne    = -1,
        Zero        = 0 ,
        One             ,
        Two             ,
        Three           ,
        Four            ,
        Five            ,
        Six             ,
        Seven           ,
        Eight           ,
        Nine            ,
        Ten             ,
    };

    enum ToastK {
        ToastUp = ToastsConstants::ToastUp,
        ToastCenter = ToastsConstants::ToastCenter,
        ToastDown = ToastsConstants::ToastDown
    };


    enum ResultViewListK {
        ResultViewId,
        ResultViewName,
        ResultViewVisible,
        ResultViewActive,
    };

    enum MaxK {
        MAX_INT_VALUE     = std::numeric_limits<int>::max()
    };

    static CommonConstants* instance() {
        static CommonConstants Instance;
        return &Instance;
    }

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new CommonConstants;
    }

    static void resultViewInit(QVariantList &result);

    static const QString ObjectName;
    static const QString Parent;
    static const QString ParentObjectName;
    static const QString Name;
    static const QString Url;
    static const QString Caller;
    static const QString Owner;
    static const QString Relay;
    static const QString X;
    static const QString Y;
    static const QString Z;
    static const QString Width;
    static const QString Height;
    static const QString Modal;
    static const QString Empty;
    static const QString Properties;
    static const QString W;
    static const QString H;
    static const QString Visible;
    static const QString Mode;
    static const QString Object;
    static const QString Index;
    static const QString Arg;
    static const QString Image;
    static const QString Up;
    static const QString Down;
    static const QString Normal;
    static const QString Press;
    static const QString Pressed;
    static const QString Focus;
    static const QString Hover;
    static const QString Select;
    static const QString Transparent;
    static const QString Undefined;
    static const QString Gray;
    static const QString White;
    static const QString Black;
    static const QString Blue;
    static const QString Green;
    static const QString Red;
    static const QString Dark;
    static const QString SharpBlack;
    static const QString SharpWhite;
    static const QString Total;
    static const QString Error;
    static const QString ButtonText;
    static const QString Var;
    static const QString DontUseEditBox;
    static const QString System;
    static const QString ReFocus;
    static const QString IsClosed;
    static const QString Show;
    static const QString PullUp;
    static const QString MessageType;
    static const QString Active;


    //setting
    static const QString Section;
    static const QString Key;
    static const QString Value;
    static const QString Setting;
    static const QString Settings;
    static const QString Type;
    static const QString NoMapData;
    static const QString IncompleteMapData;
    static const QString IncompatibleMapData;
    static const QString ErrorMapData;
    static const QString ResponseNotificationId;
    static const QString RouteSearch;


    //search
    static const QString Command;
    static const QString RequestCommandPtr;
    static const QString Text;
    static const QString ViewId;
    static const QString Address;
    static const QString Industry;
    static const QString Around;
    static const QString Detail;
    static const QString EvDetail;
    static const QString Lowest;
    static const QString BookmarkType;
    static const QString Favorite;
    static const QString RecentDestination;
    static const QString Level;
    static const QString SubLevel;
    static const QString Slot;
    static const QString Keyword;
    static const QString Max;
    static const QString Offset;
    static const QString Alias;
    static const QString Count;
    static const QString FocusIndex;
    static const QString Kind;
    static const QString From;
    static const QString To;
    static const QString T;
    static const QString B;
    static const QString L;
    static const QString R;
    static const QString Fuel;
    static const QString Code;
    static const QString Request;
    static const QString Response;
    static const QString Result;
    static const QString Notification;
    static const QString Sync;

    //toast
    static const QString Position;
    static const QString Duration;
    static const QString Message;

    //keyboard
    static const QString SearchBtnAutoEnable;
    static const QString SearchBtnAlwaysEnable;

    //soft key
    static const QString NoOption;

    //Application
    static const QString ContentRoot;
    static const QString ContentWrap;
    static const QString ZoneWrap;
    static const QString MapViewWrap;
    static const QString ToastWrap;
    static const QString AlertWrap;
    static const QString ProgressWrap;
    static const QString WidgetWrap;
    static const QString TestWrap;
    static const QString ContentRootWithoutSoftkey;
    static const QString ContentRootForCommonDialog;


    //Dialog
    static const QString ContentWidth;
    static const QString ContentHeight;
    static const QString Title;
    static const QString Ok;
    static const QString Cancel;
    static const QString Menus;
    static const QString Message2;
    static const QString TypeA;
    static const QString TypeB;
    static const QString TypeC;


    //Lang
    static const QString Lang;
    static const QString Ko;
    static const QString En;

    static const QString ObjectOwnership;
};

}

#endif

