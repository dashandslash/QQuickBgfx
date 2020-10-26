import QtQuick 2.0
import BgfxQQuickItem 1.0

Item {

    width: 1024
    height: 640

    BgfxItem {
        objectName: "bgfxItem1"
        id: bgfxItem1
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 1
        backgroundColor: "#3369ff"

        Text {
            id: labelItem1
            color: "black"
            wrapMode: Text.WordWrap
            text: "Name: " + parent.objectName + " | ViewId: " + parent.viewId
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 20
        }
    }

    BgfxItem {
        objectName: "bgfxItem2"
        id: bgfxItem2
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }
        anchors.top: bgfxItem1.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 2
        backgroundColor: "#ff69ff"

        Text {
            id: labelItem2
            color: "black"
            wrapMode: Text.WordWrap
            text: "Name: " + parent.objectName + " | ViewId: " + parent.viewId
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 20
        }
    }

    BgfxItem {
        objectName: "bgfxItem3"
        id: bgfxItem3
        SequentialAnimation on t {
            NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
            NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
            loops: Animation.Infinite
            running: true
        }
        anchors.top: bgfxItem2.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 0
        backgroundColor: "#4fa92f"

        Text {
            id: labelItem3
            color: "black"
            wrapMode: Text.WordWrap
            text: "Name: " + parent.objectName + " | ViewId: " + parent.viewId
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 20
        }
    }

    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: "The background here is a rendered using bgfx Metal using the beforeRendering() and beforeRenderPassRecording() signals in QQuickWindow. This text label and its border is rendered using QML"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }
}
