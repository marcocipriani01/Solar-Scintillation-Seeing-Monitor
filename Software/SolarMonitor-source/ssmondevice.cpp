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

#include "ssmondevice.h"
#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <iostream>
#include <string>

namespace CommProtocol
{
    // Standard identifiers
    const char *Input  = "A0";
    const char *Seeing = "A1";
    // Additional identifiers
    const char *Samples = "D0";
    const char *LongSeeing = "C2";
}

QT_USE_NAMESPACE

ssMonDevice::ssMonDevice(QObject *parent)
    : QThread(parent), waitTimeout(0), quit(false), breakloop(false), isSimulator(false)
{
}

ssMonDevice::~ssMonDevice()
{
    mutex.lock();
    quit = true;
    cond.wakeOne();
    mutex.unlock();
    wait();
}

void ssMonDevice::connectDevice(const QString &portName, int waitTimeout, int baud)
{
    QMutexLocker locker(&mutex);
    this->portName = portName;
    this->waitTimeout = waitTimeout;
    this->baudrate = baud;
    this->quit = false;
    this->breakloop = false;
    if (!isRunning())
        start();
    else
        cond.wakeOne();
}

void ssMonDevice::stop()
{
    breakloop=true;
    quit=true;
}

void ssMonDevice::run()
{
    mutex.lock();
    QString currentPortName = portName;
    int currentWaitTimeout = waitTimeout;
    int currentBaudrate = baudrate;
    mutex.unlock();

    QSerialPort serial;
    std::string lineIn, line, valInput, valSeeing;
    double vInput, vSeeing;
    while (!quit)
    {
        int firstTime = 3;
        //serial.close();
        if (currentPortName == "Simulator")
            isSimulator = true;
        else
        {
            serial.setPortName(currentPortName);
            serial.setBaudRate(currentBaudrate);
            isSimulator = false;
            if (!serial.open(QIODevice::ReadWrite))
            {
                emit error(tr("Can't open %1, error code %2")
                           .arg(portName).arg(serial.error()));
                return;
            }
            serial.setFlowControl(QSerialPort::SoftwareControl);
            serial.setRequestToSend(true);
        }
        if (!isSimulator) serial.clear();
        else srand ((unsigned)time(nullptr));

        while (!breakloop)
        {
            if (isSimulator)
            {
                sleep (1);

                int iRand = rand() % 400 + 100;
                double iVal = (iRand / 100.) +  (double ((iRand % 100)) / 100.);
                emit seeing(iVal);
                iVal = (rand() % 40 + 57.) / 100.;
                emit input(iVal);
            }
            else
            {
                if (serial.waitForReadyRead(currentWaitTimeout * firstTime))
                {
                    QByteArray responseData = serial.readAll();
                    while (serial.waitForReadyRead(10))
                        responseData += serial.readAll();
                    firstTime = 1;
                    QString response(responseData);
                    QStringList list = response.split("\r");
                    for (int i=0; i < list.count(); i++)
                    {
                        QString tmp=list.at(i);
                        tmp.replace("\n", "");
                        if (!tmp.trimmed().isEmpty())
                        {
                            lineIn = tmp.toStdString().data();


                            if (lineIn.size() == 6 && lineIn.substr(5, 1) == "$")
                            {
                                if (lineIn.substr(0, 1) == "A")
                                line = "A1: " + lineIn.substr(1, 4);
                                else line = "A0: " + lineIn.substr(1, 4);
                            }
                            else line = lineIn;
                            if (line.size() == 8)
                            {
                                if (line.substr(0, 2) == CommProtocol::Input)
                                {
                                    valInput = line.substr(4);
                                    vInput = std::stod (valInput.data());
                                    emit input(vInput);

                                }
                                else if (line.substr(0, 2) == CommProtocol::Seeing)
                                {
                                    valSeeing = line.substr(4);
                                    vSeeing = std::stod (valSeeing.data() );
                                    emit seeing(vSeeing);

                                }
                            }
                        }
                    }
                }
                else
                {

                   if (firstTime > 1)
                   {
                       breakloop = true;
                       quit=true;
                       emit error("Unable to connect to device");
                   }
                   else emit error("Read response timeout");
                }
            }

        }
        if (!isSimulator) serial.close();
    }
}
