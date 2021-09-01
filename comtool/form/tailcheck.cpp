#include "tailcheck.h"
#include "ui_tailcheck.h"

TailCheck::TailCheck(QWidget *parent) : QWidget(parent),  ui(new Ui::TailCheck)
{
    ui->setupUi(this);
}

TailCheck::~TailCheck()
{
    delete ui;
}
