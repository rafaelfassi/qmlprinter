/**************************************************************************
**
** This file is part of QmlPrinter
**
** Copyright (c) 2012 Rafael Fassi Lobão
**
** Contact: www.fassi.com.br - rafael@fassi.com.br
**
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include <QDeclarativeItem>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QtAlgorithms>

#include "qmlprinterengine.h"

#include <QDebug>

void GraphicsEffect::draw(QPainter *painter)
{
    drawSource(painter);
}

void GraphicsEffect::drawGraphicsEffect(QDeclarativeItem *item, QPainter *painter)
{
    /*
     Caso item seja um elemento qml nao visual a funcao sourcePixmap() ira
     reclamar com um Q_ASSERT em modo debug.
     Isso nao compromete o resultado, pois os filhos visuais do item serao
     denhados normalmente. Para evitar essa mensagem, troque os itens nao
     visuais que contenham a diretiva _PrintScreen_ por itens visuais.
     Como exemplo, se item for um elemento qml Item troque por um Rectangle.
    */
    Q_ASSERT( !(item->flags() & QGraphicsItem::ItemHasNoContents) );
    item->setGraphicsEffect(this);

    const QPixmap &pixmap = sourcePixmap(Qt::LogicalCoordinates);
    painter->drawPixmap(0, 0, pixmap);
}

QmlPrinterEngine::QmlPrinterEngine(QObject *obj)
    : QObject(obj), m_item(0)
{
    reset();
}

void QmlPrinterEngine::reset()
{
    clearData();
    m_exMargin = 10; m_drawMargins = false;
    m_scale = 1.0; m_aScale = 1.0; m_autoScale = true;
    m_topMargin = 0; m_autoTopMargin = true; m_aligVertical = false;
    m_leftMargin = 0; m_autoLeftMargin = true; m_aligHorizontal = false;
    m_headerAt = -1;
    m_countPagesInFooter = false;
    m_resizeFirstHeader = false;
    m_resizeFooter = false;
    m_resizeLastInfo = false;
    m_lastInfoInBottom = false;
}

void QmlPrinterEngine::clearData()
{
    m_qmlItens.clear();
    m_group.clear();
    m_headers.clear();
    m_headerAt = -1;
    m_footerHeight = 0;
}

void QmlPrinterEngine::clearTemporaryItens()
{
    for(int x=0; x<m_qmlItens.count(); x++)
    {
        if(m_qmlItens[x].objectName.startsWith('*'))
        {
            m_qmlItens.removeAt(x);
            x--;
        }
    }
}

void QmlPrinterEngine::setItemSource(QDeclarativeItem *item)
{
    m_item = item;
}

void QmlPrinterEngine::setScale(qreal scale)
{
    Q_ASSERT(scale);
    if(!scale) return;

    m_scale = scale;
    m_aScale = 1.0/scale;
}

void QmlPrinterEngine::print(bool pdfFormat, QDeclarativeItem *item)
{
    if(item) m_item = item;
    Q_ASSERT(m_item);
    if(!m_item) return;

    m_index = 0;
    parseItem(m_item);
    parseDone();

    QPrinter printer;
    printer.setPageMargins(10, 10, 10, 10, QPrinter::Millimeter);

    if(pdfFormat)
    {
        QString fileName = QFileDialog::getSaveFileName(0,
                                                        tr("Salvar ") + m_reportName,
                                                        QDir::homePath() + "/" + m_reportName,
                                                        "*.pdf");
        if(fileName.isEmpty()) return;
        if (!fileName.endsWith(".pdf", Qt::CaseInsensitive)) fileName += ".pdf";

        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        printDocument(&printer);
    }
    else
    {
        QPrintPreviewDialog previewDialog(&printer);

        connect(&previewDialog, SIGNAL(paintRequested(QPrinter*)), this, SLOT(printDocument(QPrinter*)));
        previewDialog.exec();
    }

    clearData();
}

