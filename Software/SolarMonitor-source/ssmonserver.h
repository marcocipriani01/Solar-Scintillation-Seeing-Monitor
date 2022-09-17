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

#ifndef SSMONSERVER_H
#define SSMONSERVER_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTcpServer>


class ssMonServer : public QObject
{
    Q_OBJECT

public:
    explicit ssMonServer(QObject *parent = nullptr);
    ~ssMonServer();

    bool startserver(quint16 port);
    void endserver();
    bool isConnected();
    void sendCommand(QString cmd);

//    void connectServer(int port);
//    void run() Q_DECL_OVERRIDE;
//    void stop();

signals:
//    void seeing(double);
//    void input(double);
//    void response(const QString &s);
//    void error(const QString &s);
//    void timeout(const QString &s);
    void input(const QString &s);

public  slots:
    void newConnection();
    void closeConnection();
    void inputConnection();


private:
    QTcpServer *tcpServer;
    //QTcpSocket *socket;
    QSet<QTcpSocket *> sockets;
    bool  connected = false;
    bool quit;
    bool breakloop;
    bool isSimulator;
    int baudrate;
};
#endif // SSMONSERVER_H
