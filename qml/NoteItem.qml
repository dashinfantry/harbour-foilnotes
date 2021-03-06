import QtQuick 2.2
import Sailfish.Silica 1.0
import harbour.foilnotes 1.0

Item {
    id: noteItem

    property int pageNumber
    property color color
    property string body
    property string filter
    property bool highlighted
    property bool selected
    property bool secret

    readonly property color gradientColor0: secret ? Theme.rgba(noteItem.color, 0.1) : Theme.rgba(Theme.primaryColor, 0.0)
    readonly property color gradientColor1: secret ? Theme.rgba(noteItem.color, 0.4) : Theme.rgba(Theme.primaryColor, 0.1)

    Rectangle {
        readonly property bool showHighlightBackground: highlighted || selected
        anchors.fill: parent
        color: Theme.highlightBackgroundColor
        visible: opacity > 0
        opacity: showHighlightBackground ? Theme.highlightBackgroundOpacity : 0.0
        Behavior on opacity { FadeAnimator { duration: 100 } }
    }

    Item {
        anchors.fill: parent
        clip: true
        Rectangle {
            rotation: 45 // diagonal gradient
            width: parent.width * 1.412136
            height: parent.height * 1.412136
            anchors.centerIn: parent
            gradient: Gradient {
                GradientStop { position: 0.0; color: gradientColor0 }
                GradientStop { position: 1.0; color: gradientColor1 }
            }
        }
    }

    Item {
        anchors {
            fill: parent
            margins: Theme.paddingLarge
        }
        Text {
            id: summary

            anchors {
                top: parent.top
                topMargin: - (font.pixelSize / 4)
                left: parent.left
                right: parent.right
            }
            height: parent.height
            text: body ? Theme.highlightText(body.substr(0, Math.min(body.length, 300)), noteItem.filter, Theme.highlightColor).replace(/\n/g, '<br/>') : ""
            textFormat: Text.StyledText
            font {
                family: Theme.fontFamily
                pixelSize: Theme.fontSizeSmall
            }
            color: highlighted ? Theme.highlightColor : Theme.primaryColor
            wrapMode: Text.Wrap
            readonly property real bottomMargin: Theme.paddingMedium
            readonly property real maxPaintedHeight: Math.max(height - bottomMargin, bottomMargin)
            readonly property int defaultLineCount: Math.floor(maxPaintedHeight/font.pixelSize)
            elide: Text.ElideRight
            onTextChanged: maximumLineCount = defaultLineCount
            onDefaultLineCountChanged: maximumLineCount = defaultLineCount
            onPaintedHeightChanged: adjustLineCount()
            onMaxPaintedHeightChanged: adjustLineCount()
            function adjustLineCount() {
                if (paintedHeight > maxPaintedHeight && maximumLineCount > 1) {
                    maximumLineCount--
                }
            }
        }

        OpacityRampEffect {
            sourceItem: summary
            slope: 0.6
            offset: 0
            direction: OpacityRamp.TopToBottom
        }

        Rectangle {
            anchors {
                bottom: parent.bottom
                left: parent.left
            }
            width: Theme.itemSizeExtraSmall
            height: width/8
            radius: Math.round(Theme.paddingSmall/3)
            color: noteItem.color
            visible: !secret
        }
    }

    Text {
        id: pagenumber

        anchors {
            baseline: parent.bottom
            baselineOffset: -Theme.paddingMedium
            right: parent.right
            rightMargin: Theme.paddingMedium
        }
        opacity: 0.4
        color: highlighted ? Theme.highlightColor : Theme.primaryColor
        font {
            family: Theme.fontFamily
            pixelSize: Theme.fontSizeLarge
        }
        horizontalAlignment: Text.AlignRight
        text: noteItem.pageNumber
    }

    Loader {
        anchors.fill: parent
        active: secret
        sourceComponent: Component {
            Item {
                Image {
                    source: "image://" + HarbourImageProvider + "/" + Qt.resolvedUrl(HarbourTheme.darkOnLight ? "images/cover-lock-dark.svg" : "images/cover-lock.svg") + "?" + noteItem.color
                    x: Theme.paddingLarge
                    y: pagenumber.y + Math.floor((pagenumber.height - height)/2)
                    height: size
                    width: size
                    sourceSize: Qt.size(size, size)
                    readonly property real size: Math.ceil(Theme.itemSizeExtraSmall/3)
                }
            }
        }
    }

    Loader {
        z: noteItem.z + 1
        anchors.fill: parent
        active: opacity > 0
        opacity: noteItem.selected ? 1 : 0
        sourceComponent: Component {
            Item {
                Image {
                    x: pagenumber.x - width - Theme.paddingSmall
                    y: pagenumber.y + Math.floor((pagenumber.height - height)/2)
                    source: "image://theme/icon-s-installed"
                }
            }
        }
    }
}
