 import QtQuick 1.0

Rectangle {
    height: 15
    width: 60
    radius: 5

    property alias text: buttonText.text
    signal buttonClick

    Text {
        id: buttonText
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }

    MouseArea {
         anchors.fill: parent

         onClicked: {
             buttonClick();
         }
    }
}