void QmlPrinterEngine::parseItem(QDeclarativeItem *item, bool group,
                                 qreal opacity, qreal rotation, qreal scale)
{

    //qDebug() << item->metaObject()->className();

    bool parseChildItems = true;

    preProcessItem(item);

    if(item->isVisible() && !item->objectName().contains("_NoPrint_"))
    {
        QmlItem itemDada;
        QRegExp rx;

        itemDada.className = item->metaObject()->className();
        itemDada.objectName = item->objectName();
        itemDada.resize = true;
        itemDada.rect = QRectF(item->scenePos().x(), item->scenePos().y(),
                                item->width(), item->height());


        rx.setPattern("_StartHeader:(\\d+)_");
        if(item->objectName().contains(rx))
        {
            bool ok;
            itemDada.header = rx.cap(1).toInt(&ok);
            Q_ASSERT(ok); // Falha ao obter o id do header
            if(!ok) itemDada.header = 0;
        }
        else if(item->objectName().contains("_EndHeader_"))
        {
            itemDada.header = -1;
        }
        else itemDada.header = 0;

        if(!group)
        {
            m_index++;

            rx.setPattern("_PageBreak:?(\\d+)?_");
            if(item->objectName().contains(rx))
            {
                itemDada.breakAlways = rx.cap(1).isEmpty();
                if(!itemDada.breakAlways)
                {
                    bool ok;
                    itemDada.breakOffset = rx.cap(1).toDouble(&ok);
                    Q_ASSERT(ok); // Falha ao obter o offset
                    itemDada.breakByOffset = ok;
                }
            }
            else
            {
                itemDada.breakAlways = false;
                itemDada.breakByOffset = false;
            }
        }

        itemDada.index = m_index;

        rx.setPattern("(_Group_)|(_Header:\\d+_)");
        if(item->objectName().contains(rx) && !group)
        {
            posProcessItem(item);
            itemDada.picture = createGroup(item, itemDada.rect, opacity, rotation, scale);
            itemDada.visible = true;
            m_qmlItens.append(itemDada);
            return;
        }

        opacity *= item->opacity();
        rotation += item->rotation();
        scale *= item->scale();

        if(!(item->flags() & QGraphicsItem::ItemHasNoContents)
                && !item->objectName().contains("_NoPaint_"))
        {
            QPicture picture;
            QPainter painter;
            QStyleOptionGraphicsItem option;

            painter.begin(&picture);

            painter.setOpacity(opacity);
            painter.rotate(rotation);
            painter.scale(scale, scale);

            if(item->objectName().contains("_PrintScreen_"))
            {
                GraphicsEffect graphicsEffect;
                graphicsEffect.drawGraphicsEffect(item, &painter);
                parseChildItems = false;
            }
            else item->paint(&painter, &option, 0);

            painter.end();

            itemDada.picture = picture;
            itemDada.visible = true;
        }
        else itemDada.visible = false;

        if(!group) m_qmlItens.append(itemDada);
        else m_group.append(itemDada);

        posProcessItem(item);

        if(!parseChildItems) return;

        for(int x = 0; x < item->childItems().count(); x++)
        {
            QDeclarativeItem *itemChild = qobject_cast<QDeclarativeItem *>(item->childItems()[x]);
            parseItem(itemChild, group, opacity, rotation, scale);
        }
    }
    else posProcessItem(item);

}

QPicture QmlPrinterEngine::createGroup(QDeclarativeItem *item, QRectF &rect,
                                       qreal opacity, qreal rotation, qreal scale)
{
    m_group.clear();
    parseItem(item, true, opacity, rotation, scale);

    QPicture picture;
    QPainter painter;

    for(int x=0; x<m_group.count(); x++)
    {
        rect = rect.united(m_group[x].rect);
        //if(m_group[x].visible) rect = rect.united(m_group[x].rect);
    }

    painter.begin(&picture);

    for(int x=0; x<m_group.count(); x++)
    {
        if(m_group[x].visible)
        {
            m_group[x].rect.translate(-rect.left(), -rect.top());
            painter.drawPicture(m_group[x].rect.topLeft(), m_group[x].picture);
        }
    }

    painter.end();

    QRegExp rx("_Header:(\\d+)_");
    if(item->objectName().contains(rx))
    {
        QmlItem qmlItem;
        qmlItem.picture = picture;
        qmlItem.rect = rect;
        qmlItem.rect.translate(0, -rect.top() + m_exMargin);
        qmlItem.breakAlways = false;
        qmlItem.breakByOffset = false;
        qmlItem.resize = true;
        qmlItem.index = 0;
        qmlItem.objectName = "*Header*";

        bool ok;
        m_headers.insert(rx.cap(1).toInt(&ok), qmlItem);
        Q_ASSERT(ok);
    }

    return picture;
}

