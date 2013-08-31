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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QColor>
#include <QColorDialog>
#include <QDebug>
#include <QDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QVector>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT
    
public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

    enum Color {
        colorMembrane = 0,
        colorCurrent  = 1,
        colorN        = 2,
        colorM        = 3,
        colorH        = 4
    };

    QString toColorCode(QColor color);
    QColor fromColorCode(QString code);

    double getTimeSteps();
    int getPlottingIntervals();
    int getMinCurrent();
    int getMaxCurrent();
    int getMinGNa();
    int getMaxGNa();
    int getMinGK();
    int getMaxGK();
    int getMinGL();
    int getMaxGL();
    QVector<QColor> getColors();

    void setMinCurrent(int v);
    void setMaxCurrent(int v);
    void setMinGNa(int v);
    void setMaxGNa(int v);
    void setMinGK(int v);
    void setMaxGK(int v);
    void setMinGL(int v);
    void setMaxGL(int v);
    void setColors(QVector<QColor> colors);

private:
    Ui::Settings *ui;

    QVector<QToolButton *> buttons;
    QVector<QLineEdit *> colors;

private slots:
    void changeColor();
};

#endif // SETTINGS_H
