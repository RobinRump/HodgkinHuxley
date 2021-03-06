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

#include "preferences.h"
#include "ui_preferences.h"

Preferences::Preferences(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences)
{
    ui->setupUi(this);
    this->setWindowTitle("Preferences");

    // define pointers
    this->colorButtons.resize(5);
    this->colorButtons[0] = ui->colorMembraneButton;
    this->colorButtons[1] = ui->colorCurrentButton;
    this->colorButtons[2] = ui->colorNButton;
    this->colorButtons[3] = ui->colorMButton;
    this->colorButtons[4] = ui->colorHButton;
    this->colorValues.resize(5);
    this->colorValues[0] = ui->colorMembraneValue;
    this->colorValues[1] = ui->colorCurrentValue;
    this->colorValues[2] = ui->colorNValue;
    this->colorValues[3] = ui->colorMValue;
    this->colorValues[4] = ui->colorHValue;

    // connect ui elements with slots
    connect(ui->buttonBox, SIGNAL(accepted()), this->parent(), SLOT(updatePreferences()));
    connect(ui->buttonBox, SIGNAL(rejected()), this->parent(), SLOT(loadPauseState()));

    QVector<QSignalMapper*> signalMapper;
    signalMapper.resize(5);
    for (int i = 0; i<5; i++) {
        signalMapper[i] = new QSignalMapper(this);
        connect(this->colorButtons[i], SIGNAL(clicked(bool)), signalMapper[i], SLOT(map()));
        signalMapper[i]->setMapping(this->colorButtons[i], i);
        connect(signalMapper[i], SIGNAL(mapped(int)), this, SLOT(changeColor(int)));
    }
}

Preferences::~Preferences()
{
    delete ui;
}

QString Preferences::toColorCode(QColor color)
{
    QString r = QString::number(color.red(), 16);
    QString g = QString::number(color.green(), 16);
    QString b = QString::number(color.blue(), 16);
    if (r.length() == 1) { r.prepend("0"); }
    if (g.length() == 1) { g.prepend("0"); }
    if (b.length() == 1) { b.prepend("0"); }

    return "#" + r + g + b;
}

QColor Preferences::fromColorCode(QString code)
{
    QString r = code.mid(1, 2);
    QString g = code.mid(3, 2);
    QString b = code.mid(5, 2);
    bool ok;
    return QColor(r.toInt(&ok, 16), g.toInt(&ok, 16), b.toInt(&ok, 16));

}

QVector<QColor> Preferences::getColors() {
    QVector<QColor> colors(5);
    for (int i = 0; i<5; i++) {
        colors[i] = this->fromColorCode(this->colorValues[i]->text());
    }

    return colors;
}

void Preferences::setColors(QVector<QColor> colors)
{
    for (int i = 0; i<5; i++) {
        this->colorButtons[i]->setStyleSheet("background-color: " + this->toColorCode(colors[i]));
        this->colorValues[i]->setText(this->toColorCode(colors[i]));
    }
}

void Preferences::changeColor(int i) {
    QColor color = QColorDialog::getColor();
    colorButtons[i]->setStyleSheet("background-color: " + this->toColorCode(color));
    colorValues[i]->setText(this->toColorCode(color));
}

int Preferences::getMinCurrent() { return ui->minCurrentValue->value(); }
int Preferences::getMaxCurrent() { return ui->maxCurrentValue->value(); }
int Preferences::getMinGNa() { return ui->minGNaValue->value(); }
int Preferences::getMaxGNa() { return ui->maxGNaValue->value(); }
int Preferences::getMinGK() { return ui->minGKValue->value(); }
int Preferences::getMaxGK() { return ui->maxGKValue->value(); }
int Preferences::getMinGL() { return ui->minGLValue->value(); }
int Preferences::getMaxGL() { return ui->maxGLValue->value(); }

void Preferences::setMinCurrent(int v) { ui->minCurrentValue->setValue(v); }
void Preferences::setMaxCurrent(int v) { ui->maxCurrentValue->setValue(v); }
void Preferences::setMinGNa(int v) { ui->minGNaValue->setValue(v); }
void Preferences::setMaxGNa(int v) { ui->maxGNaValue->setValue(v); }
void Preferences::setMinGK(int v) { ui->minGKValue->setValue(v); }
void Preferences::setMaxGK(int v) { ui->maxGKValue->setValue(v); }
void Preferences::setMinGL(int v) { ui->minGLValue->setValue(v); }
void Preferences::setMaxGL(int v) { ui->maxGLValue->setValue(v); }
