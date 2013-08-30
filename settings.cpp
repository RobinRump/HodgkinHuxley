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

#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowTitle("Preferences");

    // connect ui elements with slots
    connect(ui->buttonBox, SIGNAL(accepted()), this->parent(), SLOT(updatePreferences()));
    connect(ui->buttonBox, SIGNAL(rejected()), this->parent(), SLOT(pause()));
}

Settings::~Settings()
{
    delete ui;
}

int Settings::getMinCurrent() { return ui->minCurrentValue->value(); }
int Settings::getMaxCurrent() { return ui->maxCurrentValue->value(); }
int Settings::getMinGNa() { return ui->minGNaValue->value(); }
int Settings::getMaxGNa() { return ui->maxGNaValue->value(); }
int Settings::getMinGK() { return ui->minGKValue->value(); }
int Settings::getMaxGK() { return ui->maxGKValue->value(); }
int Settings::getMinGL() { return ui->minGLValue->value(); }
int Settings::getMaxGL() { return ui->maxGLValue->value(); }

void Settings::setMinCurrent(int v) { ui->minCurrentValue->setValue(v); }
void Settings::setMaxCurrent(int v) { ui->maxCurrentValue->setValue(v); }
void Settings::setMinGNa(int v) { ui->minGNaValue->setValue(v); }
void Settings::setMaxGNa(int v) { ui->maxGNaValue->setValue(v); }
void Settings::setMinGK(int v) { ui->minGKValue->setValue(v); }
void Settings::setMaxGK(int v) { ui->maxGKValue->setValue(v); }
void Settings::setMinGL(int v) { ui->minGLValue->setValue(v); }
void Settings::setMaxGL(int v) { ui->maxGLValue->setValue(v); }
