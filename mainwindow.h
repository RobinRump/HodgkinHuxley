/***************************************************************************
**                                                                        **
**  HodgkinHuxley Simulator                                               **
**  Copyright (C) 2013 Robin Rump                                         **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Robin Rump                                           **
**  Website/Contact: http://www.robinrump.com/                            **
**             Date: 28.08.13                                             **
**          Version: 1.0.0                                                **
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qcustomplot.h"
#include "settings.h"
#include "welcome.h"

#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QFile>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStandardPaths>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <QXmlStreamWriter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum Modes {
        currentManual   = 0,
        currentImpulse  = 1,
        currentFunction = 2
    };

    enum Graphs {
        graphMembrane = 0,
        graphCurrent  = 1,
        graphN        = 2,
        graphM        = 3,
        graphH        = 4
    };

    enum Files {
        fileXml  = 0,
        fileJson = 1,
        filePng  = 2,
        fileJpg  = 3,
        filePdf  = 4
    };

    void init();

    QJsonObject fromConfig();
    bool toConfig(QJsonObject j);

    void resizeEvent(QResizeEvent* event);

private:
    Ui::MainWindow *ui;

    int interval, j;
    double dt;

    double VRest, Cm, gMaxNa, gMaxK, gMaxL, gNa, gK, gL, ENa, EK, EL;
    QVector<double> time, V, I, blank;
    double m, n, h;

    QVector<double> mh, nh, hh;

    int cI, cMode;
    int impulseDuration, impulseStart;

    bool isPaused;
    bool isNShown, isMShown, isHShown;

    QTimer *timer;
    Settings *s;
    Welcome *w;

    QFile *config;

    QVector<QVector <double> *> values;

    double alphaN(double v); double betaN(double v);
    double alphaM(double v); double betaM(double v);
    double alphaH(double v); double betaH(double v);

public slots:
    void updatePreferences();
    int minCurrentValue();

private slots:
    void updatePlot();
    void updateCurrent();
    void updateGNa();
    void updateGK();
    void updateGL();

    void showN();
    void showM();
    void showH();

    void pause();
    void unpause();
    void switchPause();
    void reset();
    void clear();
    void changeCurrentMode(int m);

    void settings();
    void about();
    void welcome();

    void toJson();
    void toXml();
    void toPng();
    void toJpg();
    void toPdf();
};

#endif // MAINWINDOW_H
