import QtQuick 2.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls 1.4
import AllIn 1.0

Item {
    id: queryView
//    property DataViewQML DataViewQML
    anchors.fill: parent
//    Component{
//        id: columnComponent
//        TableViewColumn { width: 120 }
//    }
    TextField
    {
        id: queryName
        anchors.top: parent.top
        anchors.topMargin: 10
        height: 50
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 100
        font.pointSize: 20
//        placeholderText: "Query"
    }

    Button
    {
        id: backButton
        anchors.top: queryName.top
        height: 50
        anchors.left: queryName.right
        anchors.leftMargin: 20
        width: 80
        text: "Back"
    }


    TextField
    {
        id: nodeName
        anchors.top: queryName.bottom
        anchors.topMargin: 20
        height: 30
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 100
        font.pointSize: 15
//        placeholderText: "Query"
    }
    Row
    {
        id: funcButtonArea
        anchors.top: nodeName.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 60
        spacing: 5
        Button
        {
            id: createTable
            text: "table"
            width: 50
            height: 20
        }
        Button
        {
            id: filter
            text: "filter"
            width: 50
            height: 20
        }
        Button
        {
            id: append
            text: "append"
            width: 50
            height: 20
        }
        Button
        {
            id: join
            text: "join"
            width: 50
            height: 20
        }
        Button
        {
            id: pivot
            text: "pivot"
            width: 50
            height: 20
        }
        Button
        {
            id: sum
            text: "sum"
            width: 50
            height: 20
        }
        Button
        {
            id: sql
            text: "sql"
            width: 50
            height: 20
        }
    }
    Item
    {
        id: nodeArea
        anchors.top: funcButtonArea.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        height: parent.height / 3
        Rectangle
        {
            color: "red"
            anchors.fill: parent
        }
    }

    Item
    {
        id: dataArea
        anchors.top: nodeArea.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 50
        Rectangle
        {
            color: "yellow"
            anchors.fill: parent
        }

        Button
        {
            id: addAll
            anchors.left: dataArea.left
            anchors.leftMargin: 20
            anchors.top: dataArea.top
            width: 80
            height: 20
            text: "Add All"
        }
        Button
        {
            id: removeAll
            anchors.left: dataArea.left
            anchors.leftMargin: 20
            anchors.top: addAll.bottom
            anchors.topMargin: 10
            width: 80
            height: 20
            text: "Remove All"
        }
        ListView
        {
            id: columnArea
            anchors.top: removeAll.bottom
            anchors.topMargin: 40
            anchors.left: dataArea.left
            width: 120
            anchors.bottom: dataArea.bottom
            model: columnModel
            delegate: columnDelegate
            ListModel {
                 id: columnModel
                 ListElement {
                     name: "Apple"
                     cost: 2.45
                 }
                 ListElement {
                     name: "Orange"
                     cost: 3.25
                 }
                 ListElement {
                     name: "Banana"
                     cost: 1.95
                 }
             }
            Component
            {
                id: columnDelegate
                Item {
                    width: 120; height: 40
                    Button {
                        id: columnButton
                        anchors.centerIn: parent
                        width: 110
                        height: 30
                        text: name
                    }
                }
            }
        }

        TableView
        {
            id: queryTable
            anchors.left: columnArea.right
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }
    }
    TabView
    {
        anchors.top: dataArea.bottom
        anchors.left: dataArea.left
        anchors.right: dataArea.right
        anchors.bottom: parent.bottom
        frameVisible: false
        Tab {
            title: "Red"
        }
        Tab {
            title: "yellow"
        }
    }



//    TableView
//    {
//        id: queryTable
//        anchors.bottom: parent.bottom
//        anchors.left: parent.left
//        anchors.leftMargin: parent.width / 8
//        anchors.right:  parent.right
//        anchors.rightMargin: 20
//        anchors.bottomMargin: 20
//        anchors.top: parent.top
//        anchors.topMargin: parent.height / 2
//        property var queryTableColumnInfos
//        style: TableViewStyle {
//            headerDelegate: Rectangle {
//                id : headerDelegate
//                height: 30
//                width: textItem.implicitWidth
//                color:
//                {
//                    if (TableColumnInfoQML.DT_TEXT == queryTable.queryTableColumnInfos[styleData.column].colType)
//                    {
//                        return "green"
//                    }
//                    else if (TableColumnInfoQML.DT_DATE == queryTable.queryTableColumnInfos[styleData.column].colType)
//                    {
//                        return "gray"
//                    }
//                    else
//                        return "lightsteelblue"
//                }
//                Text {
//                    id: textItem
//                    anchors.fill: parent
//                    verticalAlignment: Text.AlignVCenter
//                    horizontalAlignment: styleData.textAlignment
//                    anchors.leftMargin: 12
//                    text: styleData.value
//                    elide: Text.ElideRight
//                }

//                Rectangle {
//                    anchors.right: parent.right
//                    anchors.top: parent.top
//                    anchors.bottom: parent.bottom
//                    anchors.bottomMargin: 1
//                    anchors.topMargin: 1
//                    width: 1
//                    color: "#ccc"
//                }
//            }
//        }
//        function readTableColumnInfos(colInfos) {
//            queryTableColumnInfos = colInfos
//            while (queryTable.getColumn(0))
//            {
//                queryTable.removeColumn(0)
//            }
//            for (var i = 0; i < queryTableColumnInfos.length; i++)
//            {
//                queryTable.addColumn(columnComponent.createObject(tablePreview, { "role": queryTableColumnInfos[i].colName,
//                                                                        "title": queryTableColumnInfos[i].colName}))
//            }
//            getData()
//        }
//        function getData()
//        {
//            var firstPack = true
//            queryTable.model = DataViewQML.getAllData(firstPack)
//            firstPack = false
//            var lastPack = DataViewQML.finishingGetData()
//            while (!lastPack)
//            {
//                queryTable.model = null
//                queryTable.model = DataViewQML.getAllData(firstPack)
//                lastPack = DataViewQML.finishingGetData()
//            }
//        }

//        Component.onCompleted:
//        {
//            console.log(2222)
//            DataViewQML.getMetaData(queryTable)
//        }
//    }
}
