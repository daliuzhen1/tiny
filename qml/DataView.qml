import QtQuick 2.0
import QtQuick.Controls 2.0
//import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.3
import QtQuick.Particles 2.11
import QtQuick.Controls.Styles 1.4
import AllIn 1.0
import "Global.js" as Global
Item {
    id: dataView

    property Item mainWindowContentItem
    anchors.fill: mainWindowContentItem
    DataViewQML
    {
        id: dataViewQML
    }

    CollapsiblePane
    {
        id: collapsiblePane
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 10
        defaultHeight: parent.height / 3
        button.contentItem:Text
        {
            text: "Add Connect"
            opacity: enabled ? 1.0 : 0.3
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            font.pointSize:  20
        }
        paneView: paneViewIntern
        Component
        {
            id: paneViewIntern
            Rectangle
            {
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                width: height
                Button
                {
                    anchors.fill: parent
                    contentItem:Text
                    {
                        text: "upload Area"
                        opacity: enabled ? 1.0 : 0.3
                        font.pointSize:  15
                        horizontalAlignment:Text.AlignHCenter
                        verticalAlignment:Text.AlignVCenter
                    }
                    onClicked:
                    {
                        fileDialog.visible = true
                    }

                    FileDialog {
                        id: fileDialog
                        title: "Please choose a file"
                        folder: shortcuts.home
                        onAccepted: {
                            if (dataViewQML.uploadFile(fileDialog.fileUrl))
                            {
                                dataBaseModel.append({"databaseName": dataViewQML.getDataBaseName(dataBaseModel.count)})
                            }
                        }
                        onRejected: {
                        }
                        Component.onCompleted: visible = false
                    }
                }

                DropArea
                {

                    anchors.fill: parent
                    onDropped: {
                        if(drop.hasUrls){
                            if (dataViewQML.uploadFile(drop.urls[0]))
                            {
                                dataBaseModel.append({"databaseName": dataViewQML.getDataBaseName(dataBaseModel.count)})
                            }
                        }
                    }
                }
            }
        }
        opacity: 0.7
    }
    Rectangle
    {
        id: line
        color: "gray"
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: collapsiblePane.bottom
        anchors.topMargin: 5
        height:2
    }

    Item
    {
        id: tabView
        anchors.top: line.bottom
        anchors.topMargin: 30
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30

        ListView
        {
            id: dataBaseView
            anchors.top: parent.top
            anchors.topMargin: 40
            anchors.left: parent.left
            anchors.leftMargin: 10
            width: 200
            anchors.bottom: parent.bottom
            ListModel {
                id : dataBaseModel
            }
            Component
            {
                id: dataBaseDelegate
                Item {
                    width: 200; height: 40
                    Button {
                        id: databaseButton
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        width: 120
                        height: 30
                        text: databaseName
                        onClicked:
                        {
                            var tablistArray = dataViewQML.getTableList(index)
                            tableModel.clear()
                            for (var i in tablistArray)
                            {
                                tableModel.append({"tableName" : tablistArray[i]})
                            }
                            dataBaseView.currentIndex = index
                            console.log(index)
                        }
                    }
                    Button {
                        anchors.left: databaseButton.right
                        anchors.top: databaseButton.top
                        anchors.leftMargin: 10
                        width: 30
                        height: 30
                        text: "Del"
                        onClicked:
                        {
                            if (index == dataBaseView.currentIndex)
                            {
                                tableView.model.clear()
                                preView.visible = false
                            }
                            if (dataViewQML.deleteDataBase(index))
                                dataBaseModel.remove(index)

                        }
                    }
                }
            }
            model: dataBaseModel
            delegate: dataBaseDelegate
            Component.onCompleted:
            {
                var tablistArray = dataViewQML.loadHistoryDatabase()
                dataBaseModel.clear()
                for (var i in tablistArray)
                {
                    dataBaseModel.append({"databaseName" : tablistArray[i]})
                }
            }
        }
        ListView
        {
            id: tableView
            anchors.top: parent.top
            anchors.topMargin: 40
            anchors.left: dataBaseView.right
            anchors.leftMargin: 10
            width: 200
            anchors.bottom: parent.bottom
            model: tableModel
            delegate: tableDelegate
            ListModel {
                id : tableModel
            }
            Component
            {
                id: tableDelegate
                Item {
                    width: 200; height: 40
                    Button {
                        id: tableButton
                        anchors.left: parent.left
                        anchors.verticalCenter: parent.verticalCenter
                        width: 120
                        height: 30
                        text: tableName
                        onClicked:
                        {
                            if (dataViewQML.selectTable(dataBaseView.currentIndex, tableName))
                            {

                                var preTableColumnInfos = dataViewQML.getMetaData()
                                for (var i = 0; i < preTableColumnInfos.length; i++)
                                {
                                    console.log(preTableColumnInfos[i].colName)
                                }
                                preView.setTitle(preTableColumnInfos)
                                preView.model = dataViewQML.getPreViewData()
                                preView.visible = true
                            }
                            else
                                preView.visible = false
                        }
                    }
                    Button
                    {
                        id: creatQuery
                        anchors.left: tableButton.right
                        anchors.top: tableButton.top
                        anchors.leftMargin: 10
                        text: "create"
                        width: 50
                        height: 30
                        onClicked:
                        {
                            console.log(tableName)
                            var queryViewQML = dataViewQML.createQuery(tableName)
                            if (!queryViewQML)
                                console.log("create query error")
                            else
                                console.log(queryViewQML.queryName)
                            console.log("aaa")
                            var component = Qt.createComponent("QueryView.qml")
                            console.log(component.errorString())
                            if (component.status == Component.Ready)
                            {
                                var queryView = component.createObject(mainWindowContentItem, {"queryViewQML" : queryViewQML})
                                Global.addChildView("queryView", queryView)
                            }
                            dataView.visible = false
                        }
                    }
                }
            }
        }

        DataPreView
        {
            id: preView
            visible: false
            dataViewQML: dataViewQML
            anchors.left: parent.left
            anchors.leftMargin: 430
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.top: tableView.top
            anchors.bottom: tableView.bottom
            LoadingView
            {
                id: loading
                asynchronous: false
            }
        }
    }

}
