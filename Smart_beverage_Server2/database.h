#ifndef DATABASE_H
#define DATABASE_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QList>
#include <QMap>
#include <QVariant>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    ~Database();
    /** 数据库 */
    QSqlDatabase db;

    QSqlQuery *query = nullptr;

    /** 连接数据库 */
    bool connect(QString name);

    /** 给query分配空间 */
    void upQuery();
};

#endif // DATABASE_H
