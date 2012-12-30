#include <QAbstractListModel>
#include <QStringList>
#include <QDeclarativeItem>

 class Product
 {
 public:
     Product(const QString &p_productName, const QString &p_price);

     QString name() const;
     QString price() const;

     QString m_name;
     QString m_price;

 };

 class ProductModel : public QAbstractListModel
 {
     Q_OBJECT

     Q_PROPERTY(int count READ rowCount NOTIFY dataChange)
 public:
     enum ProductRoles {
         ProductRole = Qt::UserRole + 1,
         PriceRole
     };

     ProductModel(QObject *parent = 0);

     Q_INVOKABLE void print(bool pdf, QDeclarativeItem *item, QString reportName);

     void addProduct(const Product &p_product);

     int rowCount(const QModelIndex & parent = QModelIndex()) const;

     QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

 signals:
     void dataChange();

 private:
     QList<Product> m_products;
 };
