/*
SolarMonitor - Solar Seeing Scintillation Monitor
Copyright (C) 2019 Paul de Backer

This file is part of SolarMonitor.

SolarMonitor is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

SolarMonitor is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ssMon.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ssmonserver.h"
#include <QTime>
#include <iostream>
#include <string>
#include <QTcpSocket>
#include <QTcpServer>
#include <QByteArray>

QT_USE_NAMESPACE

ssMonServer::ssMonServer(QObject *parent)
    : QObject(parent)
{
    connected = false;
}


ssMonServer::~ssMonServer()
{

}

void ssMonServer::endserver()
{
    foreach (QTcpSocket *socket, sockets){
        socket->close();}
    tcpServer->close();
}

bool ssMonServer::startserver(quint16 port)
{
    tcpServer = new QTcpServer(this);



    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
    if (!tcpServer->listen(QHostAddress::Any, port))
    {
        std::cout << tcpServer->errorString().toStdString().c_str() << std::endl;
        return false;
    }
    return true;
}


bool ssMonServer::isConnected()
{
    return connected;
}

void ssMonServer::sendCommand(QString cmd)
{
  foreach (QTcpSocket *socket, sockets)
  {
    socket->write(cmd.toLocal8Bit());
    socket->flush();
  }
}

void ssMonServer::closeConnection()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender());
    sockets.remove(clientSocket);
    if (sockets.count() == 0) connected = false;

    return;
}

void ssMonServer::newConnection()
{

   QTcpSocket *socket = tcpServer->nextPendingConnection();
   connect (socket, SIGNAL(disconnected()), this, SLOT(closeConnection()));
   connect (socket, SIGNAL(readyRead()), this, SLOT(inputConnection()));
   sockets.insert(socket);


   connected = true;
}

void ssMonServer::inputConnection()
{
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(QObject::sender());

    QByteArray msg = clientSocket->readAll();

    emit(input(QString::fromUtf8(msg)));

}
