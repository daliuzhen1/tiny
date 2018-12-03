import QtQuick 2.0
import QtQuick.Controls 1.4
import AllIn 1.0
import QtQuick.Controls.Styles 1.4
TableView
{
    id: preViewTable
    property DataViewQML dataViewQML
    property var preTableColumnInfos
    Component{
        id: columnComponent
        TableViewColumn { width: 100 }
    }
    anchors.fill: parent
    function setTitle(colInfo)
    {
        preTableColumnInfos = colInfo
        while (preViewTable.getColumn(0))
        {
            preViewTable.removeColumn(0)
        }
        for (var i = 0; i < colInfo.length; i++)
        {
            preViewTable.addColumn(columnComponent.createObject(preViewTable, { "role": colInfo[i].colName,
                                                                    "title": colInfo[i].colName}))
        }
    }
    style: TableViewStyle {
        headerDelegate: Rectangle {
            id : headerDelegate
            height: 30
            width: textItem.implicitWidth
            color:
            {
                if (TableColumnInfoQML.DT_TEXT == preViewTable.preTableColumnInfos[styleData.column].colType)
                {
                    return "green"
                }
                else if (TableColumnInfoQML.DT_DATE == preViewTable.preTableColumnInfos[styleData.column].colType)
                {
                    return "gray"
                }
                else
                    return "lightsteelblue"
            }
            Text {
                id: textItem
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: styleData.textAlignment
                anchors.leftMargin: 12
                text: styleData.value
                elide: Text.ElideRight
            }

            Rectangle {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 1
                anchors.topMargin: 1
                width: 1
                color: "#ccc"
            }
        }

    }
}
