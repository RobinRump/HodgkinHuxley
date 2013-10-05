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

#include "welcome.h"
#include "ui_welcome.h"

Welcome::Welcome(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    this->setWindowTitle("Welcome to HodgkinHuxley!");


    this->scene = new QGraphicsScene(ui->graphicsView->sceneRect());
    this->items = new QGraphicsItemGroup;

    QGraphicsRectItem sliderPage(this->scene->sceneRect());
    QGraphicsPixmapItem slider(QPixmap(":/resources/slider.png"));
    slider.setParentItem(&sliderPage);

    QGraphicsRectItem controlsPage(this->scene->sceneRect());
    QGraphicsPixmapItem controls(QPixmap(":/resources/controls.png"));
    controls.setParentItem(&controlsPage);

    this->items->addToGroup(new QGraphicsPixmapItem(QPixmap(":/resources/slider.png")));
    this->items->addToGroup(new QGraphicsPixmapItem(QPixmap(":/resources/controls.png")));
    this->scene->addItem(this->items);

    ui->graphicsView->setScene(this->scene);

    // connect ui elements with slots
    connect(ui->closeButton, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->leftButton, SIGNAL(clicked(bool)), this, SLOT(slideLeft()));
}

Welcome::~Welcome()
{
    delete ui;
}

void Welcome::slideLeft()
{
    this->items->childItems().at(0)->moveBy(20,30);
}

void Welcome::slideRight()
{

}
