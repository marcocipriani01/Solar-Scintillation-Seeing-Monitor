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

#ifndef CONFIG_H
#define CONFIG_H
#include <QString>

struct ssmconfig
{
    double ssmTrigger;
    double ssmThreshold;
    quint16 ssmTCP;
    bool ssmEnabledLog;
    QString logPath;
    QString lastDevice;
};

class ssMonConfig
{
public:
    ssMonConfig();
    ssmconfig getConfig(ssmconfig);
    void saveConfig(ssmconfig);

private:

};

#endif // CONFIG_H
