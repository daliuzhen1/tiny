import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import QtQuick.Dialogs 1.3

Item {
    id: main
    property alias button : buttonIntern
    property Component paneView
    property int defaultHeight
    height: buttonIntern.height
    Button
    {
        id: buttonIntern
        anchors.left: parent.left
        anchors.right: parent.right
        onClicked: paneSettingsList.shown = !paneSettingsList.shown
        Component.onCompleted:
        {
            main.height = height
        }
    }
    Pane {
        id: paneSettingsList
        property bool shown: false
        visible: shown
        onVisibleChanged:
        {
            if (visible)
                parent.height = defaultHeight
            else
                parent.height = buttonIntern.height
        }

        height: shown ? defaultHeight - buttonIntern.height : 0
        clip: true
        padding: 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: button.bottom
        Behavior on height {
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }
        Loader
        {
            id: loader
            sourceComponent: paneView
            anchors.fill: parent
        }
    }
}
