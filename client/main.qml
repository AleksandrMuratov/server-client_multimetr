import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Item{
    id: item1
    width: 640
    height: 480
    ScrollView {
        id: scrollView
        x: 640
        y: 8
        width: 215
        height: 464
        opacity: 1
        anchors.right: parent.right
        anchors.rightMargin: 0
        function scrollToDown() {
            ScrollBar.vertical.position = 1.0 - ScrollBar.vertical.size
        }
        objectName: "Scroll"
        TextEdit {
            objectName: "Log info"
            id: textEdit
            x: 0
            y: 0
            width: parent.width
            height: parent.height
            text: qsTr("")
            font.pixelSize: 12
            persistentSelection: false
            overwriteMode: false
            readOnly: true
        }
    }
    Column {
        id: column4
        x: 25
        y: 76
        width: 394
        height: 308
        spacing: 20
        Row {
            id: row1
            width: 233
            height: 51
            spacing: 10
            Button {
                id: button2
                width: 111
                height: 33
                text: qsTr("Start measure")
                checkable: false
                objectName: "Start"
            }
            Button {
                id: button3
                width: 111
                height: 33
                text: qsTr("Stop measure")
                objectName: "Stop"
            }
        }
        Row {
            id: row
            width: 347
            height: 52
            spacing: 10
            Rectangle {
                id: rectangle
                width: 150
                height: 50
                color: "#ffffff"
                TextEdit {
                    id: textEdit1
                    width: parent.width
                    height: parent.height
                    text: qsTr("")
                    font.pixelSize: 16
                    readOnly: true
                    overwriteMode: true
                    objectName: "Display"
                }
            }
            /*TextField {
                id: textField
                width: 150
                height: 50
                topInset: 0
                placeholderText: qsTr("")
                objectName: "Display"
            }*/
            Column {
                id: column2
                width: 100
                height: 50
                spacing: 10
                Button {
                    id: button
                    width: 99
                    text: qsTr("Get value")
                    objectName: "Get_value"
                }
                Button {
                    id: button4
                    text: qsTr("Get all values")
                    objectName: "Get_values"
                }
            }
            Column {
                id: column3
                width: 125
                height: 51
                spacing: 10

                Button {
                    id: button5
                    width: 124
                    text: qsTr("Get status")
                    objectName: "Get_status"
                }

                Button {
                    id: button6
                    width: 124
                    text: qsTr("Get all statuses")
                    objectName: "Get_statuses"
                }
            }
        }
        Column {
            id: column
            width: 200
            height: 68
            Label {
                id: label
                width: 137
                height: 25
                text: qsTr("Number of channels")
            }
            SpinBox {
                id: spinBox1
                objectName: "Num_of_channel"
            }
        }
        Column {
            id: column1
            width: 251
            height: 55
            spacing: 10
            ComboBox {
                id: comboBox
                width: 236
                height: 24
                currentIndex: 0
                model: ["range0 [0.0000001 ... 0.001)V",
                    "range1 [0.001 ... 1)V",
                    "range2 [1 ... 1000)V",
                    "range3 [1000 ... 1000000)V"]
                objectName: "Range"
            }
            Button {
                id: button1
                width: 236
                height: 25
                text: qsTr("Set range")
                objectName: "Set_range"
            }
        }
    }
}


