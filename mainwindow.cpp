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
    this->setMinimumWidth(800);
    this->setMinimumHeight(630);

    // setup variables and sliders
    this->timer = new QTimer(this);
    this->config = new QFile(QDir::toNativeSeparators(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/com.robinrump.hodgkinhuxley.json"));

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

    // prepare simulator
    this->init();

    // init windows
    this->s = new Settings(this);
    this->w = new Welcome(this);

    // define the pointers
    this->values.resize(5);
    this->values[0] = &this->V;
    this->values[1] = &this->I;
    this->values[2] = &this->nh;
    this->values[3] = &this->mh;
    this->values[4] = &this->hh;

    // plot config
    ui->plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    ui->plot->plotLayout()->insertRow(0);
    ui->plot->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->plot, "HodgkinHuxley"));

    // legend
    ui->plot->legend->setVisible(true);
    QFont legendFont = this->font();
    legendFont.setPointSize(10);
    ui->plot->legend->setFont(legendFont);
    ui->plot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    ui->plot->legend->setIconSize(QSize(10, 3));
    ui->plot->legend->setSelectableParts(QCPLegend::spItems);
    ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    // graphes
    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);  // V
    ui->plot->graph(graphMembrane)->setData(this->time, this->V);
    ui->plot->graph(graphMembrane)->setName("Membrane");

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis);  // I
    ui->plot->graph(graphCurrent)->setPen(QPen(QColor(255, 100, 0)));
    ui->plot->graph(graphCurrent)->setData(this->time, this->I);
    ui->plot->graph(graphCurrent)->setName("Current");

    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2); // n
    ui->plot->graph(graphN)->setPen(QPen(QColor(255, 0, 0)));
    ui->plot->graph(graphN)->setName("n");
    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2); // m
    ui->plot->graph(graphM)->setPen(QPen(QColor(0, 255, 0)));
    ui->plot->graph(graphM)->setName("m");
    ui->plot->addGraph(ui->plot->xAxis, ui->plot->yAxis2); // h
    ui->plot->graph(graphH)->setPen(QPen(QColor(60, 10, 80)));
    ui->plot->graph(graphH)->setName("h");

    // axes configuration
    ui->plot->xAxis->setLabel("Time in ms");
    ui->plot->yAxis->setLabel("Membran potential in mV");
    ui->plot->yAxis2->setLabel("Gatting channels");
    ui->plot->yAxis2->setVisible(true);

    // ranges
    ui->plot->xAxis->setRange(0, 100);
    ui->plot->yAxis->setRange(-50, 150);
    ui->plot->yAxis2->setRange(0, 1);
    ui->plot->replot();

    // config
    QJsonObject json;
    if (this->config->exists()) {
        json = this->fromConfig();
    } else {
        json.insert("version", 105);
        json.insert("startup", true);
        this->toConfig(json);
    }

    // connect ui elements with slots
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

    connect(ui->pause, SIGNAL(clicked(bool)), this, SLOT(switchPause()));
    connect(ui->reset, SIGNAL(clicked(bool)), this, SLOT(reset()));
    connect(ui->clear, SIGNAL(clicked(bool)), this, SLOT(clear()));

    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(settings()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui->actionJson, SIGNAL(triggered(bool)), this, SLOT(toJson()));
    connect(ui->actionXml, SIGNAL(triggered(bool)), this, SLOT(toXml()));
    connect(ui->actionPng, SIGNAL(triggered(bool)), this, SLOT(toPng()));
    connect(ui->actionJpg, SIGNAL(triggered(bool)), this, SLOT(toJpg()));
    connect(ui->actionPdf, SIGNAL(triggered(bool)), this, SLOT(toPdf()));
    connect(ui->actionWelcome, SIGNAL(triggered(bool)), this, SLOT(welcome()));
    connect(ui->actionReset, SIGNAL(triggered(bool)), this, SLOT(reset()));
    connect(ui->actionClear, SIGNAL(triggered(bool)), this, SLOT(clear()));

    // set right click
    ui->plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->plot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    // connect and start timer
    connect(this->timer, SIGNAL(timeout()), this, SLOT(updatePlot()));
    this->timer->start(this->interval);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    // setup time variables
    this->j        = 0;
    this->time.resize(this->j+1);
    this->time[0]  = 0;

    // init voltage
    this->V.resize(this->j+1); // mV
    this->V[0] = this->VRest;

    // init gating variables
    this->n = this->alphaN(V[0])/(this->alphaN(V[0]) + this->betaN(V[0]));
    this->m = this->alphaM(V[0])/(this->alphaM(V[0]) + this->betaM(V[0]));
    this->h = this->alphaH(V[0])/(this->alphaH(V[0]) + this->betaH(V[0]));
    this->nh.resize(this->j+1);
    this->nh[0] = this->n;
    this->mh.resize(this->j+1);
    this->mh[0] = this->m;
    this->hh.resize(this->j+1);
    this->hh[0] = this->h;

    // init current
    this->I.resize(this->j+1); // mA
    this->I[0] = 0;
    this->cI = 0;
    ui->currentSlider->setValue(this->cI);
    ui->currentValue->setText(QString::number(this->cI));

    this->j++;
}

