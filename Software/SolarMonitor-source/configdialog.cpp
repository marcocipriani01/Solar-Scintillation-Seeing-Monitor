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

#include "configdialog.h"
#include "ui_configdialog.h"
#include "config.h"
#include <QString>
#include <QFileDialog>
#include <mainwindow.h>

configDialog::configDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configDialog)
{
    ui->setupUi(this);
    ssmconfig cfg;
    ssMonConfig cf;
    cfg = cf.getConfig(cfg);
    ui->autoCapTrigger->setValue(cfg.ssmTrigger);
    ui->discardTrigger->setValue(cfg.ssmThreshold);
    ui->autoCapTCP->setText(QString::number(cfg.ssmTCP));
    ui->lblLogPath->setText(cfg.logPath);
    ui->logEnabled->setChecked(cfg.ssmEnabledLog);
}

configDialog::~configDialog()
{
    delete ui;
}





void configDialog::on_buttonBox_accepted()
{
    ssmconfig cfg;
    ssMonConfig cf;  
    cfg.ssmTrigger = ui->autoCapTrigger->value();
    cfg.ssmThreshold = ui->discardTrigger->value();
    if (cfg.ssmThreshold < cfg.ssmTrigger) cfg.ssmThreshold = cfg.ssmTrigger;
    cfg.ssmTCP = ui->autoCapTCP->text().toUShort();
    cfg.logPath = ui->lblLogPath->text();
    cfg.ssmEnabledLog = ui->logEnabled->checkState();
    cf.saveConfig(cfg);
}

void configDialog::on_pbLogPath_clicked()
{
    QString dirName = QFileDialog::getExistingDirectory(this, "Data logging directory", ui->lblLogPath->text(), QFileDialog::ShowDirsOnly );
    if (!dirName.isEmpty()) ui->lblLogPath->setText(dirName);
}
