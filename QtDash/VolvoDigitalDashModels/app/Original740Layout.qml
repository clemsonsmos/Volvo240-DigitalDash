import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3

Item {

    Rectangle {
        width: 1280
        height: 480
        color: "black"


        Rectangle {
            id: tachContainer740
            width: tachSize
            height: tachSize
            color: "transparent"
            anchors.left: speedoContainer740.right
            anchors.verticalCenter: speedoContainer740.verticalCenter

            /* Rpm: */
            ListView {
                model: rpmModel
                delegate: rpmDelegate740
            }

        }

        Rectangle {
            id: clockContainer740
            anchors.right: speedoContainer740.left
            anchors.verticalCenter: speedoContainer740.verticalCenter
            height: tachSize
            width: tachSize
            color: "transparent"

            Clock740 {
                anchors.fill: parent
            }
        }


        Rectangle {
            id: speedoContainer740
            width: speedoSize
            height: speedoSize
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            anchors.topMargin: 10
            color: "transparent"

            /* Rpm: */
            ListView {
                model: speedoModel
                delegate: speedoDelegate740
            }
        }


        Rectangle {
            id: boostContainer
            anchors.left: tachContainer740.right
            anchors.bottom: speedoContainer740.verticalCenter
            anchors.bottomMargin: 10
            height: smallGaugeSize
            width: smallGaugeSize
            color: "transparent"

            ListView {
                model: boostModel
                delegate: boostDelegate740
            }
        }

        Rectangle {
            id: coolantTempContainer
            anchors.left: tachContainer740.right
            anchors.top: speedoContainer740.verticalCenter
            anchors.topMargin: 10
            height: smallGaugeSize
            width: smallGaugeSize
            color: "transparent"

            ListView {
                model: coolantTempModel
                delegate: coolant740Delegate
            }
        }

        Rectangle {
            id: fuelLevelContainer
            anchors.right: clockContainer740.left
            anchors.top: speedoContainer740.verticalCenter
            anchors.topMargin: 10
            height: smallGaugeSize
            width: smallGaugeSize
            color: "transparent"

            ListView {
                model: fuelLevelModel
                delegate: fuelLevel740Delegate
            }
        }



        Rectangle {
            id: voltContainer740
            anchors.right: clockContainer740.left
            anchors.bottom: speedoContainer740.verticalCenter
            anchors.bottomMargin: 10
            height: smallGaugeSize
            width: smallGaugeSize
            color: "transparent"

            ListView {
                model: voltMeterModel
                delegate: voltMeterDelegate740
            }
        }


    }
}
