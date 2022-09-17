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
#include "logfile.h"
#include <QFile>
#include <QTime>
#include <QByteArray>
#include <QString>
#include <QDateTime>
#include <QString>

logFile::logFile(QObject *parent) : QObject(parent)
{

}

void logFile::newLog(const QString path)
{
    logF.setFileName(path + "/" + (QDateTime::currentDateTime().toString("yyyyMMddThhmmss.log")));
    logF.open( QIODevice::WriteOnly);
}

void logFile::saveLog()
{
    logF.close();
}

void logFile::writeData(long long timestamp, QString now, double seeing, double input, double average, QString Txt, double Best, double Worst, double Avg)

{
    QByteArray tmp;
    tmp.append(QString::asprintf("%llu;%s;%6.2lf;%6.2lf;%6.2lf;", timestamp, now.toStdString().c_str(),   seeing, input, average));
    if (!Txt.isEmpty()) tmp.append(Txt);
    if (Best > 0.0) tmp.append(QString::asprintf(";%6.2lf;%6.2lf;%6.2lf", Best, Worst, Avg));
    tmp.append("\n");
    logF.write(tmp);
}
