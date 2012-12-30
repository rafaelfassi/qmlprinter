#include "model.h"
#include <QDeclarativeContext>
#include <QDeclarativeView>

#include <QApplication>

 int main(int argc, char ** argv)
 {
     QApplication app(argc, argv);

     AnimalModel model;

     for(int x=0; x<50; x++)
     {
        model.addAnimal(Animal("Algum produto da loja " + QString::number(x+1), "R$35,00"));
     }

     QDeclarativeView view;
     QDeclarativeContext *ctxt = view.rootContext();
     ctxt->setContextProperty("myModel", &model);

     view.setSource(QUrl("qml/view.qml"));
     view.show();
     view.setWindowTitle("Venda de produtos");

     return app.exec();
 }
