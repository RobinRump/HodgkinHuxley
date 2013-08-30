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

#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT
    
public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

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

    void setMinCurrent(int v);
    void setMaxCurrent(int v);
    void setMinGNa(int v);
    void setMaxGNa(int v);
    void setMinGK(int v);
    void setMaxGK(int v);
    void setMinGL(int v);
    void setMaxGL(int v);

private:
    Ui::Settings *ui;

private slots:

};

#endif // SETTINGS_H
