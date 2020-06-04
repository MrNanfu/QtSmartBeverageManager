#ifndef COMMUNICATION_H
#define COMMUNICATION_H


#include <QTcpSocket>
#include <QObject>

class communication : public QObject
{
public:
    communication(QObject *parent = 0);

    ~communication();

    void setIp(QString ip);

    void setPort(qint16 port);

    QString getIp();

    qint16 getPort();

    QTcpSocket* getTcpSocket();

    /** 建立连接 */
    void link();

    /** 断开连接 */
    void breakLink();

    /** 是否连接 */
    bool isLinked();

    /** 是否断开连接 */
    bool isBreakLinked();

    /** 发送数据 */
    void write(QString str);

    /** 获取信息 */
    QByteArray readAll();

private:
    /** 通信套接字 */
    QTcpSocket *tcpSocket;

    /** ip地址 */
    QString ip = "127.0.0.1";

    /** 端口 */
    qint16 port = 8888;

};

#endif // COMMUNICATION_H