void MainWindow::updatePlot()
{
    // resize arrays
    this->time.resize(this->j+1);
    this->V.resize(this->j+1);
    this->I.resize(this->j+1);
    this->nh.resize(this->j+1);
    this->mh.resize(this->j+1);
    this->hh.resize(this->j+1);

    // check whether impulse has expired
    if (this->cMode == currentImpulse) {
        if (this->j > this->impulseStart + this->impulseDuration/this->dt) {
            changeCurrentMode(currentManual);
        }
    }

    // calculate time and values
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

    // replot
    ui->plot->graph(graphMembrane)->setData(this->time, this->V);
    ui->plot->graph(graphCurrent)->setData(this->time, this->I);
    if (this->isNShown) { ui->plot->graph(graphN)->setData(this->time, this->nh); }
    if (this->isMShown) { ui->plot->graph(graphM)->setData(this->time, this->mh); }
    if (this->isHShown) { ui->plot->graph(graphH)->setData(this->time, this->hh); }
    ui->plot->replot();

    this->j++;
    this->timer->start(this->interval);
}

void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (this->isPaused == true) {
        menu->addAction("Unpause", this, SLOT(unpause()));
    } else {
        menu->addAction("Pause", this, SLOT(pause()));
    }
    menu->addAction("Reset settings", this, SLOT(reset()));
    menu->addAction("Clear all graphs", this, SLOT(clear()));

    menu->popup(ui->plot->mapToGlobal(pos));
}

QJsonObject MainWindow::fromConfig()
{
    QJsonDocument document;
    QByteArray bytes;
    if (!this->config->open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QJsonObject();
    }
    bytes = this->config->readAll();
    this->config->close();
    document = QJsonDocument::fromJson(bytes);
    return document.object();
}

bool MainWindow::toConfig(QJsonObject j)
{
    QJsonDocument document(j);
    QByteArray bytes = document.toJson();
    if (!this->config->open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }
    this->config->write(bytes);
    this->config->close();
    return true;
}

void MainWindow::updateCurrent()
{
    this->cI = ui->currentSlider->value();
    ui->currentValue->setText(QString::number(this->cI));
}

void MainWindow::updateGNa()
{
    this->gMaxNa = ui->gNaSlider->value();
    ui->gNaValue->setText(QString::number(this->gMaxNa));
}

void MainWindow::updateGK()
{
    this->gMaxK = ui->gKSlider->value();
    ui->gKValue->setText(QString::number(this->gMaxK));
}

void MainWindow::updateGL()
{
    this->gMaxL = ui->gLSlider->value() * 0.1;
    ui->gLValue->setText(QString::number(this->gMaxL));
}

void MainWindow::showN()
{
    if (ui->checkN->isChecked() == true) {
        this->isNShown = true;
        ui->plot->graph(graphN)->setData(this->time, this->nh);
        ui->plot->replot();
    } else {
        this->isNShown = false;
        ui->plot->graph(graphN)->setData(this->time, this->blank);
    }
}

void MainWindow::showM()
{
    if (ui->checkM->isChecked() == true) {
        this->isMShown = true;
        ui->plot->graph(graphM)->setData(this->time, this->mh);
        ui->plot->replot();
    } else {
        this->isMShown = false;
        ui->plot->graph(graphM)->setData(this->time, this->blank);
    }
}

void MainWindow::showH()
{
    if (ui->checkH->isChecked() == true) {
        this->isHShown = true;
        ui->plot->graph(graphH)->setData(this->time, this->hh);
        ui->plot->replot();
    } else {
        this->isHShown = false;
        ui->plot->graph(graphH)->setData(this->time, this->blank);
    }
}