void QmlPrinterEngine::printDocument(QPrinter *printer)
{
    QPainter painter;

    int page(1);
    int pagesCount = processPages(printer);

    if(printer->fromPage() && printer->fromPage() <= pagesCount) page = printer->fromPage();
    if(printer->toPage() && printer->toPage() < pagesCount) pagesCount = printer->toPage();

    if (!painter.begin(printer)) return;

    if(m_resizeFirstHeader)
    {
       painter.scale(m_scale, m_scale);
       if(page == 1) painter.drawPicture(0, 0, m_firstHeader);
    }
    else
    {
        if(page == 1) painter.drawPicture(0, 0, m_firstHeader);
        painter.scale(m_scale, m_scale);
    }

    drawMargins(printer, &painter);

    for(; page <= pagesCount; page++)
    {
        for(int x=0; x < m_qmlItens.count(); x++)
        {
            if(m_qmlItens[x].page == page)
            {
                if(!m_qmlItens[x].resize)
                {
                    painter.scale(m_aScale, m_aScale);
                    painter.drawPicture(m_qmlItens[x].printRect.topLeft(), m_qmlItens[x].picture);
                    painter.scale(m_scale, m_scale);
                }
                else painter.drawPicture(m_qmlItens[x].printRect.topLeft(), m_qmlItens[x].picture);
            }
        }
        if(page < pagesCount)
        {
            printer->newPage();
            drawMargins(printer, &painter);
        }
    }
    painter.end();
}

void QmlPrinterEngine::preProcessPages(QPrinter *printer)
{
    m_contentRect = QRectF();

    for(int x=0; x < m_qmlItens.count(); x++)
    {
        m_qmlItens[x].printRect = m_qmlItens[x].rect;

        if(m_qmlItens[x].visible)
        {
            if(m_contentRect.isNull()) m_contentRect = m_qmlItens[x].rect;
            else m_contentRect = m_contentRect.united(m_qmlItens[x].rect);
        }
    }

    if(m_autoScale)
    {
        m_scale = printer->pageRect().width()/(m_contentRect.width() + 2*m_exMargin);
        m_aScale = 1.0/m_scale;
    }

    m_firstHeader = firstHeader(printer->pageRect().width()*(m_resizeFirstHeader ? m_aScale : 1));

    if(m_autoTopMargin)
    {
        m_topMargin = -m_contentRect.top();

        if(m_aligVertical) m_topMargin += (printer->pageRect().height()*m_aScale - m_contentRect.height())/2;
        else m_topMargin += m_exMargin + m_firstHeader.height()*(m_resizeFirstHeader ? 1 : m_aScale);

    }

    if(m_autoLeftMargin)
    {
        m_leftMargin = -m_contentRect.left();

        if(m_aligHorizontal) m_leftMargin += ((printer->pageRect().width()*m_aScale - m_contentRect.width())/2);
        else m_leftMargin += m_exMargin;
    }

    // Translacao dos itens
    for(int x=0; x<m_qmlItens.count(); x++)
        m_qmlItens[x].printRect.translate(m_leftMargin, m_topMargin);

    // Translacao dos cabecalhos
    QHash<int, QmlItem>::iterator i;
     for (i = m_headers.begin(); i != m_headers.end(); ++i)
     {
         i.value().printRect = i.value().rect;
         i.value().printRect.translate(m_leftMargin, 0);
     }

     m_qmlItens.append(createLastInfo(printer, m_contentRect.bottom() + m_topMargin));
}

int QmlPrinterEngine::processPages(QPrinter *printer)
{
    qSort(m_qmlItens.begin(), m_qmlItens.end(), lessThanToTop);

    clearTemporaryItens();

    preProcessPages(printer);

    int page(1);

    int count = m_qmlItens.count();

    m_qmlItens.append(createPageFooter(printer, page));

    QmlItem *lastInfoItem(0);

    for(int x=0; x<count; x++)
    {
        if(m_qmlItens[x].header != 0) m_headerAt = m_qmlItens[x].header;

        if(m_qmlItens[x].visible)
        {

            if((m_qmlItens[x].objectName == "*LastInfo*"))
            {
                lastInfoItem = &m_qmlItens[x];

                if(!m_resizeLastInfo)
                    m_qmlItens[x].printRect.moveTop(m_qmlItens[x].printRect.top()*m_scale);
            }

            qreal offset = -m_qmlItens[x].printRect.top();

            if(breakPage(m_qmlItens[x], printer))
            {
                page++;

                qreal headerHeight(0);

                if(m_headerAt > 0)
                {
                    m_headers[m_headerAt].page = page;
                    m_qmlItens.append(m_headers[m_headerAt]);
                    headerHeight = m_qmlItens.last().printRect.height();
                }

                m_qmlItens.append(createPageFooter(printer, page));

                for(int y=x; y<count; y++)
                {
                    m_qmlItens[y].printRect.translate(0, offset + m_exMargin + headerHeight);
                }

            }
            m_qmlItens[x].page = page;
        }
        else m_qmlItens[x].page = 0;
    }

    if(lastInfoItem && m_lastInfoInBottom)
    {
        if(m_resizeLastInfo)
        {
            lastInfoItem->printRect.moveTop((printer->pageRect().height()
                                             - m_footerHeight*m_scale
                                             - lastInfoItem->printRect.height()*m_scale)*m_aScale);
        }
        else
        {
            lastInfoItem->printRect.moveTop(printer->pageRect().height()
                                            - m_footerHeight*m_scale
                                            - lastInfoItem->printRect.height());
        }
    }

    if(m_countPagesInFooter)
    {
        for(int x=count; x<m_qmlItens.count(); x++)
        {
            if(m_qmlItens[x].objectName == "*Footer*")
            {
               m_qmlItens[x].picture = pageFooter(printer->pageRect().width()*(m_resizeFooter ? m_aScale : 1),
                                                  m_qmlItens[x].page,
                                                  page);
            }
        }
    }

    qSort(m_qmlItens.begin(), m_qmlItens.end(), lessThanToIndex);

    return page;
}

