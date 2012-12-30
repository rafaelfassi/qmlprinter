#include <QPainter>
#include "qmlprinter.h"

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
                     "Page " + QString::number(page) + " of " + QString::number(count));

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


    painter.drawText(5, 0, pageWidth, 20, Qt::AlignLeft, "Last report information");

    QRectF rectangle(0.0, 0.0, pageWidth, 20.0);
    painter.setPen(Qt::transparent);
    painter.drawRect(rectangle);

    painter.end();

    return picture;
}
