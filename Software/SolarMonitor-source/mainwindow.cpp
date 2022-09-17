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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QMessageBox>
#include <QMetaEnum>
#include <iostream>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <ssmondevice.h>
#include <configdialog.h>
#include <string>
#include "version.h"
#include <QDateTime>
#include <QDateTimeEdit>
#include <ssmonserver.h>


#define ABORT true

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)

{
    connect(&devThread, &ssMonDevice::seeing, this, &MainWindow::on_seeingReceived);
    connect(&devThread, &ssMonDevice::input, this, &MainWindow::on_inputReceived);
    connect(&devThread, &ssMonDevice::error, this, &MainWindow::on_deviceError);
    setlocale(LC_ALL, "C");
    readSettings();
    if (pServer.startserver(ssmonConfigs.ssmTCP))    ui->setupUi(this);
    else
        return;
    adaptSettings();




    ui->seeIngRange->setRange(4, 10);
    ui->seeIngRange->setValue(6);

    ui->inputValue->setAutoFillBackground(true);
    ui->seeingValue->setAutoFillBackground(true);
    fillDevices();
    fillbaudRates();
    setupRealtimeLoop(ui->qCustomPlot);
    setWindowTitle("Solar Scintillation Monitor");

    ui->qCustomPlot->replot();
}

MainWindow::~MainWindow()
{
    saveSettings();
    cleanUp();
    delete ui;
}

void MainWindow::cleanUp()
{
    if (devThread.isRunning()) devThread.terminate();
    pServer.sendCommand("#DISCONNECT$\n");
    pServer.endserver();

    QTime dieTime= QTime::currentTime().addSecs(1);
     while (QTime::currentTime() < dieTime)
         QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

}

QString MainWindow::whichDevice()
{
    return ui->deviceSelector->currentText();
}
void MainWindow::setupRealtimeLoop(QCustomPlot *customPlot)
{

#if QT_VERSION < QT_VERSION_CHECK(4, 7, 0)
  QMessageBox::critical(this, "", "You're using Qt < 4.7, this program needs functions that are available with Qt 4.7 to work properly");
#endif

  QPen mPen;
  customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);
  customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
  mPen.setWidth(1);
  mPen.setColor(Qt::blue);
  customPlot->graph(0)->setPen(mPen);

  customPlot->addGraph();

  mPen.setWidth(1);
  mPen.setColor(Qt::darkGreen);
  customPlot->graph(1)->setPen(mPen);
  customPlot->addGraph();

  mPen.setWidth(1);
  mPen.setColor(Qt::darkRed);
  customPlot->graph(2)->setPen(mPen);


  customPlot->xAxis->setTickLabelRotation(-90);
  customPlot->yAxis2->setRange(0, 2);
  customPlot->yAxis2->setVisible(true);

  customPlot->yAxis2->setLabel("Input Value");
  customPlot->yAxis->setLabel("Seeing in arcsec");
  QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);

  timeTicker->setDateTimeFormat("hh:mm:ss");
  customPlot->xAxis->setTicker(timeTicker);

  customPlot->yAxis2->setTickLabelColor(Qt::blue);

  customPlot->yAxis2->setLabelColor(Qt::blue);
  customPlot->yAxis->setLabelColor(Qt::darkGreen);


  connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

