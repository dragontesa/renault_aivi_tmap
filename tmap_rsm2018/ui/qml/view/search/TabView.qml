import QtQuick 2.0

Item {
    id: root

    property int current: 0
    onCurrentChanged: {
        setCurrentPage(current)
    }

    function setCurrentPage(index)
    {
        for (var i in children) {
             children[i].visible = i == index
        }
    }

    function setCurrentPageCswFocus(index) {
        for (var i in children) {
            console.log('setCurrentPageCswFocus('+i+')')
            if (i == index) {
                var page = children[i].children[0]
                page.focusCsw = true
            }
        }
    }

    function getCurrentPage()
    {
        for (var i in children) {
            if(children[i].visible)
                return i
        }
        return 0
    }
}
