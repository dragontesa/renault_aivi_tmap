#include "CommonConstants.h"
#include "ViewRegistry.h"
#include <QFontMetrics>

using namespace SKT;

const QString CommonConstants::ObjectName           = QStringLiteral("objectName");
const QString CommonConstants::Parent               = QStringLiteral("parent");
const QString CommonConstants::ParentObjectName     = QStringLiteral("parentObjectName");
const QString CommonConstants::Name                 = QStringLiteral("name");
const QString CommonConstants::Url                  = QStringLiteral("url");
const QString CommonConstants::Caller               = QStringLiteral("caller");
const QString CommonConstants::Owner                = QStringLiteral("owner");
const QString CommonConstants::Relay                = QStringLiteral("relay");
const QString CommonConstants::X                    = QStringLiteral("x");
const QString CommonConstants::Y                    = QStringLiteral("y");
const QString CommonConstants::Z                    = QStringLiteral("z");
const QString CommonConstants::Width                = QStringLiteral("width");
const QString CommonConstants::Height               = QStringLiteral("height");
const QString CommonConstants::Modal                = QStringLiteral("modal");
const QString CommonConstants::Empty                = QStringLiteral("");
const QString CommonConstants::Properties           = QStringLiteral("properties");
const QString CommonConstants::W                    = QStringLiteral("w");
const QString CommonConstants::H                    = QStringLiteral("h");
const QString CommonConstants::Visible              = QStringLiteral("visible");
const QString CommonConstants::Mode                 = QStringLiteral("mode");
const QString CommonConstants::Object               = QStringLiteral("object");
const QString CommonConstants::Index                = QStringLiteral("index");
const QString CommonConstants::Arg                  = QStringLiteral("arg");
const QString CommonConstants::Image                = QStringLiteral("image");
const QString CommonConstants::Up                   = QStringLiteral("up");
const QString CommonConstants::Down                 = QStringLiteral("down");
const QString CommonConstants::Normal               = QStringLiteral("normal");
const QString CommonConstants::Press                = QStringLiteral("press");
const QString CommonConstants::Pressed              = QStringLiteral("pressed");
const QString CommonConstants::Focus                = QStringLiteral("focus");
const QString CommonConstants::Hover                = QStringLiteral("hover");
const QString CommonConstants::Select               = QStringLiteral("select");
const QString CommonConstants::Transparent          = QStringLiteral("transparent");
const QString CommonConstants::Undefined            = QStringLiteral("undefined");
const QString CommonConstants::Gray                 = QStringLiteral("gray");
const QString CommonConstants::White                = QStringLiteral("white");
const QString CommonConstants::Black                = QStringLiteral("black");
const QString CommonConstants::Blue                 = QStringLiteral("blue");
const QString CommonConstants::Green                = QStringLiteral("green");
const QString CommonConstants::Red                  = QStringLiteral("red");
const QString CommonConstants::Dark                 = QStringLiteral("dark");
const QString CommonConstants::SharpBlack           = QStringLiteral("#000000");
const QString CommonConstants::SharpWhite           = QStringLiteral("#ffffff");
const QString CommonConstants::Total                = QStringLiteral("total");
const QString CommonConstants::Error                = QStringLiteral("error");
const QString CommonConstants::ButtonText           = QStringLiteral("buttonText");
const QString CommonConstants::Var                  = QStringLiteral("var");
const QString CommonConstants::DontUseEditBox       = QStringLiteral("dontUseEditBox");
const QString CommonConstants::System               = QStringLiteral("system");
const QString CommonConstants::ReFocus              = QStringLiteral("reFocus");
const QString CommonConstants::IsClosed             = QStringLiteral("isClosed");
const QString CommonConstants::Show                 = QStringLiteral("show");
const QString CommonConstants::PullUp               = QStringLiteral("pullup");
const QString CommonConstants::MessageType          = QStringLiteral("messageType");
const QString CommonConstants::Active               = QStringLiteral("active");


//toast
const QString CommonConstants::Position             = QStringLiteral("position");
const QString CommonConstants::Duration             = QStringLiteral("duration");
const QString CommonConstants::Message              = QStringLiteral("message");



//setting
const QString CommonConstants::Section              = QStringLiteral("section")            ;
const QString CommonConstants::Key                  = QStringLiteral("key")                ;
const QString CommonConstants::Value                = QStringLiteral("value")              ;
const QString CommonConstants::Setting              = QStringLiteral("setting")            ;
const QString CommonConstants::Settings             = QStringLiteral("settings")           ;
const QString CommonConstants::Type                 = QStringLiteral("type")               ;
const QString CommonConstants::NoMapData            = QStringLiteral("noMapData")          ;
const QString CommonConstants::IncompleteMapData    = QStringLiteral("incompleteMapData")  ;
const QString CommonConstants::IncompatibleMapData  = QStringLiteral("incompatibleMapData");
const QString CommonConstants::ErrorMapData         = QStringLiteral("errorMapData")       ;
const QString CommonConstants::ResponseNotificationId = QStringLiteral("responseNotificationId");
//-section에 해당하는 것으로 소문자로 변경하지 말것
const QString CommonConstants::RouteSearch          = QStringLiteral("RouteSearch")        ;


