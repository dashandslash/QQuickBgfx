import QtQuick 2.0
import BgfxQQuickItem 1.0

Item {

    width: 1024
    height: 640

    BgfxItem {
        objectName: "bgfxItem1"
        id: bgfxItem1
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 1
        backgroundColor: "#3369ff"

        Text {
            id: labelItem1
            color: "white"
            wrapMode: Text.WordWrap
            text: "Name: " + parent.objectName + " | ViewId: " + parent.viewId + " | Color: " + parent.backgroundColor
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 20
        }
        
        SequentialAnimation on backgroundColor {
            ColorAnimation { to: "white"; duration: 2000 }
            ColorAnimation { to: "#3369ff"; duration: 2000 }
            loops: Animation.Infinite
            running: true
        }
    }

    BgfxItem {
        objectName: "bgfxItem2"
        id: bgfxItem2
        anchors.top: bgfxItem1.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 2
        backgroundColor: "#ff69ff"
        
        SequentialAnimation on backgroundColor {
            ColorAnimation { to: "white"; duration: 2000 }
            ColorAnimation { to: "#ff69ff"; duration: 2000 }
            loops: Animation.Infinite
            running: true
        }

        Text {
            id: labelItem2
            color: "white"
            wrapMode: Text.WordWrap
            text: "Name: " + parent.objectName + " | ViewId: " + parent.viewId + " | Color: " + parent.backgroundColor
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.margins: 20
        }
    }

    BgfxItem {
        objectName: "bgfxItem3"
        id: bgfxItem3
        anchors.top: bgfxItem2.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 0
        backgroundColor: "#4fa92f"

        SequentialAnimation on backgroundColor {
            ColorAnimation { to: "white"; duration: 2000 }
            ColorAnimation { to: "#4fa92f"; duration: 2000 }
            loops: Animation.Infinite
            running: true
        }

        Text {
            id: labelItem3
            color: "white"
            wrapMode: Text.WordWrap
            text: "Name: " + parent.objectName + " | ViewId: " + parent.viewId + " | Color: " + parent.backgroundColor
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
        color: "white"
        wrapMode: Text.WordWrap
        text: "The backgrounds here are rendered by bgfx Metal backend using the beforeRendering() and beforeRenderPassRecording() signals in QQuickWindow. This text label and its border is rendered using QML"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }
}
