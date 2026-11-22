import QtQuick
import QtQuick3D

View3D {
    id: root
    property color lightSquare: "#d2c3e6"
    property color darkSquare: "#65527f"
    property bool threats: false
    property real angle: 0
    camera: boardCamera

    environment: SceneEnvironment {
        backgroundMode: SceneEnvironment.Color
        clearColor: "#070813"
        antialiasingMode: SceneEnvironment.MSAA
        antialiasingQuality: SceneEnvironment.Medium
    }

    Node {
        eulerRotation.y: root.angle
        PerspectiveCamera {
            id: boardCamera
            position: Qt.vector3d(0, 1030, 980)
            eulerRotation.x: -46
            fieldOfView: 43
            clipFar: 5000
        }
    }
    DirectionalLight { eulerRotation: Qt.vector3d(-50, -30, 0); brightness: 1.7 }
    DirectionalLight { eulerRotation: Qt.vector3d(-40, 140, 0); brightness: 0.6; color: "#d9ccff" }

    Model {
        source: "#Cube"
        y: -24
        scale: Qt.vector3d(8.4, 0.43, 8.4)
        materials: PrincipledMaterial { baseColor: "#141425"; metalness: 0.4; roughness: 0.4 }
    }

    Repeater3D {
        model: verse.pieces
        delegate: Node {
            id: cell
            required property var modelData
            readonly property int indexOnBoard: modelData.square
            readonly property int kind: modelData.type
            readonly property bool white: modelData.white
            readonly property color pieceColor: white ? "#fff0d4" : "#282542"
            readonly property color trim: white ? "#c49354" : "#6b5baf"
            readonly property bool selected: verse.selected === indexOnBoard
            x: (indexOnBoard % 8 - 3.5) * 100
            z: (Math.floor(indexOnBoard / 8) - 3.5) * 100

            PrincipledMaterial { id: bodyMaterial; baseColor: cell.pieceColor; metalness: 0.12; roughness: 0.30 }
            PrincipledMaterial { id: trimMaterial; baseColor: cell.trim; metalness: 0.45; roughness: 0.35 }

            Model {
                source: "#Cube"
                scale: Qt.vector3d(0.99, 0.12, 0.99)
                pickable: true
                readonly property int square: cell.indexOnBoard
                materials: PrincipledMaterial {
                    baseColor: verse.hint === cell.indexOnBoard ? "#36d9be" :
                               cell.selected ? "#f3b956" :
                               verse.selected >= 0 && verse.legal(cell.indexOnBoard) ? "#9780c4" :
                               root.threats && verse.threat(cell.indexOnBoard) ? "#a83b65" :
                               (Math.floor(cell.indexOnBoard / 8) + cell.indexOnBoard % 8) % 2 ?
                                    root.darkSquare : root.lightSquare
                    roughness: 0.4
                }
            }

            Node {
                visible: cell.kind !== 0
                y: cell.selected ? 20 : 7
                Behavior on y { NumberAnimation { duration: 140 } }
                Model {
                    source: "#Cylinder"
                    y: 6
                    scale: Qt.vector3d(0.70, 0.12, 0.70)
                    materials: bodyMaterial
                    pickable: cell.kind !== 0
                    readonly property int square: cell.indexOnBoard
                }
                Model {
                    source: "#Cylinder"
                    y: 14
                    scale: Qt.vector3d(0.58, 0.06, 0.58)
                    materials: trimMaterial
                }
                Model {
                    source: cell.kind === 3 ? "#Cylinder" : "#Cone"
                    y: cell.kind === 6 ? 32 : 40
                    scale: cell.kind === 6 ? Qt.vector3d(0.43, 0.38, 0.43) : Qt.vector3d(0.48, 0.55, 0.48)
                    materials: bodyMaterial
                    pickable: cell.kind !== 0
                    readonly property int square: cell.indexOnBoard
                }
                Model {
                    source: cell.kind === 3 ? "#Cylinder" : cell.kind === 5 ? "#Cube" : "#Sphere"
                    y: cell.kind === 6 ? 56 : 73
                    eulerRotation.x: cell.kind === 5 ? -32 : 0
                    scale: cell.kind === 3 ? Qt.vector3d(0.58, 0.18, 0.58) :
                           cell.kind === 5 ? Qt.vector3d(0.25, 0.38, 0.47) :
                           cell.kind === 4 ? Qt.vector3d(0.33, 0.48, 0.33) :
                           Qt.vector3d(0.38, 0.37, 0.38)
                    materials: bodyMaterial
                    pickable: cell.kind !== 0
                    readonly property int square: cell.indexOnBoard
                }
                Model {
                    visible: cell.kind === 5
                    source: "#Cube"
                    z: -18
                    y: 77
                    scale: Qt.vector3d(0.23, 0.16, 0.30)
                    materials: bodyMaterial
                }
                Model {
                    visible: cell.kind === 1
                    source: "#Cube"
                    y: 104
                    scale: Qt.vector3d(0.10, 0.31, 0.10)
                    materials: trimMaterial
                }
                Model {
                    visible: cell.kind === 1
                    source: "#Cube"
                    y: 108
                    scale: Qt.vector3d(0.28, 0.09, 0.10)
                    materials: trimMaterial
                }
                Repeater3D {
                    model: cell.kind === 2 ? 6 : cell.kind === 3 ? 4 : 0
                    delegate: Model {
                        required property int index
                        readonly property real angle: index * Math.PI * (cell.kind === 2 ? 1 / 3 : 1 / 2)
                        x: Math.cos(angle) * 19
                        z: Math.sin(angle) * 19
                        y: cell.kind === 2 ? 95 : 87
                        source: cell.kind === 2 ? "#Cone" : "#Cube"
                        scale: Qt.vector3d(0.12, 0.17, 0.12)
                        materials: trimMaterial
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: function(mouse) {
            const result = root.pick(mouse.x, mouse.y)
            if (result.objectHit && result.objectHit.square !== undefined)
                verse.click(result.objectHit.square)
        }
    }
}
