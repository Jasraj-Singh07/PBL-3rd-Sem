import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: report
    property var summary: ({})
    property bool reactions: true
    signal playAgain()
    title: summary.demo ? "DEMO  /  Match recap" : "Match recap"
    modal: true
    width: Math.min(parent ? parent.width - 32 : 550, 566)
    padding: 20
    background: Rectangle { color: "#10101d"; radius: 18; border.width: 2; border.color: "#ff5bc7" }

    ColumnLayout {
        spacing: 12
        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true
                Label {
                    Layout.fillWidth: true
                    text: report.summary.status || ""
                    font.pixelSize: 25
                    font.bold: true
                    wrapMode: Text.WordWrap
                }
                Label {
                    Layout.fillWidth: true
                    color: "#ff99db"
                    text: report.summary.roast || ""
                    font.pixelSize: 16
                    wrapMode: Text.WordWrap
                }
            }
            MemeDance { visible: report.reactions; dancing: report.visible; animal: "🦝"; accent: "#ff5bc7" }
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 10
            Repeater {
                model: [{title: "White", stats: report.summary.white, tint: "#45f4ff"},
                        {title: "Black", stats: report.summary.black, tint: "#ff5bc7"}]
                Rectangle {
                    required property var modelData
                    Layout.fillWidth: true
                    Layout.preferredWidth: 1
                    implicitHeight: 98
                    color: "#161a2b"
                    radius: 12
                    border.color: modelData.tint
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        Label { text: modelData.title; color: modelData.tint; font.bold: true }
                        Label {
                            font.pixelSize: 19
                            text: modelData.stats && modelData.stats.moves ?
                                Number(modelData.stats.accuracy).toFixed(1) + "% accuracy" : "No rated moves"
                        }
                        Label {
                            text: modelData.stats && modelData.stats.moves ?
                                modelData.stats.blunders + " blunders · " + Number(modelData.stats.seconds).toFixed(1) + " s/move" : ""
                        }
                    }
                }
            }
        }
        Label {
            Layout.fillWidth: true
            text: report.summary.moment ? "Lowest-rated move  ·  " + report.summary.moment :
                "This game has no rated move to review."
            wrapMode: Text.WordWrap
            color: "#d5dcf3"
        }
        RowLayout {
            Layout.fillWidth: true
            Item { Layout.fillWidth: true }
            Button { text: "Close"; onClicked: report.close() }
            Button { text: "Play again"; highlighted: true; onClicked: { report.close(); report.playAgain() } }
        }
    }
}