void MainWindow::on_seeingReceived(double seeing)
{
    static double movingAverageCount = 1;
    static double movingAverageAcc = 0;
    static double  vAverage = 0;



    static long long firstPointKey=QDateTime::currentMSecsSinceEpoch()/1000;
    QDateTime Now=QDateTime::currentDateTimeUtc();

    QString sDate = Now.toString(Qt::ISODate);//WithMs);
    sDate.replace("Z", " UTC");
    long long mSecs=Now.currentMSecsSinceEpoch();
    long long key = mSecs/1000;

    char tmp[10];
    if (pServer.isConnected() && portOpen)
    {
       if (seeing < ssmonConfigs.ssmTrigger && seeing > 0.0)
       {
          pServer.sendCommand(QString ("#S1$%1\n").arg(seeing));
          Capturing = true;
       }
       else
           if (seeing > ssmonConfigs.ssmThreshold)
           {
               pServer.sendCommand(QString ("#S0$%1\n").arg(seeing));
               Capturing = false;
           }
           else
               if (Capturing) pServer.sendCommand(QString ("#S1$%1\n").arg(seeing));
    }

    std::sprintf(tmp, "%6.2f\"", seeing);
    saved_seeing = seeing;
    ui->seeingValue->setText(QString::fromLatin1(tmp));

    if (seeing <= 0.0) ui->meterSeeing->setStyleSheet( "background-color: "
                                                       "qlineargradient(x1: 0, x2: 1, stop: 0 white)");
    else if (seeing <= 1.3) ui->meterSeeing->setStyleSheet( "background-color: "
                            "qlineargradient(x1: 0, x2: 1, stop: 0 green, stop: 0.3 white)");
    else if (seeing <= 1.9 ) ui->meterSeeing->setStyleSheet( "background-color: "
                            "qlineargradient(x1: 0, x2: 1, stop: 0 green, stop: 0.3 yellow, stop: 0.5 white)");

    else if (seeing <= 2.5) ui->meterSeeing->setStyleSheet( "background-color: "
                            "qlineargradient(x1: 0, x2: 1, stop: 0 green, stop: 0.3 yellow, stop: 0.5 orange, stop: 0.7 white)");
    else ui->meterSeeing->setStyleSheet( "background-color: "
                            "qlineargradient(x1: 0, x2: 1, stop: 0 green, stop: 0.3 yellow, stop: 0.5 orange, stop: 0.7 red)");



    movingAverageAcc = movingAverageAcc + seeing;

    if ( key - firstPointKey > 60.0)
    {
        movingAverageAcc = movingAverageAcc - vAverage;
        vAverage = movingAverageAcc / movingAverageCount;
    }
    else
    {
         movingAverageCount++;
    }

    if (seeing > 0) ui->qCustomPlot->graph(1)->addData(key, seeing);
    if (vAverage > 0.0) ui->qCustomPlot->graph(2)->addData(key, vAverage);

    if (pLogging)
        logger.writeData(mSecs, sDate, saved_seeing, saved_input, vAverage);

    ui->qCustomPlot->xAxis->setRange(key+10, 600, Qt::AlignRight);
    ui->qCustomPlot->replot();
}


void MainWindow::on_inputReceived(double input)
{
    long long key=QDateTime::currentMSecsSinceEpoch()/1000;

    char tmp[10];
    if (pServer.isConnected() && portOpen)
            pServer.sendCommand(QString ("#IV$%1\n").arg(input));

    std::sprintf(tmp, "%6.2f V", input);
    saved_input = input;
    ui->inputValue->setText(QString::fromLatin1(tmp));

    if (input <= 0.0)   ui->meterInput->setStyleSheet( "background-color: "
                    "qlineargradient(x1: 0, x2: 1, stop: 0 white)");
    else if (input <= 0.49) ui->meterInput->setStyleSheet( "background-color: "
                   "qlineargradient(x1: 0, x2: 1, stop: 0 red, stop: 0.4 white)");
    else if (input <= 0.99) ui->meterInput->setStyleSheet( QString ("background-color: "
                   "qlineargradient(x1: 0, x2: 1, stop: 0 red, stop: 0.4 green, stop: %1 white)").arg(input - .1));
    else ui->meterInput->setStyleSheet( "background-color: "
                    "qlineargradient(x1: 0, x2: 1, stop: 0 red, stop: 0.3 lightgreen, stop: 0.7 red)");

    if (input > 0) ui->qCustomPlot->graph(0)->addData(key, input);

}


void MainWindow::fillDevices()
{
    ui->deviceSelector->clear();
#ifdef __linux__
    if (QFileInfo::exists(":dev/ssm"))
        ui->deviceSelector->addItem("ssm");
#endif
    QList<QSerialPortInfo> Devices = QSerialPortInfo::availablePorts();
    QList<QSerialPortInfo>::iterator i;
    for(i = Devices.begin(); i != Devices.end(); i++)
       ui->deviceSelector->addItem ((*i).portName().toStdString().data());
    ui->deviceSelector->addItem("Simulator");
    for (int d = 0; d < ui->deviceSelector->count(); d++)
        if (ui->deviceSelector->itemText(d) == ssmonConfigs.lastDevice) {
            ui->deviceSelector->setCurrentIndex(d);
            break;
        }

}

