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
#ifndef LOGFILE_H
#define LOGFILE_H

#include <QObject>
#include <QFile>

class logFile : public QObject
{
    Q_OBJECT
public:
    explicit logFile(QObject *parent = nullptr);
    void newLog(const QString path);
    void writeData(long long timestamp, QString now, double seeing, double input, double average, QString Txt = "", double Best = 0., double Worst = 0., double Avg = 0.);
    void saveLog();

signals:

public slots:

private:
    QFile logF;
};

#endif // LOGFILE_H
