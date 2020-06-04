#include "communication.h"
#include <QHostAddress>

communication::communication(QObject *parent) : QObject(parent)
{
    this->tcpSocket = new QTcpSocket(this);
}

communication::~communication()
{
    delete this->tcpSocket;
    this->tcpSocket = nullptr;
}

void communication::setIp(QString ip)
{
    this->ip = ip;
}

void communication::setPort(qint16 port)
{
    this->port = port;
}

QString communication::getIp()
{
    return this->ip;
}

qint16 communication::getPort()
{
    return this->port;
}

QTcpSocket* communication::getTcpSocket()
{
    return this->tcpSocket;
}

void communication::link()
{
    this->tcpSocket->connectToHost(QHostAddress(this->ip), this->port);
}

void communication::breakLink()
{
    this->tcpSocket->disconnectFromHost();
    this->tcpSocket->close();
}

bool communication::isLinked()
{
    if (QAbstractSocket::ConnectedState != this->tcpSocket->state())
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool communication::isBreakLinked()
{
    if (QAbstractSocket::UnconnectedState == tcpSocket->state())
    {
        return true;
    }
    else
    {
        return false;
    }
}

void communication::write(QString str)
{
    this->tcpSocket->write( str.toUtf8().data() );
}

QByteArray communication::readAll()
{
    return this->tcpSocket->readAll();
}
