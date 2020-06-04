#include "database.h"
#include <QSqlError>
#include <QVariantList>
#include <QVariant>
#include <QDateTime>
#include <QDebug>

Database::Database(QObject *parent) : QObject(parent)
{

}

Database::~Database()
{
    if (nullptr != this->query)
    {
        delete this->query;
        this->query = nullptr;
    }
}

/** 跟数据库建立连接 */
bool Database::connect(QString name)
{
    this->db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(name);
    if( !db.open() )
    {
        return false;

    }
    else
    {
        return true;
    }
}

/** query实例化 */
void Database::upQuery()
{
    if (nullptr == this->query)
    {
        this->query = new QSqlQuery(this->db);
    }
}
