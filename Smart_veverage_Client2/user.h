#ifndef USER_H
#define USER_H
#include <QString>


class User
{
public:
    User();
    QString name;

    QString money;

    QString type;

    QString is_online = "0";
};

#endif // USER_H
