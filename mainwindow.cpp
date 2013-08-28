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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("HodgkinHuxley Simulator - Robin Rump");

    this->timer = new QTimer(this);

    this->blank.resize(1);
    this->dt       = 0.025;  // ms
    this->interval = 10;     // ms
    this->isPaused = false;
    this->isNShown = false;
    this->isMShown = false;
    this->isHShown = false;
    this->cMode    = currentManual;

    this->VRest  = 0;      // mV
    this->Cm     = 1;      // uF/cm2
    this->gMaxNa = 120;    // mS/cm2
    this->gMaxK  = 36;     // mS/cm2
    this->gMaxL  = 0.3;    // mS/cm2
    this->ENa    = 115;    // mS/cm2
    this->EK     = -12;    // mS/cm2
    this->EL     = 10.613; // mS/cm2

    ui->gNaSlider->setValue(this->gMaxNa);
    ui->gNaValue->setText(QString::number(this->gMaxNa));

    ui->gKSlider->setValue(this->gMaxK);
    ui->gKValue->setText(QString::number(this->gMaxK));

    ui->gLSlider->setValue(this->gMaxL*10);
    ui->gLValue->setText(QString::number(this->gMaxL));

    this->init();
    this->s = new Settings(this);
    this->w = new Welcome(this);

    // plot config
    ui->plot->setInteraction(QCP::iRangeDrag, true);
    ui->plot->setInteraction(QCP::iRangeZoom, true);
    ui->plot->plotLayout()->insertRow(0);
    ui->plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->plot, "Hodgkin-Huxley"));

    // graphes
    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);
    ui->plot->graph(0)->setData(this->time, this->V);    

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);
    ui->plot->graph(1)->setPen(QPen(QColor(255, 100, 0)));
    ui->plot->graph(1)->setData(this->time, this->I);

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2); // n
    ui->plot->graph(2)->setPen(QPen(QColor(255, 0, 0)));
    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2); // m
    ui->plot->graph(3)->setPen(QPen(QColor(0, 255, 0)));
    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2); // h
    ui->plot->graph(4)->setPen(QPen(QColor(0, 0, 255)));

    // axes configuration
    ui->plot->xAxis->setLabel("Time in ms");
    ui->plot->yAxis->setLabel("Membran potential in mV");
    ui->plot->yAxis2->setLabel("Gatting channels");

    // ranges
    ui->plot->xAxis->setRange(0, 100);
    ui->plot->yAxis->setRange(-50, 150);
    ui->plot->yAxis2->setRange(0, 1);
    ui->plot->replot();

    // settings
    QJsonDocument document;
    QJsonObject json;
    QFile file(QDir::toNativeSeparators(QDir::homePath() + "/HH/config.json"));
    QByteArray bytes;
    if (file.exists()) {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        bytes = file.readAll();
        file.close();
        document.fromBinaryData(bytes);
        json = document.object();
        qDebug() << json;
    } else {
        json.insert("startup", true);
        document.setObject(json);
        bytes = document.toJson();
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.write(bytes);
        file.close();
    }

    connect(ui->currentSlider, SIGNAL(valueChanged(int)), this, SLOT(updateCurrent()));
    connect(ui->gNaSlider, SIGNAL(valueChanged(int)), this, SLOT(updateGNa()));
    connect(ui->gKSlider, SIGNAL(valueChanged(int)), this, SLOT(updateGK()));
    connect(ui->gLSlider, SIGNAL(valueChanged(int)), this, SLOT(updateGL()));

    connect(ui->checkN, SIGNAL(stateChanged(int)), this, SLOT(showN()));
    connect(ui->checkM, SIGNAL(stateChanged(int)), this, SLOT(showM()));
    connect(ui->checkH, SIGNAL(stateChanged(int)), this, SLOT(showH()));

    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(ui->impulseButton, SIGNAL(clicked(bool)), signalMapper, SLOT(map())) ;
    signalMapper->setMapping(ui->impulseButton, currentImpulse);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(changeCurrentMode(int)));

    connect(ui->pause, SIGNAL(clicked(bool)), this, SLOT(pause()));
    connect(ui->reset, SIGNAL(clicked(bool)), this, SLOT(reset()));
    connect(ui->clear, SIGNAL(clicked(bool)), this, SLOT(clear()));

    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(settings()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionJson, SIGNAL(triggered(bool)), this, SLOT(toJson()));
    connect(ui->actionWelcome, SIGNAL(triggered(bool)), this, SLOT(welcome()));

    connect(this->timer, SIGNAL(timeout()), this, SLOT(updatePlot()));
    this->timer->start(this->interval);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    this->j        = 1;
    this->time.resize(this->j);
    this->time[0]  = 0;

    this->V.resize(this->j); // mV
    this->V[0] = this->VRest;
    this->n = this->alphaN(V[0])/(this->alphaN(V[0]) + this->betaN(V[0]));
    this->m = this->alphaM(V[0])/(this->alphaM(V[0]) + this->betaM(V[0]));
    this->h = this->alphaH(V[0])/(this->alphaH(V[0]) + this->betaH(V[0]));

    this->I.resize(this->j); // mA
    this->I[0] = 0;
    this->cI = 0;
    ui->currentSlider->setValue(this->cI);
    ui->currentValue->setText(QString::number(this->cI));

    this->nh.resize(this->j);
    this->nh[0] = this->n;
    this->mh.resize(this->j);
    this->mh[0] = this->m;
    this->hh.resize(this->j);
    this->hh[0] = this->h;

    this->j++;
}

