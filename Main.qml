import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls.Material
import QtCore

ApplicationWindow {
    id: app
    visible: true
    width: 1300
    height: 850
    minimumWidth: 960
    minimumHeight: 680
    title: "ChessVerse"
    color: "#050509"
    Material.theme: Material.Dark
    Material.accent: "#40f5ff"

    readonly property color cyan: "#45f4ff"
    readonly property color lime: "#c6ff55"
    readonly property color pink: "#ff5bc7"
    readonly property color violet: "#b88bff"
    readonly property color orange: "#ffae57"
    readonly property var pageColors: [cyan, violet, pink, orange, lime, cyan, violet]
    readonly property var pageIcons: ["⌂", "♟", "◉", "↺", "⚡", "✧", "⚙"]

    background: Canvas {
        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.fillStyle = "#050509"
            ctx.fillRect(0, 0, width, height)
            function nebula(x, y, radius, tint) {
                const glow = ctx.createRadialGradient(x, y, 0, x, y, radius)
                glow.addColorStop(0, tint)
                glow.addColorStop(1, "transparent")
                ctx.fillStyle = glow
                ctx.fillRect(x - radius, y - radius, radius * 2, radius * 2)
            }
            nebula(width * 0.19, height * 0.15, width * 0.45, "#33206a")
            nebula(width * 0.83, height * 0.38, width * 0.43, "#064257")
            nebula(width * 0.70, height * 0.96, width * 0.40, "#501132")
            for (let i = 0; i < 170; ++i) {
                const x = (i * 337 % 997) * width / 997
                const y = (i * 607 % 991) * height / 991
                ctx.fillStyle = i % 13 === 0 ? "#8cf8ff" : i % 9 === 0 ? "#cba7ff" : "#55556f"
                const size = i % 13 === 0 ? 2 : 1
                ctx.fillRect(x, y, size, size)
            }
        }
    }

    readonly property var pages: ["Home", "Match", "Profile", "History", "Laboratory", "Wrapped", "Settings"]
    property string page: "Home"
    property alias threeD: preferences.threeD
    property alias threats: preferences.threats
    readonly property color darkSquare: preferences.palette === 1 ? "#8a5f47" :
                                         preferences.palette === 2 ? "#44685e" : "#65527f"
    readonly property color lightSquare: preferences.palette === 1 ? "#f0d6b2" :
                                          preferences.palette === 2 ? "#d9e9d4" : "#d2c3e6"
    Settings {
        id: preferences
        category: "Board"
        property bool threeD: false
        property bool threats: false
        property int palette: 0
        property bool reactions: true
    }
    property bool autoPlay: false
    readonly property bool recapsAvailable: Boolean(verse.recap.status)

    function startMatch(mode) {
        captureToast.visible = false
        autoPlay = false
        verse.start(mode)
        page = "Match"
    }

    function showRecap(summary) {
        if (summary.status) {
            captureToast.visible = false
            captureTimer.stop()
            postGame.summary = summary
            postGame.open()
        }
    }

    Connections {
        target: verse
        function onMatchFinished() {
            if (app.page === "Match")
                app.showRecap(verse.recap)
        }
        function onPieceCaptured() {
            if (!preferences.reactions || app.page !== "Match")
                return
            captureToast.mascotIndex = (captureToast.mascotIndex + 1) % 3
            captureToast.visible = true
            captureTimer.restart()
        }
        function onChanged() {
            if (verse.promotionSquare >= 0 && !promotion.visible)
                promotion.open()
        }
    }

    Shortcut {
        sequences: [StandardKey.Undo]
        enabled: app.page === "Match" && verse.canUndo && !matchChat.activeFocus && !resign.visible && !drawDialog.visible
        onActivated: verse.undo()
    }
    Shortcut {
        sequences: [StandardKey.Redo]
        enabled: app.page === "Match" && verse.canRedo && !matchChat.activeFocus && !resign.visible && !drawDialog.visible
        onActivated: verse.redo()
    }

    Timer {
        interval: 950
        running: app.autoPlay && app.page === "Match" && verse.mode === "Spectator" &&
                 !verse.ended && !verse.replaying
        repeat: true
        onTriggered: verse.step()
    }

    component Tile: Rectangle {
        id: panel
        property color accent: app.violet
        property bool lit: hover.hovered
        radius: 16
        color: Qt.tint("#0b0b15", Qt.rgba(accent.r, accent.g, accent.b, lit ? 0.23 : 0.10))
        border.color: Qt.rgba(accent.r, accent.g, accent.b, lit ? 1 : 0.48)
        border.width: lit ? 2 : 1
        Behavior on color { ColorAnimation { duration: 160 } }
        Behavior on border.color { ColorAnimation { duration: 150 } }

        HoverHandler { id: hover }

        Rectangle {
            anchors.fill: parent
            anchors.margins: -3
            radius: panel.radius + 3
            color: "transparent"
            border.color: panel.accent
            border.width: 2
            opacity: panel.lit ? 0.36 : 0
            Behavior on opacity { NumberAnimation { duration: 170 } }
        }
        Rectangle {
            anchors.fill: parent
            anchors.margins: -7
            radius: panel.radius + 7
            color: "transparent"
            border.color: panel.accent
            border.width: 2
            opacity: panel.lit ? 0.13 : 0
            Behavior on opacity { NumberAnimation { duration: 200 } }
        }
    }

    component Action: Button {
        id: button
        property color accent: app.cyan
        implicitHeight: 39
        hoverEnabled: true
        onClicked: verse.buttonSound()
        palette.buttonText: button.enabled ? "#f5f8ff" : "#8991a3"
        background: Tile {
            radius: 9
            accent: button.accent
            lit: button.enabled && (button.hovered || button.visualFocus || button.highlighted)
            opacity: button.enabled ? 1 : 0.48
        }
    }

    component LaunchCard: Button {
        id: launch
        required property color accent
        required property string symbol
        required property string detail
        hoverEnabled: true
        Accessible.description: detail
        implicitHeight: 102
        onClicked: verse.buttonSound()
        background: Tile {
            accent: launch.accent
            lit: launch.hovered || launch.visualFocus || launch.down
        }
        contentItem: RowLayout {
            spacing: 12
            Rectangle {
                Layout.preferredWidth: 52
                Layout.preferredHeight: 52
                radius: 12
                color: Qt.rgba(launch.accent.r, launch.accent.g, launch.accent.b, 0.17)
                border.color: launch.accent
                Rectangle {
                    x: 7
                    y: 8
                    width: 37
                    height: 32
                    radius: 16
                    color: "transparent"
                    border.color: launch.accent
                    opacity: launch.hovered ? 0.85 : 0.35
                    rotation: launch.hovered ? -29 : 16
                    Behavior on rotation { NumberAnimation { duration: 180 } }
                }
                Text { x: 3; y: 1; text: "✦"; color: launch.accent; font.pixelSize: 10 }
                Text { x: 39; y: 33; text: "·"; color: launch.accent; font.pixelSize: 15 }
                Text {
                    anchors.centerIn: parent
                    text: launch.symbol
                    color: launch.accent
                    font.pixelSize: 29
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 4
                Heading {
                    Layout.fillWidth: true
                    font.pixelSize: 21
                    text: launch.text
                    elide: Text.ElideRight
                }
                Metric {
                    Layout.fillWidth: true
                    text: launch.detail
                    wrapMode: Text.WordWrap
                }
            }
            Metric { text: "↗"; color: launch.accent; font.pixelSize: 22 }
        }
        padding: 13
    }

    component StatCard: Tile {
        id: stat
        required property string caption
        required property string value
        Layout.fillWidth: true
        Layout.preferredWidth: 1
        Layout.minimumHeight: 70
        implicitHeight: 76
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 12
            spacing: 4
            Heading { text: stat.value; color: stat.accent; font.pixelSize: 24 }
            Metric { text: stat.caption }
        }
    }

    component Heading: Label {
        font.pixelSize: 26
        font.bold: true
        color: "#f7f9ff"
    }

    component Metric: Label {
        color: "#c5cbdf"
        font.pixelSize: 14
    }

    header: ToolBar {
        implicitHeight: 64
        background: Rectangle {
            color: "#0b0b17"
            Rectangle {
                anchors.bottom: parent.bottom
                width: parent.width
                height: 2
                gradient: Gradient {
                    orientation: Gradient.Horizontal
                    GradientStop { position: 0; color: app.violet }
                    GradientStop { position: 0.5; color: app.cyan }
                    GradientStop { position: 1; color: app.pink }
                }
            }
        }
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 22
            anchors.rightMargin: 22
            Heading { text: "♞  CHESS//VERSE"; color: app.cyan }
            Item { Layout.fillWidth: true }
            Metric { text: verse.player + "  ·  Recommended AI " + verse.difficulty }
            Action {
                text: app.visibility === Window.FullScreen ? "Exit fullscreen" : "Fullscreen"
                onClicked: app.visibility = app.visibility === Window.FullScreen ? Window.Windowed : Window.FullScreen
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16
        Tile {
            accent: app.violet
            Layout.preferredWidth: 152
            Layout.fillHeight: true
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                Repeater {
                    model: app.pages
                    Action {
                        required property string modelData
                        required property int index
                        accent: app.pageColors[index]
                        text: app.pageIcons[index] + "  " + modelData
                        highlighted: app.page === modelData
                        Layout.fillWidth: true
                        onClicked: app.page = modelData
                    }
                }
                Item { Layout.fillHeight: true }
                Action {
                    text: "Resume game"
                    accent: app.lime
                    Layout.fillWidth: true
                    enabled: verse.hasSave
                    onClicked: {
                        if (verse.resume()) app.page = "Match"
                        else resumeError.open()
                    }
                }
            }
        }

        StackLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: app.pages.indexOf(app.page)

            ColumnLayout {
                spacing: 12
                Tile {
                    accent: app.cyan
                    Layout.fillWidth: true
                    Layout.preferredHeight: 150
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 17
                        ColumnLayout {
                            Layout.fillWidth: true
                            Heading {
                                Layout.fillWidth: true
                                text: "Play chess"
                                font.pixelSize: 38
                                elide: Text.ElideRight
                            }
                            Metric { text: "Choose a match mode" }
                        }
                        Image {
                            id: heroArt
                            Layout.fillHeight: true
                            Layout.preferredWidth: app.width < 1080 ? 255 : 400
                            source: Qt.resolvedUrl("art/orbit.png")
                            fillMode: Image.PreserveAspectCrop
                            clip: true
                            smooth: true
                            scale: artHover.hovered ? 1.045 : 1
                            Behavior on scale { NumberAnimation { duration: 180 } }
                            HoverHandler { id: artHover; cursorShape: Qt.PointingHandCursor }
                            TapHandler {
                                onTapped: {
                                    sampleSwitch.checked = true
                                    app.page = "History"
                                }
                            }
                            Rectangle {
                                anchors.bottom: parent.bottom
                                anchors.right: parent.right
                                anchors.margins: 7
                                width: galleryLabel.implicitWidth + 16
                                height: 27
                                radius: 7
                                color: "#e9090a17"
                                border.color: app.cyan
                                Label { id: galleryLabel; anchors.centerIn: parent; text: "Demo replays ↗"; color: "#eaffff" }
                            }
                        }
                    }
                }
                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    rowSpacing: 12
                    columnSpacing: 12
                    Repeater {
                        model: [
                            { mode: "Classic", detail: "Two players on one board", symbol: "♜", color: app.cyan },
                            { mode: "Adaptive", detail: "AI that adjusts to your recent play", symbol: "♞", color: app.lime },
                            { mode: "Nemesis", detail: "AI rival with match commentary", symbol: "♛", color: app.pink },
                            { mode: "Spectator", detail: "Watch two AIs play each other", symbol: "♚", color: app.violet }
                        ]
                        LaunchCard {
                            required property var modelData
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.preferredWidth: 1
                            Layout.minimumHeight: 85
                            text: modelData.mode
                            detail: modelData.detail
                            symbol: modelData.symbol
                            accent: modelData.color
                            onClicked: app.startMatch(modelData.mode)
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    StatCard { caption: "Local Elo"; value: String(verse.localRating); accent: app.orange }
                    StatCard { caption: "Matches played"; value: String(verse.profileStats.games); accent: app.pink }
                    StatCard { caption: "Recommended AI"; value: verse.difficulty; accent: app.lime }
                }
            }

            RowLayout {
                spacing: 12
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Heading {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        font.pixelSize: 20
                        text: (verse.replaying ? "Replay" : verse.mode) + "  /  " + verse.status
                    }
                    RowLayout {
                        Action { text: app.threeD ? "2D board" : "3D board"; onClicked: app.threeD = !app.threeD }
                        Action { text: "Hint"; enabled: !verse.busy && !verse.ended && !verse.replaying && !verse.rushing && verse.mode !== "Spectator"; onClicked: verse.suggest() }
                        Action { text: "Threats"; onClicked: app.threats = !app.threats }
                        Action { text: "Save"; enabled: verse.canSave; onClicked: { if (verse.save()) saved.open(); else saveError.open() } }
                        BusyIndicator { running: verse.busy; Layout.preferredWidth: 30; Layout.preferredHeight: 30 }
                    }
                    Metric {
                        Layout.fillWidth: true
                        visible: verse.lesson.length > 0
                        wrapMode: Text.Wrap
                        text: (verse.rushing ? verse.rushSeconds + " s  ·  " + verse.rushScore + " solved  /  " : "") + verse.lesson
                    }
                    Tile {
                        accent: app.violet
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Loader {
                            id: board3D
                            active: app.threeD
                            anchors.fill: parent
                            anchors.margins: 5
                            sourceComponent: Board3D { threats: app.threats; angle: cameraAngle.value; lightSquare: app.lightSquare; darkSquare: app.darkSquare }
                        }
                        Item {
                            visible: !app.threeD
                            anchors.centerIn: parent
                            width: Math.max(8, Math.floor((Math.min(parent.width, parent.height) - 48) / 8) * 8)
                            height: width
                            Grid {
                                id: board
                                anchors.fill: parent
                                columns: 8
                                Repeater {
                                    model: verse.pieces
                                    Rectangle {
                                        required property var modelData
                                        width: board.width / 8
                                        height: width
                                        readonly property int square: modelData.square
                                        color: verse.hint === square ? "#43c6ad" :
                                               verse.selected === square ? "#efb14e" :
                                               app.threats && verse.threat(square) ? "#a83b65" :
                                               (Math.floor(square / 8) + square % 8) % 2 ? app.darkSquare : app.lightSquare
                                        Rectangle {
                                            anchors.centerIn: parent
                                            visible: verse.selected >= 0 && !verse.busy && verse.legal(square)
                                            width: parent.width * (modelData.type ? 0.90 : 0.25)
                                            height: width
                                            radius: width / 2
                                            border.width: modelData.type ? 3 : 0
                                            border.color: "#e7bbff"
                                            color: modelData.type ? "transparent" : "#8667b3"
                                        }
                                        Text {
                                            anchors.centerIn: parent
                                            text: modelData.symbol
                                            font.pixelSize: Math.max(8, parent.width * 0.79)
                                            font.family: Qt.platform.os === "windows" ? "Segoe UI Symbol" : "DejaVu Sans"
                                            color: modelData.white ? "#fff8e7" : "#15101d"
                                            style: Text.Outline
                                            styleColor: modelData.white ? "#4e3a5e" : "#c4add9"
                                        }
                                        MouseArea { anchors.fill: parent; onClicked: verse.click(parent.square) }
                                    }
                                }
                            }
                            Row {
                                y: parent.height + 2
                                Repeater {
                                    model: ["a", "b", "c", "d", "e", "f", "g", "h"]
                                    Metric {
                                        required property string modelData
                                        width: board.width / 8
                                        horizontalAlignment: Text.AlignHCenter
                                        text: modelData
                                        font.pixelSize: 11
                                    }
                                }
                            }
                            Column {
                                x: -17
                                Repeater {
                                    model: 8
                                    Metric {
                                        required property int index
                                        height: board.height / 8
                                        verticalAlignment: Text.AlignVCenter
                                        text: 8 - index
                                        font.pixelSize: 11
                                    }
                                }
                            }
                        }
                    }
                    Slider { id: cameraAngle; visible: app.threeD; Layout.fillWidth: true; from: -180; to: 180; value: 0 }
                    Action {
                        visible: verse.ended && app.recapsAvailable
                        text: "Open match recap"
                        onClicked: app.showRecap(verse.recap)
                    }
                    RowLayout {
                        visible: !verse.replaying && !verse.lesson
                        Action { text: "Undo"; enabled: verse.canUndo; onClicked: verse.undo() }
                        Action { text: "Redo"; enabled: verse.canRedo; onClicked: verse.redo() }
                        Action { text: "Resign"; visible: verse.mode !== "Spectator"; enabled: !verse.busy && !verse.ended; onClicked: resign.open() }
                        Action { text: "Draw"; visible: verse.mode !== "Spectator"; enabled: !verse.busy && !verse.ended; onClicked: drawDialog.open() }
                        Action { text: "Step"; visible: verse.mode === "Spectator"; enabled: !verse.busy && !verse.ended; onClicked: verse.step() }
                        Action { text: app.autoPlay ? "Pause" : "Auto"; visible: verse.mode === "Spectator"; enabled: !verse.ended; onClicked: app.autoPlay = !app.autoPlay }
                    }
                    RowLayout {
                        visible: verse.replaying
                        Action { text: "Previous"; enabled: verse.moves > 0; onClicked: verse.seek(verse.moves - 1) }
                        Metric { text: verse.moves + " / " + verse.replaySize }
                        Action { text: "Next"; enabled: verse.moves < verse.replaySize; onClicked: verse.seek(verse.moves + 1) }
                        Action { text: "Exit replay"; onClicked: { verse.closeReplay(); app.page = "History" } }
                    }
                }

                Tile {
                    accent: app.cyan
                    Layout.preferredWidth: 280
                    Layout.fillHeight: true
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        Heading { text: "Live analysis"; font.pixelSize: 22 }
                        Metric {
                            text: "White " + (verse.evaluation / 100).toFixed(2) +
                                  "  ·  " + verse.moves + " half-moves"
                        }
                        ComboBox { id: metric; Layout.fillWidth: true; model: ["Estimated accuracy", "Game sense"] }
                        LiveChart {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 112
                            whiteSeries: metric.currentIndex === 0 ? verse.whiteStats.accuracySeries : verse.whiteStats.senseSeries
                            blackSeries: metric.currentIndex === 0 ? verse.blackStats.accuracySeries : verse.blackStats.senseSeries
                        }
                        Metric {
                            visible: verse.mode === "Adaptive" || verse.mode === "Nemesis" || verse.mode === "Spectator"
                            text: "Match AI  ·  " + verse.matchDifficulty
                        }
                        Repeater {
                            model: [{ name: "White", player: verse.whitePlayer, color: "#45f4ff", stats: verse.whiteStats },
                                    { name: "Black", player: verse.blackPlayer, color: "#ff5bc7", stats: verse.blackStats }]
                            Metric {
                                required property var modelData
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                color: modelData.color
                                textFormat: Text.PlainText
                                text: modelData.name + "  ·  " + modelData.player + "  ·  " +
                                      (modelData.stats.moves ? Number(metric.currentIndex === 0 ? modelData.stats.accuracy : modelData.stats.sense).toFixed(0) + "%" +
                                      " · " + Number(modelData.stats.seconds).toFixed(1) + " s · " + modelData.stats.blunders + " blunders" : "—")
                            }
                        }
                        Metric {
                            visible: verse.log.length === 0
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: "Play a move to start the live analysis."
                        }
                        ListView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            clip: true
                            model: verse.log
                            delegate: Metric {
                                required property string modelData
                                width: ListView.view.width
                                text: modelData
                                wrapMode: Text.Wrap
                                textFormat: Text.PlainText
                                padding: 4
                            }
                            onCountChanged: positionViewAtEnd()
                        }
                        TextField {
                            Layout.fillWidth: true
                            visible: !verse.replaying
                            id: matchChat
                            placeholderText: "Comment or reaction"
                            onAccepted: { verse.chat(text); clear() }
                        }
                    }
                }
            }

            ColumnLayout {
                spacing: 12
                Heading { text: "Local profiles" }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    Tile {
                        accent: app.pink
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        Layout.preferredHeight: 326
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            Heading { text: "Player names"; font.pixelSize: 22 }
                            TextField { id: primary; text: verse.player; placeholderText: "White player"; Layout.fillWidth: true }
                            TextField { id: secondary; text: verse.opponent; placeholderText: "Black player"; Layout.fillWidth: true }
                            Action {
                                accent: app.pink
                                text: "Apply to new matches"
                                Layout.fillWidth: true
                                enabled: primary.text.trim().length > 0 && secondary.text.trim().length > 0 &&
                                         primary.text.trim().toLowerCase() !== secondary.text.trim().toLowerCase()
                                onClicked: verse.profile(primary.text, secondary.text)
                            }
                            Action {
                                text: "Switch players"
                                Layout.fillWidth: true
                                onClicked: {
                                    const oldPrimary = primary.text
                                    primary.text = secondary.text
                                    secondary.text = oldPrimary
                                    verse.profile(primary.text, secondary.text)
                                }
                            }
                            Metric {
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                text: "Use distinct names to track both players in Classic mode."
                            }
                        }
                    }
                    Tile {
                        accent: app.cyan
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        Layout.preferredHeight: 326
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            Heading { text: verse.player; textFormat: Text.PlainText; font.pixelSize: 22; Layout.fillWidth: true; elide: Text.ElideRight }
                            Image {
                                id: avatarImage
                                source: verse.avatar
                                visible: status === Image.Ready
                                Layout.preferredWidth: 90
                                Layout.preferredHeight: 90
                                fillMode: Image.PreserveAspectCrop
                            }
                            MemeDance {
                                visible: !avatarImage.visible
                                animal: "🛸"
                                dancing: false
                                accent: app.cyan
                                Layout.preferredWidth: 96
                                Layout.preferredHeight: 84
                            }
                            Action { text: "Choose profile photo"; onClicked: photo.open() }
                            Metric { text: verse.rivalMemory; Layout.fillWidth: true; wrapMode: Text.WordWrap }
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    StatCard { caption: "Local Elo"; value: String(verse.localRating); accent: app.orange }
                    StatCard { caption: "Recommended AI"; value: verse.difficulty; accent: app.lime }
                }
                Item { Layout.fillHeight: true }
            }

            Tile {
                accent: app.orange
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 22
                    Heading { text: verse.player + " / Match history"; textFormat: Text.PlainText }
                    RowLayout {
                        Switch { id: sampleSwitch; text: "Demo archive"; checked: verse.records.length === 0 }
                        Metric { text: sampleSwitch.checked ? verse.examples.length + " demo games" : verse.records.length + " saved matches" }
                    }
                    LaunchCard {
                        visible: !sampleSwitch.checked && verse.records.length === 0
                        Layout.fillWidth: true
                        text: "Play your first match"
                        detail: "Completed games and replays will appear here."
                        symbol: "♟"
                        accent: app.orange
                        onClicked: app.page = "Home"
                    }
                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true
                        spacing: 8
                        model: sampleSwitch.checked ? verse.examples : verse.records
                        delegate: Tile {
                            required property var modelData
                            width: ListView.view.width
                            height: 72
                            radius: 10
                            accent: modelData.outcome === "Win" ? app.lime : modelData.outcome === "Loss" ? app.pink : app.orange
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 9
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    Metric {
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                        textFormat: Text.PlainText
                                        text: modelData.outcome + " · " + modelData.white + " vs " + modelData.black + " · " + modelData.mode
                                    }
                                    Metric {
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                        text: modelData.date.slice(0, 10) + "  /  " +
                                              (modelData.stats.moves ? Number(modelData.stats.accuracy).toFixed(0) + "% accuracy  ·  " +
                                               Number(modelData.stats.seconds).toFixed(1) + " s/move  ·  " + modelData.stats.blunders + " blunders" : "No rated moves")
                                    }
                                }
                                Action {
                                    text: "Recap"
                                    onClicked: app.showRecap(verse.recapFor(modelData.id))
                                }
                                Action {
                                    text: "Replay"
                                    enabled: !verse.busy
                                    onClicked: if (verse.openReplay(modelData.id)) app.page = "Match"
                                }
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                spacing: 12
                Heading { text: "Chess Academy" }
                GridLayout {
                    columns: 2
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    rowSpacing: 12
                    columnSpacing: 12
                    Repeater {
                        model: ["Fool's mate", "Scholar's mate", "Smothered mate"]
                        LaunchCard {
                            required property string modelData
                            required property int index
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.preferredWidth: 1
                            text: modelData
                            detail: "Find checkmate in one move"
                            symbol: ["♛", "♝", "♞"][index]
                            accent: [app.pink, app.cyan, app.violet][index]
                            onClicked: { verse.practice(index); app.page = "Match" }
                        }
                    }
                    LaunchCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: 1
                        text: "Puzzle Rush"
                        detail: "60 seconds  ·  Best " + verse.rushBest + " / 3"
                        symbol: "⚡"
                        accent: app.lime
                        onClicked: { verse.startRush(); app.page = "Match" }
                    }
                }
                LaunchCard {
                    Layout.fillWidth: true
                    text: "Free practice"
                    detail: "Explore a position with hints and undo"
                    symbol: "♟"
                    accent: app.orange
                    onClicked: { verse.demo(); app.page = "Match" }
                }
            }

            ColumnLayout {
                spacing: 12
                Heading { text: verse.player + " / Wrapped"; textFormat: Text.PlainText }
                GridLayout {
                    columns: 4
                    Layout.fillWidth: true
                    rowSpacing: 12
                    columnSpacing: 12
                    StatCard { caption: "Matches"; value: String(verse.profileStats.games); accent: app.cyan }
                    StatCard { caption: "Wins"; value: String(verse.profileStats.wins); accent: app.lime }
                    StatCard { caption: "Draws"; value: String(verse.profileStats.draws); accent: app.violet }
                    StatCard { caption: "Losses"; value: String(verse.profileStats.losses); accent: app.pink }
                    StatCard { caption: "Est. accuracy"; value: verse.profileStats.ratedMoves ? Number(verse.profileStats.accuracy).toFixed(1) + "%" : "—"; accent: app.cyan }
                    StatCard { caption: "Avg. move time"; value: verse.profileStats.ratedMoves ? Number(verse.profileStats.seconds).toFixed(1) + " s" : "—"; accent: app.lime }
                    StatCard { caption: "Blunders"; value: String(verse.profileStats.blunders); accent: app.orange }
                    StatCard { caption: "Local Elo"; value: String(verse.localRating); accent: app.violet }
                }
                Tile {
                    Layout.fillWidth: true
                    Layout.minimumHeight: 160
                    accent: app.lime
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        Heading { text: "Milestones"; font.pixelSize: 22 }
                        Metric { text: (verse.profileStats.games >= 1 ? "✓  " : "○  ") + "Finish your first match" }
                        Metric { text: (verse.profileStats.wins >= 5 ? "✓  " : "○  ") + "Win five matches" }
                        Metric { text: (verse.rushBest >= 3 ? "✓  " : "○  ") + "Solve all three Puzzle Rush positions" }
                    }
                }
                Item { Layout.fillHeight: true }
            }

            ColumnLayout {
                spacing: 12
                Heading { text: "Settings" }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    Tile {
                        accent: app.cyan
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        Layout.preferredHeight: 204
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            Heading { text: "Match view"; font.pixelSize: 22 }
                            Switch { text: "3D board"; checked: app.threeD; onToggled: app.threeD = checked }
                            Switch { text: "Threat overlay"; checked: app.threats; onToggled: app.threats = checked }
                        }
                    }
                    Tile {
                        accent: app.pink
                        Layout.fillWidth: true
                        Layout.preferredWidth: 1
                        Layout.preferredHeight: 204
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            Heading { text: "Audio"; font.pixelSize: 22 }
                            Switch { text: "Sound effects"; checked: verse.sound; onToggled: verse.setSound(checked) }
                            Switch { text: "Animal reactions"; checked: preferences.reactions; onToggled: preferences.reactions = checked }
                            Action { text: "Preview sound"; accent: app.pink; enabled: verse.sound }
                        }
                    }
                }
                Tile {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 142
                    accent: app.violet
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        ColumnLayout {
                            Layout.fillWidth: true
                            Heading { text: "Board colors"; font.pixelSize: 22 }
                            ComboBox {
                                model: ["Lavender", "Walnut", "Sage"]
                                currentIndex: preferences.palette
                                onActivated: preferences.palette = currentIndex
                            }
                        }
                        Grid {
                            columns: 4
                            Repeater {
                                model: 16
                                Rectangle {
                                    required property int index
                                    width: 24
                                    height: width
                                    color: (Math.floor(index / 4) + index % 4) % 2 ? app.darkSquare : app.lightSquare
                                }
                            }
                        }
                    }
                }
                Item { Layout.fillHeight: true }
            }
        }
    }

    Tile {
        id: captureToast
        parent: Overlay.overlay
        property int mascotIndex: 0
        visible: false
        x: parent.width - width - 27
        y: 85
        width: 174
        height: 121
        accent: app.lime
        lit: visible
        z: 10
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 0
            MemeDance { animal: ["🐈", "🦆", "🐸"][captureToast.mascotIndex] }
            Metric {
                Layout.alignment: Qt.AlignHCenter
                text: ["CAT SAID YOINK", "DUCK TOOK THAT", "FROG APPROVES"][captureToast.mascotIndex]
                color: app.lime
                font.bold: true
            }
        }
    }
    Timer { id: captureTimer; interval: 1600; onTriggered: captureToast.visible = false }

    MatchRecap {
        id: postGame
        anchors.centerIn: parent
        reactions: preferences.reactions
        onPlayAgain: app.page = "Home"
    }

    Dialog {
        id: promotion
        title: "Promote pawn"
        anchors.centerIn: parent
        modal: true
        onRejected: verse.cancelPromotion()
        RowLayout {
            Repeater {
                model: ["Queen", "Rook", "Bishop", "Knight"]
                Action {
                    required property string modelData
                    required property int index
                    text: modelData
                    onClicked: { verse.promote(index + 2); promotion.accept() }
                }
            }
        }
    }
    Dialog { id: resign; title: "Resign this match?"; anchors.centerIn: parent; modal: true; standardButtons: Dialog.Yes | Dialog.No; onAccepted: verse.surrender() }
    Dialog { id: drawDialog; title: "Agree to a draw?"; anchors.centerIn: parent; modal: true; standardButtons: Dialog.Yes | Dialog.No; onAccepted: verse.draw() }
    FileDialog {
        id: photo
        title: "Choose a profile photo"
        nameFilters: ["Images (*.png *.jpg *.jpeg *.webp *.bmp)"]
        onAccepted: if (!verse.setAvatar(selectedFile)) photoError.open()
    }
    Dialog { id: photoError; title: "Could not load the profile photo"; anchors.centerIn: parent; standardButtons: Dialog.Ok }
    Dialog { id: saved; title: "Match saved"; anchors.centerIn: parent; standardButtons: Dialog.Ok }
    Dialog { id: saveError; title: "Could not save match"; anchors.centerIn: parent; standardButtons: Dialog.Ok }
    Dialog { id: resumeError; title: "Could not read saved match"; anchors.centerIn: parent; standardButtons: Dialog.Ok }
}
