#ifndef QMLPRINTER_H
#define QMLPRINTER_H

#include <QList>
#include <QDeclarativeItem>
#include "../../qmlprinterengine.h"

class QmlPrinter : public QmlPrinterEngine
{
    struct UndoProperty
            {
                UndoProperty( QDeclarativeItem * p_item, QString p_prop, QVariant p_value )
                {
                    item = p_item;
                    propertyChanged = p_prop;
                    oldPropertyValue = p_value;
                }
                QDeclarativeItem * item;
                QString propertyChanged;
                QVariant oldPropertyValue;
            };

public:
    QmlPrinter(QObject *obj);

private:
    virtual void preProcessItem( QDeclarativeItem * item );
    virtual void parseDone();
    virtual QPicture firstHeader(qreal pageWidth);
    virtual QPicture pageFooter(qreal pageWidth, int page, int count);
    virtual QPicture lastInfo(qreal pageWidth);
    QList<UndoProperty> m_undoList;
};

#endif // QMLPRINTER_H