void MainWindow::updatePlot() {
    this->time.resize(j+1);
    this->V.resize(j+1);
    this->I.resize(j+1);
    this->nh.resize(j+1);
    this->mh.resize(j+1);
    this->hh.resize(j+1);

    if (this->cMode == currentImpulse) {
        if (this->j > this->impulseStart + this->impulseDuration/this->dt) {
            changeCurrentMode(currentManual);
        }
    }

    this->time[j] = this->dt*j;
    this->I[j] = this->cI;

    this->gNa = this->gMaxNa * pow(this->m, 3) * this->h;
    this->gK  = this->gMaxK  * pow(this->n, 4);
    this->gL  = this->gMaxL;

    this->n += (this->alphaN(this->V[j-1]) * (1-this->n) - this->betaN(this->V[j-1]) * this->n) * this->dt;
    this->m += (this->alphaM(this->V[j-1]) * (1-this->m) - this->betaM(this->V[j-1]) * this->m) * this->dt;
    this->h += (this->alphaH(this->V[j-1]) * (1-this->h) - this->betaH(this->V[j-1]) * this->h) * this->dt;

    this->nh[j] = n;
    this->mh[j] = m;
    this->hh[j] = h;

    this->V[j] = this->V[j-1] + (this->I[j-1]
               - this->gNa * (this->V[j-1] - this->ENa)
               - this->gK  * (this->V[j-1] - this->EK)
               - this->gL  * (this->V[j-1] - this->EL)
                ) / this->Cm * this->dt;

    ui->plot->graph(graphMembrane)->setData(this->time, this->V);
    ui->plot->graph(graphCurrent)->setData(this->time, this->I);
    if (this->isNShown) { ui->plot->graph(graphN)->setData(this->time, this->nh); }
    if (this->isMShown) { ui->plot->graph(graphM)->setData(this->time, this->mh); }
    if (this->isHShown) { ui->plot->graph(graphH)->setData(this->time, this->hh); }

    ui->plot->replot();

    this->j++;

    this->timer->start(this->interval);
}

void MainWindow::updateCurrent() {
    this->cI = ui->currentSlider->value();
    ui->currentValue->setText(QString::number(this->cI));
}

void MainWindow::updateGNa() {
    this->gMaxNa = ui->gNaSlider->value();
    ui->gNaValue->setText(QString::number(this->gMaxNa));
}

void MainWindow::updateGK() {
    this->gMaxK = ui->gKSlider->value();
    ui->gKValue->setText(QString::number(this->gMaxK));
}

void MainWindow::updateGL() {
    this->gMaxL = ui->gLSlider->value() * 0.1;
    ui->gLValue->setText(QString::number(this->gMaxL));
}

void MainWindow::showN() {
    if (ui->checkN->isChecked() == true) {
        this->isNShown = true;
        ui->plot->graph(graphN)->setData(this->time, this->nh);
        ui->plot->replot();
    } else {
        this->isNShown = false;
        ui->plot->graph(graphN)->setData(this->time, this->blank);
    }
}

void MainWindow::showM() {
    if (ui->checkM->isChecked() == true) {
        this->isMShown = true;
        ui->plot->graph(graphM)->setData(this->time, this->mh);
        ui->plot->replot();
    } else {
        this->isMShown = false;
        ui->plot->graph(graphM)->setData(this->time, this->blank);
    }
}

void MainWindow::showH() {
    if (ui->checkH->isChecked() == true) {
        this->isHShown = true;
        ui->plot->graph(graphH)->setData(this->time, this->hh);
        ui->plot->replot();
    } else {
        this->isHShown = false;
        ui->plot->graph(graphH)->setData(this->time, this->blank);
    }
}

void MainWindow::changeCurrentMode(int m) {
    switch (m) {
        case currentManual:
            this->cMode = m;
            ui->currentSlider->setEnabled(true);
            ui->impulseButton->setEnabled(true);
            ui->impulseMagnitude->setValue(0);
            ui->impulseDuration->setValue(0);
            ui->currentSlider->setValue(0);
            this->cI = 0;
        break;

        case currentImpulse:
            this->cMode = m;
            this->cI = ui->impulseMagnitude->value();
            ui->currentSlider->setEnabled(false);
            this->impulseDuration = ui->impulseDuration->value();
            this->impulseStart = this->j;
            ui->impulseButton->setEnabled(false);
        break;
    }
}

void MainWindow::pause() {
    if (this->isPaused == true) {
        this->isPaused = false;
        ui->pause->setText("||");
        this->timer->start(10);
    } else {
        this->isPaused = true;
        ui->pause->setText(">");
        this->timer->stop();
    }
}

