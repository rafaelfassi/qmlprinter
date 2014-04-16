#include <QListView>
#include "model.h"
#include "qmlprinter.h"


 Product::Product(const QString &p_productName, const QString &p_price)
     : m_name(p_productName), m_price(p_price)
 {
 }

 QString Product::name() const
 {
     return m_name;
 }

 QString Product::price() const
 {
     return m_price;
 }

 ProductModel::ProductModel(QObject *parent)
     : QAbstractListModel(parent)
 {
 }

 QHash<int,QByteArray> ProductModel::roleNames() const
 {
     QHash<int, QByteArray> roles;
     roles[ProductRole] = "product";
     roles[PriceRole] = "price";
     return roles;
 }

 void ProductModel::print(bool pdf, QDeclarativeItem *item, QString reportName)
 {
     QmlPrinter qmlPrinter(this);
     qmlPrinter.setReportName(reportName);
     qmlPrinter.print(pdf, item);
 }

 void ProductModel::addProduct(const Product &p_product)
 {
     beginInsertRows(QModelIndex(), rowCount(), rowCount());
     m_products << p_product;
     endInsertRows();
     emit dataChange();
 }

 int ProductModel::rowCount(const QModelIndex &) const {
     return m_products.count();
 }

 QVariant ProductModel::data(const QModelIndex & index, int role) const {
     if (index.row() < 0 || index.row() > m_products.count())
         return QVariant();

     const Product &product = m_products[index.row()];
     if (role == ProductRole)
         return product.name();
     else if (role == PriceRole)
         return product.price();
     return QVariant();
 }
