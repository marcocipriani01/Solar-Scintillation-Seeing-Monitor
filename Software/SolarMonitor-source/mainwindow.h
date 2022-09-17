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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h"
#include <ssmondevice.h>
#include <config.h>
#include <logfile.h>
#include <ssmonserver.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void setupRealtimeLoop(QCustomPlot *customPlot);
    void fillDevices();
    void fillbaudRates();
    void readSettings();
    void adaptSettings();
    void saveSettings();
    void cleanUp();
    QString whichDevice();



private slots:

  void on_pushButton_clicked();
  void resizeEvent(QResizeEvent* event);
  void on_refreshPorts_clicked();
  void on_seeIngRange_valueChanged(int arg1);

  void on_actionAbout_Qt_triggered();
  void on_actionAbout_ssMon_triggered();
  void on_actionConfiguration_triggered();


  void on_inputReceived(double);
  void on_seeingReceived(double);
  void on_deviceError(const QString &s);
  void on_Error(const QString &s);
  void on_Info(const QString &s);
  void on_generalError(const QString &s);
//  void on_netInput(const QString &s);



  void on_actionEnable_Logging_triggered(bool checked);



private:
  Ui::MainWindow *ui;
  ssMonDevice devThread;
  ssMonServer pServer;
  logFile logger;
  bool portOpen = false;
  bool piConnected = false;
  bool autoCapturing = false;
  bool Capturing = false;
  bool pLogging = false;
  int numCaptured = 0;
  double saved_seeing = 0.;
  double saved_input = 0.;
  double min_captured_seeing = 0.;
  double max_captured_seeing = 0.;
  double avg_captured_seeing = 0.;
  double sum_captured_seeing = 0.;
  double captured_samples = 0.;
  ssmconfig ssmonConfigs;

  QTimer dataTimer;
  QPalette savePallet;
 };

#endif // MAINWINDOW_H
