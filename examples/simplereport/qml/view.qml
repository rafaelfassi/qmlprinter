 import QtQuick 1.0

Item {
    id: root
    width: 500; height: 400

    ListView {
        id: listview1
        objectName: "_StartHeader:1_" // A partir deste ponto sempre que ocorrer uma quebra de pagina o
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
                text: "Produto"
                anchors {
                    left: parent.left
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                }
            }

            Text {
                text: "Preço"
                width: 80
                anchors {
                    right: btnImprimir.left
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
            }

            Button {
                id: btnImprimir
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
                    myModel.print(false, root, "Nome do Relatório");
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
                    myModel.print(true, root, "Nome do Relatório");
                }
            }
        }

        delegate: Rectangle {
            width: parent.width
            height: 20
            color: "grey"

            Text {
                id: txtProduto
                anchors {
                    left: parent.left
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                text: produto
            }

            Text {
                id: txtPreco
                width: 80
                anchors {
                    right: btnDetalhes.left
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
                text: preco
            }

            Button {
                id: btnDetalhes
                objectName: "_NoPrint_"
                text: "Detalhes"
                anchors {
                    right: btnComprar.left
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
            }

            Button {
                id: btnComprar
                objectName: "_NoPrint_"
                text: "Comprar"
                anchors {
                    right: parent.right
                    rightMargin: 20
                    verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
