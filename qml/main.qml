import "Global.js" as Global
import QtQuick 2.11
import QtQuick.Controls 1.4
ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1024
    height: 768
    title: qsTr("All in")
    property var flashView
    property var mainWindowContentItem: mainWindow.contentItem
    onVisibleChanged:
    {
        var component = Qt.createComponent("FlashView.qml")
        console.log(component.errorString())
        if (component.status == Component.Ready)
            flashView = component.createObject(mainWindow.contentItem)
        timer.start()
    }

//    DataView
//    {
//        id:dataView
//        anchors.fill: parent
//        Component.onCompleted: {
//            visible = false
//        }
//    }

    Timer
    {
        id: timer
        interval: 2000
        repeat: false
        onTriggered: {
            flashView.visible = false
            flashView.destroy()
            var component = Qt.createComponent("DataView.qml")
            if (component.status == Component.Ready)
            {
                var dataView = component.createObject(mainWindowContentItem, {"mainWindowContentItem" : mainWindowContentItem})
                Global.addChildView("dataView", dataView)
            }
        }
    }
}
