import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import BookmarkPageViewEvent 1.0
import SearchTabViewEvent 1.0
import SearchResultDetailViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

BookmarkPageView {
    width: parent.width
    height: GlobalConstants.kScreenContentHeight
}
