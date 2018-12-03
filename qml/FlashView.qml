import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3
import AllIn 1.0
Item
{
    id: flashView
    anchors.fill: parent
    Text {

        anchors.centerIn: parent
        text: qsTr("all in")
        color: "red"
        font.pixelSize: 50
    }
}
