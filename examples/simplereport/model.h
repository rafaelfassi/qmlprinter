#include <QAbstractListModel>
#include <QStringList>
#include <QDeclarativeItem>
//#include <qdeclarative.h>

#define declare(type, name) \
private: type m_##name; \
public: \
void set##name(const type& var) \
{\
        m_##name = var;\
}\
\
type name() \
{\
        return m_##name; \
}\

 class Animal
 {
 public:
     Animal(const QString &type, const QString &size);

     QString type() const;
     QString size() const;

     QString m_type;
     QString m_size;

 };

 class AnimalModel : public QAbstractListModel
 {
     Q_OBJECT

     Q_PROPERTY(int count READ rowCount NOTIFY dataChange)
 public:
     enum AnimalRoles {
         TypeRole = Qt::UserRole + 1,
         SizeRole
     };

     AnimalModel(QObject *parent = 0);

     Q_INVOKABLE void print(bool pdf, QDeclarativeItem *item, QString reportName);

     void addAnimal(const Animal &animal);

     int rowCount(const QModelIndex & parent = QModelIndex()) const;

     QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

 signals:
     void dataChange();

 private:
     QList<Animal> m_animals;
     declare(int, var1)
     declare(int, var2)
 };