void MainWindow::reset() {
    this->changeCurrentMode(currentManual);
    ui->gNaSlider->setValue(120);
    ui->gKSlider->setValue(36);
    ui->gLSlider->setValue(3);

    this->updateGNa();
    this->updateGK();
    this->updateGL();
}

void MainWindow::clear() {
    this->init();

    ui->plot->graph(0)->setData(this->time, this->V);
    ui->plot->graph(1)->setData(this->time, this->I);
    ui->plot->replot();
}

void MainWindow::settings() {
    if (this->isPaused == false) {
        this->pause();
    }
    this->s->setTimeSteps(this->dt);
    this->s->setPlottingIntervals(this->interval);
    this->s->setMinCurrent(ui->currentSlider->minimum());
    this->s->setMaxCurrent(ui->currentSlider->maximum());
    this->s->setMinGNa(ui->gNaSlider->minimum());
    this->s->setMaxGNa(ui->gNaSlider->maximum());
    this->s->setMinGK(ui->gKSlider->minimum());
    this->s->setMaxGK(ui->gKSlider->maximum());
    this->s->setMinGL(ui->gLSlider->minimum());
    this->s->setMaxGL(ui->gLSlider->maximum());
    this->s->show();
}

void MainWindow::updatePreferences() {
    if (this->s->getMinCurrent() > this->s->getMaxCurrent() ||
        this->s->getMinGNa()     > this->s->getMaxGNa()     ||
        this->s->getMinGK()      > this->s->getMaxGK()      ||
        this->s->getMinGL()      > this->s->getMaxGL())     {
        QMessageBox::warning(this, "Warning", "The minimum values must not be bigger than the maxiumum values!");
        this->pause();
        return;
    }
    if (this->dt != this->s->getTimeSteps() || this->interval != this->s->getPlottingIntervals()) {
        QMessageBox::information(this, "Information", "Changing the time steps or the plotting intervals will force a clear of the current graph.");

        this->dt = this->s->getTimeSteps();
        this->interval = this->s->getPlottingIntervals();
        this->clear();
    }
    ui->currentSlider->setMinimum(this->s->getMinCurrent());
    ui->currentSlider->setMaximum(this->s->getMaxCurrent());
    ui->gNaSlider->setMinimum(this->s->getMinGNa());
    ui->gNaSlider->setMaximum(this->s->getMaxGNa());
    ui->gKSlider->setMinimum(this->s->getMinGK());
    ui->gKSlider->setMaximum(this->s->getMaxGK());
    ui->gLSlider->setMinimum(this->s->getMinGL());
    ui->gLSlider->setMaximum(this->s->getMaxGL());
    this->pause();
}

void MainWindow::about() {
    QMessageBox::about(this, "About HogdkinHuxley Simulator", "<b>HodgkinHuxley Simulator:</b><br>Copyright 2013 by Robin Rump<br><a href='http://www.qtcustomplot.com/'>http://www.robinrump.com/</a><br><br><b>QCustomPlot Library:</b><br>Copyright 2011, 2012, 2013 by<br>Emanuel Eichhammer<br><a href='http://www.qtcustomplot.com/'>http://www.qtcustomplot.com/</a><br><br>");
}

void MainWindow::welcome() {
    this->w->show();
}

int MainWindow::minCurrentValue() {
    return ui->currentSlider->minimum();
}

void MainWindow::toJson() {
    bool paused = true;
    if (this->isPaused == false) {
        this->pause();
        paused = false;
    }
    QJsonDocument document;
    QJsonObject json, jsonVoltage, jsonCurrent, jsonN, jsonM, jsonH;
    for (int i = 0; i < this->time.size(); i++) {
        jsonVoltage.insert(QString::number(this->time[i]), this->V[i]);
        jsonCurrent.insert(QString::number(this->time[i]), this->I[i]);
        jsonN.insert(QString::number(this->time[i]), this->nh[i]);
        jsonM.insert(QString::number(this->time[i]), this->mh[i]);
        jsonH.insert(QString::number(this->time[i]), this->hh[i]);
    }
    json.insert("h", jsonH);
    json.insert("m", jsonM);
    json.insert("n", jsonN);
    json.insert("voltage", jsonVoltage);
    json.insert("current", jsonCurrent);

    document.setObject(json);
    QByteArray bytes = document.toJson();
    QFile file(QFileDialog::getSaveFileName(this, "Select Directory", QDir::toNativeSeparators(QDir::currentPath() + "/HH_" + QDateTime().currentDateTime().toString("yyyy_MM_dd_hh_mm")), "*.json"));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(bytes);
    file.close();
    if (paused == false) {
        this->pause();
    }
}

double MainWindow::alphaN(double v) { return 0.01*(-v + 10)/(exp((-v + 10)/10) - 1); };
double MainWindow::betaN(double v) { return 0.125*exp(-v/80); };

double MainWindow::alphaM(double v) { return 0.1*(-v + 25)/(exp((-v + 25)/10) - 1); };
double MainWindow::betaM(double v) { return 4*exp(-v/18); };

double MainWindow::alphaH(double v) { return 0.07*exp(-v/20); };
double MainWindow::betaH(double v) { return 1/(exp((-v + 30)/10) + 1); };
