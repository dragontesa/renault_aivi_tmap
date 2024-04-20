#include "LayoutManager.h"
#include "uilayout.h"

using namespace SKT;

LayoutManager::LayoutManager()
    : mLayout( UILayout::L800x480 ),
    mDefaultLayout( UILayout::L800x480 )
{
}

LayoutManager::~LayoutManager()
{
}

int LayoutManager::layout()
{
    return mLayout;
}

QString LayoutManager::layoutName()
{
    QString ret;

    switch ( mLayout ) {
    case UILayout::L800x480:
        ret = QStringLiteral("800x480");
        break;
    case UILayout::L820x1024:
        ret = QStringLiteral("820x1024");
        break;
    case UILayout::L1920x720:
        ret = QStringLiteral("1920x720");
        break;
    }

    return ret;
}

void LayoutManager::setLayout( int layout )
{
    mLayout = layout;
}

int LayoutManager::defaultLayout()
{
    return mDefaultLayout;
}

void LayoutManager::setDefaultLayout( int layout )
{
    mDefaultLayout = layout;
}

QString LayoutManager::layoutResDir( int layout )
{
    QString ret;

    switch ( layout ) {
    case UILayout::L800x480:
        ret = QStringLiteral("drawable-800x480");
        break;
    case UILayout::L820x1024:
        ret = QStringLiteral("drawable-820x1024");
        break;
    case UILayout::L1920x720:
        ret = QStringLiteral("drawable-1920x720");
        break;
    }

    return ret;
}

QString LayoutManager::layoutGlobalConstantsJs( int layout )
{
    QString ret;

    switch ( layout ) {
    case UILayout::L800x480:
        ret = QStringLiteral("GlobalConstants_800x480.js");
        break;
    case UILayout::L820x1024:
        ret = QStringLiteral("GlobalConstants_820x1024.js");
        break;
    case UILayout::L1920x720:
        ret = QStringLiteral("GlobalConstants_1920x720.js");
        break;
    }

    return ret;
}


QString LayoutManager::mainWindowPath()
{
    QString ret = QStringLiteral("qrc:/rsm_ui/qml/ApplicationWindow.qml");

    switch ( mLayout ) {
    case UILayout::L1920x720:
        ret = QStringLiteral("qrc:/rsm_ui/qml/ApplicationWindow_1920x720.qml");
        break;
    }

    return ret;
}
