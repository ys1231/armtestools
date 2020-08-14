#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_armtools.h"

class armtools : public QMainWindow
{
    Q_OBJECT

public:
    armtools(QWidget *parent = Q_NULLPTR);

private:
    Ui::armtoolsClass ui;
};