//search
const QString CommonConstants::Command              = QStringLiteral("Command")            ;
const QString CommonConstants::RequestCommandPtr    = QStringLiteral("RequestCommandPtr")  ;
const QString CommonConstants::Text                 = QStringLiteral("text")               ;
const QString CommonConstants::ViewId               = QStringLiteral("viewId")             ;
const QString CommonConstants::Address              = QStringLiteral("address")            ;
const QString CommonConstants::Industry             = QStringLiteral("industry")           ;
const QString CommonConstants::Around               = QStringLiteral("around")             ;
const QString CommonConstants::Detail               = QStringLiteral("detail")             ;
const QString CommonConstants::EvDetail             = QStringLiteral("evdetail")           ;
const QString CommonConstants::Lowest               = QStringLiteral("lowest")             ;
const QString CommonConstants::BookmarkType         = QStringLiteral("bookmarkType")       ;
const QString CommonConstants::Favorite             = QStringLiteral("favorite")           ;
const QString CommonConstants::RecentDestination    = QStringLiteral("recentDestination")  ;
const QString CommonConstants::Level                = QStringLiteral("level")              ;
const QString CommonConstants::SubLevel             = QStringLiteral("subLevel")           ;
const QString CommonConstants::Slot                 = QStringLiteral("slot")               ;
const QString CommonConstants::Keyword              = QStringLiteral("keyword")            ;
const QString CommonConstants::Max                  = QStringLiteral("max")                ;
const QString CommonConstants::Offset               = QStringLiteral("offset")             ;
const QString CommonConstants::Alias                = QStringLiteral("alias")              ;
const QString CommonConstants::Count                = QStringLiteral("count")              ;
const QString CommonConstants::FocusIndex           = QStringLiteral("focusIndex")         ;
const QString CommonConstants::Kind                 = QStringLiteral("kind")               ;
const QString CommonConstants::From                 = QStringLiteral("from")               ;
const QString CommonConstants::To                   = QStringLiteral("to")                 ;
const QString CommonConstants::T                    = QStringLiteral("t")                  ;
const QString CommonConstants::B                    = QStringLiteral("b")                  ;
const QString CommonConstants::L                    = QStringLiteral("l")                  ;
const QString CommonConstants::R                    = QStringLiteral("r")                  ;
const QString CommonConstants::Fuel                 = QStringLiteral("fuel")               ;
const QString CommonConstants::Code                 = QStringLiteral("code")               ;
const QString CommonConstants::Request              = QStringLiteral("request")            ;
const QString CommonConstants::Response             = QStringLiteral("response")           ;
const QString CommonConstants::Result               = QStringLiteral("result")             ;
const QString CommonConstants::Notification         = QStringLiteral("notification")       ;
const QString CommonConstants::Sync                 = QStringLiteral("sync");


//keyboard
const QString CommonConstants::SearchBtnAutoEnable    = QStringLiteral("searchBtnAutoEnable");
const QString CommonConstants::SearchBtnAlwaysEnable  = QStringLiteral("searchBtnAlwaysEnable");


//soft key
const QString CommonConstants::NoOption               = QStringLiteral("noOption");


//Application
const QString CommonConstants::ContentRoot            = QStringLiteral("contentRoot");
const QString CommonConstants::ContentWrap            = QStringLiteral("contentWrap");
const QString CommonConstants::ZoneWrap               = QStringLiteral("zoneWrap");
const QString CommonConstants::MapViewWrap            = QStringLiteral("mapViewWrap");
const QString CommonConstants::ToastWrap              = QStringLiteral("toastWrap");
const QString CommonConstants::AlertWrap              = QStringLiteral("alertWrap");
const QString CommonConstants::ProgressWrap           = QStringLiteral("progressWrap");
const QString CommonConstants::WidgetWrap             = QStringLiteral("widgetWrap");
const QString CommonConstants::TestWrap               = QStringLiteral("testWrap");
const QString CommonConstants::ContentRootWithoutSoftkey    = QStringLiteral("contentRootWithoutSoftkey");
const QString CommonConstants::ContentRootForCommonDialog = QStringLiteral("contentRootForCommonDialog");



//Dialog
const QString CommonConstants::ContentWidth           = QStringLiteral("contentWidth");
const QString CommonConstants::ContentHeight          = QStringLiteral("contentHeight");
const QString CommonConstants::Title                  = QStringLiteral("title");
const QString CommonConstants::Ok                     = tr("확인");
const QString CommonConstants::Cancel                 = tr("취소");
const QString CommonConstants::Menus                  = QStringLiteral("menus");
const QString CommonConstants::Message2               = QStringLiteral("message2");
const QString CommonConstants::TypeA                  = QStringLiteral("a");
const QString CommonConstants::TypeB                  = QStringLiteral("b");
const QString CommonConstants::TypeC                  = QStringLiteral("c");


//Lang
const QString CommonConstants::Lang                   = QStringLiteral("lang");
const QString CommonConstants::Ko                     = QStringLiteral("ko");
const QString CommonConstants::En                     = QStringLiteral("en");

const QString CommonConstants::ObjectOwnership        = QStringLiteral("objectOwnership");

CommonConstants::CommonConstants()
{
}

QString CommonConstants::objectName( int viewId) {
    return ViewRegistry::objectName( viewId );
}

QString CommonConstants::url( int viewId )
{
    return ViewRegistry::url(viewId);
}

bool CommonConstants::target()
{
    #ifdef BUILD_TARGET
        return true;
    #else
        return false;
    #endif
}

void CommonConstants::resultViewInit(QVariantList &result)
{
    for(int i=0; i <= CommonConstants::ResultViewActive; i++) {
        result.append(CommonConstants::Empty);
    }
}

QString CommonConstants::getElideText(QVariant fontName, int fontPixelSize, int lineWidth, QVariant string)
{
    QFont f(fontName.toString());
    f.setPixelSize(fontPixelSize);
    QFontMetrics fm(f);
    return fm.elidedText(string.toString(),Qt::ElideRight, lineWidth);
}
