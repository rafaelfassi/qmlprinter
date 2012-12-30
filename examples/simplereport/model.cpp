#include "model.h"
#include <QDebug>
#include "qmlprinter.h"
#include <QListView>



 Animal::Animal(const QString &type, const QString &size)
     : m_type(type), m_size(size)
 {
 }

 QString Animal::type() const
 {
     return m_type;
 }

 QString Animal::size() const
 {
     return m_size;
 }

 AnimalModel::AnimalModel(QObject *parent)
     : QAbstractListModel(parent)
 {
     QHash<int, QByteArray> roles;
     roles[TypeRole] = "produto";
     roles[SizeRole] = "preco";
     setRoleNames(roles);
 }

 void AnimalModel::print(bool pdf, QDeclarativeItem *item, QString reportName)
 {
     QmlPrinter qmlPrinter(this);
     qmlPrinter.setReportName(reportName);
     qmlPrinter.print(pdf, item);
 }

 void AnimalModel::addAnimal(const Animal &animal)
 {
     beginInsertRows(QModelIndex(), rowCount(), rowCount());
     m_animals << animal;
     endInsertRows();
     emit dataChange();
 }

 int AnimalModel::rowCount(const QModelIndex &) const {
     return m_animals.count();
 }

 QVariant AnimalModel::data(const QModelIndex & index, int role) const {
     if (index.row() < 0 || index.row() > m_animals.count())
         return QVariant();

     const Animal &animal = m_animals[index.row()];
     if (role == TypeRole)
         return animal.type();
     else if (role == SizeRole)
         return animal.size();
     return QVariant();
 }
