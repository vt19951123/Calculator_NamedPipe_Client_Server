// client/qml/main.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: mainWindow
    width: 400
    height: 600
    visible: true
    title: "Calculator"
    color: "#f0f0f0"
    
    // Kết nối với signals từ C++
    Connections {
        target: calculatorClient
        
        function onResultReady(result) {
            resultDisplay.text = result
            statusText.text = "Calculation completed"
            statusText.color = "green"
        }
        
        function onErrorOccurred(errorMessage) {
            resultDisplay.text = "Error"
            statusText.text = errorMessage
            statusText.color = "red"
        }
    }
    
    // Layout chính
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15
        
        // Tiêu đề
        Text {
            text: "Named Pipe Calculator"
            font.pixelSize: 24
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }
        
        // Hiển thị kết quả
        Rectangle {
            Layout.fillWidth: true
            height: 80
            color: "white"
            border.color: "#dddddd"
            radius: 5
            
            TextInput {
                id: expressionInput
                anchors.fill: parent
                anchors.margins: 10
                font.pixelSize: 20
                verticalAlignment: TextInput.AlignVCenter
                focus: true
                selectByMouse: true
                
                // Placeholder text
                Text {
                    anchors.fill: parent
                    text: "Enter expression (e.g., 5 + 3 * 2)"
                    font.pixelSize: 20
                    color: "#aaaaaa"
                    verticalAlignment: Text.AlignVCenter
                    visible: !expressionInput.text && !expressionInput.activeFocus
                }
            }
        }
        
        // Nút tính toán
        Button {
            text: "Calculate"
            Layout.fillWidth: true
            height: 50
            font.pixelSize: 18
            
            onClicked: {
                if (expressionInput.text.trim() !== "") {
                    statusText.text = "Calculating..."
                    statusText.color = "blue"
                    var result = calculatorClient.calculateExpression(expressionInput.text)
                }
            }
        }
        
        // Kết quả
        Rectangle {
            Layout.fillWidth: true
            height: 60
            color: "white"
            border.color: "#dddddd"
            radius: 5
            
            Text {
                id: resultDisplay
                anchors.fill: parent
                anchors.margins: 10
                font.pixelSize: 20
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight
                text: "Result"
            }
        }
        
        // Status text
        Text {
            id: statusText
            text: "Ready"
            color: "green"
            font.pixelSize: 14
            Layout.fillWidth: true
        }
        
        // Phím tắt cho calculator
        GridLayout {
            columns: 4
            Layout.fillWidth: true
            Layout.fillHeight: true
            columnSpacing: 10
            rowSpacing: 10
            
            // Hàng 1
            CalcButton { text: "7"; onClicked: expressionInput.text += "7" }
            CalcButton { text: "8"; onClicked: expressionInput.text += "8" }
            CalcButton { text: "9"; onClicked: expressionInput.text += "9" }
            CalcButton { text: "/"; onClicked: expressionInput.text += " / " }
            
            // Hàng 2
            CalcButton { text: "4"; onClicked: expressionInput.text += "4" }
            CalcButton { text: "5"; onClicked: expressionInput.text += "5" }
            CalcButton { text: "6"; onClicked: expressionInput.text += "6" }
            CalcButton { text: "*"; onClicked: expressionInput.text += " * " }
            
            // Hàng 3
            CalcButton { text: "1"; onClicked: expressionInput.text += "1" }
            CalcButton { text: "2"; onClicked: expressionInput.text += "2" }
            CalcButton { text: "3"; onClicked: expressionInput.text += "3" }
            CalcButton { text: "-"; onClicked: expressionInput.text += " - " }
            
            // Hàng 4
            CalcButton { text: "0"; onClicked: expressionInput.text += "0" }
            CalcButton { text: "."; onClicked: expressionInput.text += "." }
            CalcButton { 
                text: "C"; 
                onClicked: expressionInput.text = "" 
                backgroundColor: "#ffcccc"
            }
            CalcButton { text: "+"; onClicked: expressionInput.text += " + " }
        }
    }
    
    // Component cho các nút calculator
    component CalcButton: Rectangle {
        property string text: ""
        property color backgroundColor: "#e0e0e0"
        signal clicked()
        
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: mouseArea.pressed ? Qt.darker(backgroundColor, 1.2) : backgroundColor
        radius: 5
        
        Text {
            anchors.centerIn: parent
            text: parent.text
            font.pixelSize: 18
        }
        
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: parent.clicked()
        }
    }
}