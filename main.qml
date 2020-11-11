import QtQuick 2.0
import QQuickBgfx 1.0

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
        viewId: 0
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
            ColorAnimation { to: "#3369ff"; duration: 2000 }
            ColorAnimation { to: bgfxItem2.backgroundColor; duration: 1500 }
            loops: Animation.Infinite
            running: true
        }

        Rectangle {
            anchors.fill: parent
            border.width: 4
            border.color: parent.backgroundColor
            color: "transparent"
        }
    }

    BgfxItem {
        objectName: "bgfxItem2"
        id: bgfxItem2
        anchors.top: bgfxItem1.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 1
        backgroundColor: "#ff69ff"

        SequentialAnimation on backgroundColor {
            ColorAnimation { to: "#ff69ff"; duration: 1500 }
            ColorAnimation { to: bgfxItem3.backgroundColor; duration: 1400 }
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

        Rectangle {
            anchors.fill: parent
            border.width: 4
            border.color: parent.backgroundColor
            color: "transparent"
        }
    }

    BgfxItem {
        objectName: "bgfxItem3"
        id: bgfxItem3
        anchors.top: bgfxItem2.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height / 3
        viewId: 2
        backgroundColor: "#4fa92f"

        SequentialAnimation on backgroundColor {
            ColorAnimation { to: "#4fa92f"; duration: 1600 }
            ColorAnimation { to: bgfxItem1.backgroundColor; duration: 1200 }
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

        Rectangle {
            anchors.fill: parent
            border.width: 4
            border.color: parent.backgroundColor
            color: "transparent"
        }
    }

    Rectangle {
        color: Qt.rgba(1, 0.85, 0.7, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.top: label1.top
        anchors.right: label1.right
        anchors.left: label1.left
        anchors.bottom: label3.bottom
        anchors.margins: -10
    }

    Text {
        id: label1
        color: "black"
        // wrapMode: Text.WordWrap
        text: "Drag mouse over the height: change Bgfx color luminance at render time. Change camera Z coord."
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: label2.top
        anchors.margins: 2
    }
    Text {
        id: label2
        color: "black"
        // wrapMode: Text.WordWrap
        text: "Drag mouse over the width: change camera X coord."
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: label3.top
        anchors.margins: 2
    }
    Text {
        id: label3
        color: "black"
        // wrapMode: Text.WordWrap
        text: "BgfxItem background color is animated using SequentialAnimation in main.qml."
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 2
    }
}
