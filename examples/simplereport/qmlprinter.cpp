#include <QPainter>
#include "qmlprinter.h"

#include <QDebug>

QmlPrinter::QmlPrinter(QObject *obj)
    : QmlPrinterEngine(obj)
{
    set_lastInfoInBottom(true);
    setCountPagesInFooter(true);
}

QPicture QmlPrinter::firstHeader(qreal pageWidth)
{
    QPicture picture;
    QPainter painter;

    painter.begin(&picture);

    QPixmap logo("images/customLogo.png");

    painter.drawPixmap((pageWidth - logo.width())/2, 0, logo);

    QRectF rectangle(0.0, 0.0, pageWidth, logo.height());
    painter.setPen(Qt::transparent);
    painter.drawRect(rectangle);
    //painter.drawText(0,0, pageWidth, 30, Qt::AlignCenter, "Testeando");

    painter.end();

    return picture;
}

QPicture QmlPrinter::pageFooter(qreal pageWidth, int page, int count)
{
    QPicture picture;
    QPainter painter;

    painter.begin(&picture);


    painter.drawLine(0, 0, pageWidth, 0);

    painter.drawText(0,0, pageWidth - 5, 30, Qt::AlignRight,
                     "P\341gina " + QString::number(page) + " de " + QString::number(count));

    QRectF rectangle(0.0, 0.0, pageWidth, 30.0);
    painter.setPen(Qt::transparent);
    painter.drawRect(rectangle);

    painter.end();

    return picture;
}

QPicture QmlPrinter::lastInfo(qreal pageWidth)
{
    QPicture picture;
    QPainter painter;

    painter.begin(&picture);


    painter.drawText(5, 0, pageWidth, 20, Qt::AlignLeft,
                     "Os valores apresentados neste relat\363rio est\343o sujeitos a mundan\347as di\341rias sem aviso pr\351vio");

    QRectF rectangle(0.0, 0.0, pageWidth, 20.0);
    painter.setPen(Qt::transparent);
    painter.drawRect(rectangle);

    painter.end();

    return picture;
}
