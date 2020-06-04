#ifndef COMMUNICATION_H
#define COMMUNICATION_H


#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <QObject>
#include<QList>

class communication : public QObject
{
public:
    communication(QObject *parent = 0);

    ~communication();

    void setTcpSocket(QTcpSocket *tcpSocket);

    QTcpServer* getTcpServer();

    QTcpSocket* getTcpSocket();

    /** 开始监听 */
    void listen();

    /** 新的连接 */
    QString newLink();

    /** 是否有连接 */
    bool isLinked();

    /** 断开连接 */
    QString breakLink();

    /** 读取所有信息 */
    QByteArray readAll();

    /** 发送数据 */
    void write(QString str);

    /** 客户对象列表 */
    QList<QTcpSocket*> clients;

    /** 客户对象列表的数量 */
    int size = 0;

private:
    /** 监听套接字 */
    QTcpServer *tcpServer = nullptr;

    /** 通信套接字 */
    QTcpSocket *tcpSocket = nullptr;
};

#endif // COMMUNICATION_H
