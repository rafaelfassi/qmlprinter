#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QApplication>
#include "model.h"

 int main(int argc, char ** argv)
 {
     QApplication app(argc, argv);

     ProductModel model;

     for(int x=0; x<50; x++)
     {
        model.addProduct(Product("Some product " + QString::number(x+1), "$35,00"));
     }

     QDeclarativeView view;
     QDeclarativeContext *ctxt = view.rootContext();
     ctxt->setContextProperty("myModel", &model);

     view.setSource(QUrl("qml/view.qml"));
     view.show();
     view.setWindowTitle("Products List");

     return app.exec();
 }
