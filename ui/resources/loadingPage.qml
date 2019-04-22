import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtQuick.Window 2.2

Page {
    id: loadingPage

    signal indicatorPressedAndHold()

    Rectangle {
        id: rcRectangle
        anchors.fill: parent
        visible: true

        BusyIndicator {
            id: busyIndicator
            x: 280
            y: 195
            width: 300
            height: 300
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            MouseArea {
                anchors.fill: parent
                onPressAndHold: {
                    loadingPage.indicatorPressedAndHold()
                }
            }
        }
    }
}