bool QmlPrinterEngine::breakPage(QmlItem &qmlitem, QPrinter *printer)
{
    if(qmlitem.breakAlways)
    {
        return true;
    }
    else
    {
        qreal offset(0);
        qreal itemScale = qmlitem.resize ? m_scale : 1;

        if(qmlitem.breakByOffset) offset = qmlitem.breakOffset;
        return qmlitem.printRect.bottom()*itemScale + (m_footerHeight + m_exMargin + offset)*m_scale
                > printer->pageRect().height();
    }
}

void QmlPrinterEngine::drawMargins(QPrinter *printer, QPainter *painter)
{
    if(!m_drawMargins) return;

    qreal leftMargin, topMargin, rightMargin, bottomMargin;
    printer->getPageMargins(&leftMargin, &topMargin, &rightMargin, &bottomMargin, QPrinter::DevicePixel);

    QPointF topLeft(0, printer->pageRect().top() - topMargin);
    QPointF topRight(printer->pageRect().width()*m_aScale, topLeft.y());
    QPointF bottomLeft(topLeft.x(), printer->pageRect().height()*m_aScale);
    QPointF bottomRight(topRight.x(), bottomLeft.y());
    QPointF overplusH(20*m_aScale, 0);
    QPointF overplusV(0, overplusH.x());

    painter->drawLine(topLeft - overplusH, topRight + overplusH); //top
    painter->drawLine(topLeft - overplusV, bottomLeft + overplusV); //left
    painter->drawLine(topRight - overplusV, bottomRight + overplusV); //right
    painter->drawLine(bottomLeft - overplusH, bottomRight + overplusH); //bottom
}

QmlItem QmlPrinterEngine::createPageFooter(QPrinter *printer, int page)
{
    QmlItem qmlItem;

    qmlItem.picture = pageFooter(printer->pageRect().width()*(m_resizeFooter ? m_aScale : 1), page);
    qmlItem.rect = QRectF(0,
                          printer->pageRect().height()*(m_resizeFooter ? m_aScale : 1) - qmlItem.picture.height(),
                          qmlItem.picture.width(),
                          qmlItem.picture.height());

    qmlItem.printRect = qmlItem.rect;
    qmlItem.visible = true;
    qmlItem.breakAlways = false;
    qmlItem.breakByOffset = false;
    qmlItem.resize = m_resizeFooter;
    qmlItem.page = page;
    qmlItem.index = 0;
    qmlItem.objectName = "*Footer*";

    m_footerHeight = qmlItem.picture.height()*(m_resizeFooter ? 1 : m_aScale);

    return qmlItem;
}

QmlItem QmlPrinterEngine::createLastInfo(QPrinter *printer, qreal top)
{
    QmlItem qmlItem;

    qmlItem.picture = lastInfo(printer->pageRect().width()*(m_resizeLastInfo ? m_aScale : 1));
    qmlItem.rect = QRectF(0,
                          top,
                          qmlItem.picture.width(),
                          qmlItem.picture.height());

    qmlItem.printRect = qmlItem.rect;
    qmlItem.visible = true;
    qmlItem.breakAlways = false;
    qmlItem.breakByOffset = false;
    qmlItem.resize = m_resizeLastInfo;
    qmlItem.index = 0;
    qmlItem.objectName = "*LastInfo*";

    return qmlItem;
}

bool QmlPrinterEngine::lessThanToTop(QmlItem &qmlItem1, QmlItem &qmlItem2)
{
    return qmlItem1.rect.top() < qmlItem2.rect.top();
}

bool QmlPrinterEngine::lessThanToIndex(QmlItem &qmlItem1, QmlItem &qmlItem2)
{
    return qmlItem1.index < qmlItem2.index;
}
