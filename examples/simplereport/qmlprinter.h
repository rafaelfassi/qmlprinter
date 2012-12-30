#ifndef QMLPRINTER_H
#define QMLPRINTER_H

#include "../../qmlprinterengine.h"

class QmlPrinter : public QmlPrinterEngine
{
public:
    QmlPrinter(QObject *obj);

private:
    virtual QPicture firstHeader(qreal pageWidth);
    virtual QPicture pageFooter(qreal pageWidth, int page, int count);
    virtual QPicture lastInfo(qreal pageWidth);
};

#endif // QMLPRINTER_H
