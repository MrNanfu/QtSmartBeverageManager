#include "communication.h"


communication::communication(QObject *parent) : QObject(parent)
{
    this->tcpServer = new QTcpServer(this);
}

communication::~communication()
{
    delete this->tcpServer;
    this->tcpServer = nullptr;
}

/** 设置socket套接字 */
void communication::setTcpSocket(QTcpSocket *tcpSocket)
{
    this->tcpSocket = tcpSocket;
}

/** 得到servert套接字 */
QTcpServer* communication::getTcpServer()
{
    return this->tcpServer;
}

/** 得到socket套接字 */
QTcpSocket* communication::getTcpSocket()
{
    return this->tcpSocket;
}

/** sever监听端口 */
void communication::listen()
{
    this->tcpServer->listen(QHostAddress::Any, 8888);
}

/** socket与服务器断开连接 */
QString communication::breakLink()
{
    return QString("[%1:%2]:成功断开").arg(tcpSocket->peerAddress().toString()).arg(tcpSocket->peerPort());
}

/** 服务器开始等待客户端的连接 */
QString communication::newLink()
{
    /** 取出建立好连接的套接字 */
    this->tcpSocket = tcpServer->nextPendingConnection();
    clients.push_back(this->tcpSocket);
    this->size++;
    /** 获取对方的IP和端口 */
    QString ip = tcpSocket->peerAddress().toString();
    qint16 port = tcpSocket->peerPort();
    return QString("[%1:%2]:成功连接").arg(ip).arg(port);
}

/** socket的read函数 */
QByteArray communication::readAll()
{
    return this->tcpSocket->readAll();
}

/** socket的writed函数 */
void communication::write(QString str)
{
    if (nullptr != this->tcpSocket)
    {
        tcpSocket->write( str.toUtf8().data() );
    }
}