void MainWindow::fillbaudRates()
{
    ui->baudRate->addItems({ "9600", "19200", "38400", "57600", "115200", "230400" });
    ui->baudRate->setCurrentIndex(4);
}

void MainWindow::on_pushButton_clicked()
{
    if (portOpen)
    {
        devThread.stop();

        ui->pushButton->setText("Connect");
        ui->actionConnect->setChecked(false);
        pServer.sendCommand("#DISCONNECT$\n");
        if (pLogging)
        {
            logger.saveLog();
            pLogging = false;
        }

        portOpen = false;

        fillDevices();
    }
    else
    {
        if (ui->deviceSelector->count() > 0)
        {
            ui->inputValue->clear();
            ui->seeingValue->clear();
            ui->statusBar->showMessage(QString::fromStdString(""));
            ui->pushButton->setText("Disconnect");
            ui->actionConnect->setChecked(true);
            devThread.connectDevice( ui->deviceSelector->currentText(), 2000, ui->baudRate->currentText().toInt());
            ssmonConfigs.lastDevice = ui->deviceSelector->currentText();
            portOpen = true;
            if (ssmonConfigs.ssmEnabledLog)
            {
                logger.newLog(ssmonConfigs.logPath);
                pLogging = true;
            }
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   QMainWindow::resizeEvent(event);

   ui->infoMessages->setGeometry(20, ui->centralWidget->height() - 80 ,ui->centralWidget->width() - 40, 75);
   ui->qCustomPlot->setFixedSize((ui->centralWidget->width() -40), (ui->centralWidget->height() - ui->infoMessages->height() - 90));
   ui->qCustomPlot->adjustSize();
}



void MainWindow::on_refreshPorts_clicked()
{
    fillDevices();
}

void MainWindow::on_seeIngRange_valueChanged(int arg1)
{
     ui->qCustomPlot->yAxis->setRange(0, arg1);
     ui->qCustomPlot->yAxis2->setRange(0, 2);
     ui->qCustomPlot->replot();
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this, "Solar Scintillation Monitor");
}

void MainWindow::on_actionAbout_ssMon_triggered()
{
   QString Title = QString("%1 - Version: A-%2.%3.%4").arg(P_NAME).arg(MAJOR_VERSION).arg(MINOR_VERSION).arg(PATCH_VERSION);
   QString Text = QString("(C) 2019 - %1\n %2").arg(P_AUTHOR).arg(P_INFO);

    QMessageBox::about(this, Title, Text);
}

void MainWindow::on_actionConfiguration_triggered()
{
    configDialog conFigure;
    conFigure.exec();
    readSettings();
    adaptSettings();
}


void MainWindow::on_Info(const QString &s)
{
    ui->infoMessages->append(s);
    //ui->statusBar->showMessage(s);
}

void MainWindow::on_deviceError(const QString &s)
{
    ui->statusBar->showMessage(s);

}

void MainWindow::on_Error(const QString &s)
{
    ui->statusBar->showMessage(s);
}


void MainWindow::readSettings() {
    ssMonConfig cf;
    ssmonConfigs = cf.getConfig(ssmonConfigs);
}

void MainWindow::adaptSettings() {
        QString t = "Trigger: " + QString::number(ssmonConfigs.ssmTrigger, 2, 1) + "\nThreshold: " + QString::number(ssmonConfigs.ssmThreshold, 2, 1);
        ui->labelautocap_info->setText(t);
        ui->actionEnable_Logging->setChecked(ssmonConfigs.ssmEnabledLog);
}

void MainWindow::saveSettings()
{
    ssMonConfig cf;
    ssmonConfigs.lastDevice = whichDevice();
    cf.saveConfig(ssmonConfigs);
}

void MainWindow::on_generalError(const QString &s)
{
    QMessageBox::critical(this, "Error Occured", s);
}

void MainWindow::on_actionEnable_Logging_triggered(bool checked)
{
        ssMonConfig cf;
        ssmonConfigs.ssmEnabledLog = checked;
        cf.saveConfig(ssmonConfigs);
}


