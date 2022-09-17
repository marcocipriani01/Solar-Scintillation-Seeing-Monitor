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

#include <QSettings>
#include <QDir>
#include <QString>
#include <config.h>

#define SSMTRIGGER "ssmTrigger"
#define SSMTHRESHOLD "ssmThreshold"
#define SSMTCP "ssmTCP"
#define SSMLOGPATH "ssmLogPath"
#define SSMLOGENABLE "ssmLogEnable"
#define SSMDEVICE "ssmLastDevice"

ssMonConfig::ssMonConfig()
{

}

ssmconfig ssMonConfig::getConfig(ssmconfig s)
{
    QSettings settings;
    s.ssmTrigger = (settings.value(SSMTRIGGER, .8)).toDouble();
    s.ssmThreshold = (settings.value(SSMTHRESHOLD, .8)).toDouble();
    s.ssmTCP =(settings.value(SSMTCP, 50001)).toString().toUShort();
    s.logPath =(settings.value(SSMLOGPATH, (QDir::homePath()))).toString();
    s.ssmEnabledLog =(settings.value(SSMLOGENABLE, false)).toBool();
    s.lastDevice = (settings.value(SSMDEVICE, "").toString());
    return s;
}

void ssMonConfig::saveConfig(ssmconfig s)
{
    QSettings msettings;

    msettings.setValue(SSMDEVICE, s.lastDevice);
    msettings.setValue(SSMTRIGGER, s.ssmTrigger);
    msettings.setValue(SSMTHRESHOLD, s.ssmThreshold);
    msettings.setValue(SSMTCP, s.ssmTCP);
    msettings.setValue(SSMLOGPATH, s.logPath);
    msettings.setValue(SSMLOGENABLE, s.ssmEnabledLog);

}
