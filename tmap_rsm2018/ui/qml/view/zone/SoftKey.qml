import QtQuick 2.0
import ApplicationEvent 1.0

Item {
    id: root

    signal backClicked()
    signal optionClicked()
    signal naviClicked()
    signal menuClicked(string menuName)

    property string thisName

    function acquire(name) {
        if (zone3Softkey) {
            zone3Softkey.regist(name, this)
            thisName = name
            console.log("Softkey." + name + " is acuqired. !!!!!!!!!!!!!!!!!!!!!!")
        } else {
            console.log("Softkey " + name + " is passed !!!")
        }
    }
    function isCurrent() {
        return zone3Softkey.isCurrent(thisName)
    }

    //-------------------------------------------------------------------------
    // Do not call directly !!!
    //-------------------------------------------------------------------------
    function procBack() {
        root.backClicked()
    }
    function procOption() {
        root.optionClicked()
    }
    function procNavi() {
        root.naviClicked()
    }
    function procMenu(menuName) {
        root.menuClicked(menuName)
    }
}
