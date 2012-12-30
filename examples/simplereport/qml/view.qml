 import QtQuick 1.0

Item {
    id: root
    width: 500; height: 400

    ListView {
        id: listview1
        objectName: "_ListView_StartHeader:1_" // A partir deste ponto sempre que ocorrer uma quebra de pagina o
                                               // objeto declarado como Header:1 sera o cabecalho da nova pagina.

        anchors.fill: parent

        model: myModel
        cacheBuffer: myModel.count * 20

        header: Rectangle {
            objectName: "_Header:1_" //Declaramos que este objeto sera o cabecalho das paginas
            height: 20
            width: parent.width
            color: "blue"

            Text {
                text: "Product"
                anchors {
                    left: parent.left
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                }
            }

            Text {
                text: "Price"
                width: 80
                anchors {
                    right: btnPrint.left
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
            }

            Button {
                id: btnPrint
                objectName: "_NoPrint_" // Nao queremos que botoes sejam impressos.
                                        // poderiamos ter adicionado _NoPrint_ dentro do componente
                                        // Button para que nenhum botao seja impresso.

                text: "Print"
                color: "darkgray"
                anchors {
                    right: btnPdf.left
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                onButtonClick: {
                    myModel.print(false, root, "Report Name");
                }
            }

            Button {
                id: btnPdf
                objectName: "_NoPrint_"
                text: "PDF"
                color: "darkgray"
                anchors {
                    right: parent.right
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                onButtonClick: {
                    myModel.print(true, root, "Report Name");
                }
            }
        }

        delegate: Rectangle {
            width: parent.width
            height: 20
            color: "grey"

            Text {
                id: txtProduct
                anchors {
                    left: parent.left
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                text: product
            }

            Text {
                id: txtPrice
                width: 80
                anchors {
                    right: btnDetails.left
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                text: price
            }

            Button {
                id: btnDetails
                objectName: "_NoPrint_"
                text: "Details"
                anchors {
                    right: btnBuy.left
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
            }

            Button {
                id: btnBuy
                objectName: "_NoPrint_"
                text: "Buy"
                anchors {
                    right: parent.right
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
