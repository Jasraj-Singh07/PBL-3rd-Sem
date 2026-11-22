import QtQuick
import QtQuick.Controls

Item {
    id: dance
    property bool dancing: true
    property string animal: "🐈"
    property color accent: "#c6ff55"
    implicitWidth: 128
    implicitHeight: 92

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        width: 82
        height: 13
        radius: height / 2
        color: Qt.rgba(dance.accent.r, dance.accent.g, dance.accent.b, 0.17)
        border.color: dance.accent
    }
    Text {
        x: 9
        y: 11
        text: "✦"
        font.pixelSize: 21
        color: dance.accent
        opacity: 0.8
    }
    Text {
        anchors.right: parent.right
        anchors.rightMargin: 7
        y: 24
        text: "✧"
        font.pixelSize: 22
        color: "#ff5bc7"
    }
    Text {
        id: mascot
        x: (parent.width - width) / 2
        y: 9
        text: dance.animal
        font.family: Qt.platform.os === "windows" ? "Segoe UI Emoji" : "Noto Color Emoji"
        font.pixelSize: 54
        SequentialAnimation {
            running: dance.visible && dance.dancing
            loops: Animation.Infinite
            ParallelAnimation {
                NumberAnimation { target: mascot; property: "y"; to: 0; duration: 220; easing.type: Easing.OutQuad }
                NumberAnimation { target: mascot; property: "rotation"; to: -15; duration: 220 }
            }
            ParallelAnimation {
                NumberAnimation { target: mascot; property: "y"; to: 14; duration: 220; easing.type: Easing.InQuad }
                NumberAnimation { target: mascot; property: "rotation"; to: 15; duration: 220 }
            }
        }
    }
}
