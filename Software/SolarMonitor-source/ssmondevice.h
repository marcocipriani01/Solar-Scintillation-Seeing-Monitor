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

#ifndef SSMONDEVICE_H
#define SSMONDEVICE_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>


class ssMonDevice : public QThread
{
    Q_OBJECT

public:
    explicit ssMonDevice(QObject *parent = nullptr);
    ~ssMonDevice();

    void connectDevice(const QString &portName, int waitTimeout, int baud);
    void run() Q_DECL_OVERRIDE;
    void stop();

signals:
    void seeing(double);
    void input(double);
    void response(const QString &s);
    void error(const QString &s);
    void timeout(const QString &s);

private:
    QString portName;
    int waitTimeout;
    QMutex mutex;
    QWaitCondition cond;
    bool quit;
    bool breakloop;
    bool isSimulator;
    int baudrate;
};

#endif // SSMONDEVICE_H
