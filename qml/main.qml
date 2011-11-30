import QtQuick 1.0
//import QtDesktop 0.1

Rectangle {
    width: 360
    height: 360
    TextEdit {
        anchors.fill: parent
        focus: true
        wrapMode: TextEdit.WordWrap
        text: "Type here!"
    }

//    TextArea {
//        anchors.fill: parent
//        enabled: true
//        focus: true
//        readOnly: false
//        wrapMode: TextEdit.WordWrap
//        text: "Type here!"
//    }
}