void MainWindow::changeCurrentMode(int m)
{
    switch (m) {
        case currentManual:
            this->cMode = m;
            ui->currentSlider->setEnabled(true);
            ui->impulseButton->setEnabled(true);
            ui->impulseDuration->setEnabled(true);
            ui->impulseMagnitude->setEnabled(true);
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
            ui->impulseDuration->setEnabled(false);
            ui->impulseMagnitude->setEnabled(false);
        break;
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    ui->plot->setGeometry(ui->plot->x(), ui->plot->y(), this->size().width()-40, this->size().height()-259);
    ui->copyright->setGeometry(this->size().width()-451, this->size().height()-70, ui->copyright->width(), ui->copyright->height());
    ui->tabCurrent->setGeometry(ui->tabCurrent->x(), this->size().height()-220, ui->tabCurrent->width(), ui->tabCurrent->height());
    ui->controlBox->setGeometry(this->size().width()-120, this->size().height()-228, ui->controlBox->width(), ui->controlBox->height());
    ui->tabSettings->setGeometry((ui->controlBox->x()-(ui->tabCurrent->x()+ui->tabCurrent->width()))/2 + ui->tabCurrent->x() + ui->tabCurrent->width() - ui->tabSettings->width()/2, this->size().height()-220, ui->tabSettings->width(), ui->tabSettings->height());
}

void MainWindow::pause()
{
    this->timer->stop();
    this->isPaused = true;
    ui->pause->setText(">");
}

void MainWindow::unpause()
{
    this->isPaused = false;
    ui->pause->setText("||");
    this->timer->start(10);
}

void MainWindow::switchPause()
{
    if (this->isPaused == true) {
        this->unpause();
    } else {
        this->pause();
    }
}

void MainWindow::savePauseState()
{
    this->pauseState = this->isPaused;
}

void MainWindow::loadPauseState()
{
    if (this->pauseState == true) {
        this->pause();
    } else {
        this->unpause();
    }
    this->pauseState = this->isPaused;
}

void MainWindow::reset()
{
    this->changeCurrentMode(currentManual);
    ui->gNaSlider->setValue(120);
    ui->gKSlider->setValue(36);
    ui->gLSlider->setValue(3);

    this->updateGNa();
    this->updateGK();
    this->updateGL();
}

void MainWindow::clear()
{
    this->init();

    ui->plot->graph(0)->setData(this->time, this->V);
    ui->plot->graph(1)->setData(this->time, this->I);
    ui->plot->replot();
}

void MainWindow::settings()
{
    this->savePauseState();
    this->pause();

    this->s->setMinCurrent(ui->currentSlider->minimum());
    this->s->setMaxCurrent(ui->currentSlider->maximum());
    this->s->setMinGNa(ui->gNaSlider->minimum());
    this->s->setMaxGNa(ui->gNaSlider->maximum());
    this->s->setMinGK(ui->gKSlider->minimum());
    this->s->setMaxGK(ui->gKSlider->maximum());
    this->s->setMinGL(ui->gLSlider->minimum());
    this->s->setMaxGL(ui->gLSlider->maximum());
    QVector<QColor> colors(5);
    colors[0] = ui->plot->graph(0)->pen().color();
    colors[1] = ui->plot->graph(1)->pen().color();
    colors[2] = ui->plot->graph(2)->pen().color();
    colors[3] = ui->plot->graph(3)->pen().color();
    colors[4] = ui->plot->graph(4)->pen().color();
    this->s->setColors(colors);
    this->s->show();
}

void MainWindow::updatePreferences()
{
    if (this->s->getMinCurrent() > this->s->getMaxCurrent() ||
        this->s->getMinGNa()     > this->s->getMaxGNa()     ||
        this->s->getMinGK()      > this->s->getMaxGK()      ||
        this->s->getMinGL()      > this->s->getMaxGL())     {
        QMessageBox::warning(this, "Warning", "The minimum values must not be bigger than the maxiumum values!");
    } else {
        ui->currentSlider->setMinimum(this->s->getMinCurrent());
        ui->currentSlider->setMaximum(this->s->getMaxCurrent());
        ui->gNaSlider->setMinimum(this->s->getMinGNa());
        ui->gNaSlider->setMaximum(this->s->getMaxGNa());
        ui->gKSlider->setMinimum(this->s->getMinGK());
        ui->gKSlider->setMaximum(this->s->getMaxGK());
        ui->gLSlider->setMinimum(this->s->getMinGL());
        ui->gLSlider->setMaximum(this->s->getMaxGL());
        QVector<QColor> colors = this->s->getColors();
        for (int i = 0; i<5; i++) {
            ui->plot->graph(i)->setPen(QPen(colors[i]));
        }
    }

    this->loadPauseState();
}

void MainWindow::about()
{
    QMessageBox::about(this, "About HogdkinHuxley Simulator", "<b>HodgkinHuxley Simulator:</b><br>Copyright 2013 by Robin Rump<br><a href='http://www.qtcustomplot.com/'>http://www.robinrump.com/</a><br><br><b>QCustomPlot Library:</b><br>Copyright 2011, 2012, 2013 by<br>Emanuel Eichhammer<br><a href='http://www.qtcustomplot.com/'>http://www.qtcustomplot.com/</a><br><br>");
}

void MainWindow::welcome()
{
    this->w->show();
}

void MainWindow::toJson()
{
    this->savePauseState();
    this->pause();

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

    this->loadPauseState();
}

void MainWindow::toXml()
{
    this->savePauseState();
    this->pause();

    QFile file(QFileDialog::getSaveFileName(this, "Select Directory", QDir::toNativeSeparators(QDir::currentPath() + "/HH_" + QDateTime().currentDateTime().toString("yyyy_MM_dd_hh_mm")), "*.xml"));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QXmlStreamWriter stream(&file);

    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    QList<QString> sections;
    sections.append("voltage");
    sections.append("current");
    sections.append("n");
    sections.append("m");
    sections.append("h");
    int j = 0;
    foreach (QString section, sections) {
        stream.writeStartElement(section);
        for (int i = 0; i < this->time.size(); i++) {
            //stream.writeTextElement(QString::number(this->time[i]), QString::number(this->values[j]->at(i)));
            stream.writeEmptyElement("value");
            stream.writeAttribute("time", QString::number(this->time[i]));
            stream.writeAttribute(section, QString::number(this->values[j]->at(i)));
        }
        stream.writeEndElement();
        j++;
    }
    stream.writeEndDocument();
    file.close();

    this->loadPauseState();
}

void MainWindow::toPng()
{
    this->savePauseState();
    this->pause();

    ui->plot->savePng(QFileDialog::getSaveFileName(this, "Select Directory", QDir::toNativeSeparators(QDir::currentPath() + "/HH_" + QDateTime().currentDateTime().toString("yyyy_MM_dd_hh_mm")), "*.png"),
                      QInputDialog::getInt(this, "Export as Png", "Width?", ui->plot->width(), 1),
                      QInputDialog::getInt(this, "Export as Png", "Height?", ui->plot->height(), 1));

    this->loadPauseState();
}

void MainWindow::toJpg()
{
    this->savePauseState();
    this->pause();

    ui->plot->saveJpg(QFileDialog::getSaveFileName(this, "Select Directory", QDir::toNativeSeparators(QDir::currentPath() + "/HH_" + QDateTime().currentDateTime().toString("yyyy_MM_dd_hh_mm")), "*.jpg"),
                      QInputDialog::getInt(this, "Export as Jpg", "Width?", ui->plot->width(), 1),
                      QInputDialog::getInt(this, "Export as Jpg", "Height?", ui->plot->height(), 1));

    this->loadPauseState();
}

void MainWindow::toPdf()
{
    this->savePauseState();
    this->pause();

    ui->plot->savePdf(QFileDialog::getSaveFileName(this, "Select Directory", QDir::toNativeSeparators(QDir::currentPath() + "/HH_" + QDateTime().currentDateTime().toString("yyyy_MM_dd_hh_mm")), "*.pdf"), false,
                      QInputDialog::getInt(this, "Export as Pdf", "Width?", ui->plot->width(), 1),
                      QInputDialog::getInt(this, "Export as Pdf", "Height?", ui->plot->height(), 1));

    this->loadPauseState();
}

double MainWindow::alphaN(double v) { return 0.01*(-v + 10)/(exp((-v + 10)/10) - 1); };
double MainWindow::betaN(double v) { return 0.125*exp(-v/80); };

double MainWindow::alphaM(double v) { return 0.1*(-v + 25)/(exp((-v + 25)/10) - 1); };
double MainWindow::betaM(double v) { return 4*exp(-v/18); };

double MainWindow::alphaH(double v) { return 0.07*exp(-v/20); };
double MainWindow::betaH(double v) { return 1/(exp((-v + 30)/10) + 1); };
