#include "tailcheck.h"
#include "ui_tailcheck.h"
#include "quihelper.h"
#include "crcLib.h"
#include "QDebug"

TailCheck::TailCheck(QWidget *parent) : QWidget(parent),  ui(new Ui::TailCheck)
{
    ui->setupUi(this);
    InitTailCheckForm();
}

TailCheck::~TailCheck()
{
    delete ui;
}

void TailCheck::InitTailCheckForm(void)
{
    QSignalMapper *myTcMapper;
    myTcMapper = new QSignalMapper(this);

    QRadioButton *btn[TC_BTN_NUM] = {
        ui->cs8Btn,     //0
        ui->cs16Btn,    //1
        ui->crc8Btn,    //2
        ui->ItuBtn,
        ui->rohcBtn,
        ui->maximBtn,
        ui->ibmBtn,
        ui->usbBtn,
        ui->modbusBtn,
        ui->ccittBtn,
        ui->ccittFBtn,
        ui->x25Btn,
        ui->xmodemBtn,
        ui->dnpBtn,
        ui->crc32Btn,
        ui->lrcBtn,
        ui->bccBtn
    };
    QButtonGroup *gA = new QButtonGroup;
    for (int i = 0; i < TC_BTN_NUM; i++) {
        connect(btn[i], SIGNAL(clicked(bool)), myTcMapper, SLOT(map()));
        gA->addButton(btn[i],i);
        myTcMapper->setMapping(btn[i], i);
    }
    gA->setExclusive(true);
    connect(myTcMapper, SIGNAL(mappedInt(int)), this, SLOT(radioBtn_Clicked(int)));

    QButtonGroup *gB = new QButtonGroup;
    gB->addButton(ui->rbLBtn);
    gB->addButton(ui->rbGBtn);
    gB->setExclusive(true);

    ui->rbLBtn->setChecked(true); //默认小端
    isLittleMode = true;
    ui->cs8Btn->setChecked(true); //默认CheckSum-8
    switchCheck = 0;

}

void TailCheck::hideEvent(QHideEvent *event)
{
    emit hide_signal();
    QWidget::hideEvent(event);
}

void TailCheck::closeEvent(QCloseEvent *event)
{
    switchCheck = 0xff;
}

void TailCheck::radioBtn_Clicked(int idx)
{
    switchCheck = idx;
    if (ui->rbLBtn->isChecked()) {
        isLittleMode = true;
    } else {
        isLittleMode = false;
    }
    this->hide();
}

QString TailCheck::TailCheckData(QByteArray data)
{
    uint16_t res;
    QByteArray bA = data;
    switch(switchCheck) {
    case 0: {
        res = CheckSum8((uint8_t*)bA.data(), bA.length());
    } break;
    case 1: {
        res = CheckSum16((uint8_t*)bA.data(), bA.length());
    } break;
    case 2: {
        res = crc8((uint8_t*)bA.data(), bA.length());
    } break;
    case 3: {
        res = crc8_itu((uint8_t*)bA.data(), bA.length());
    } break;
    case 4: {
        res = crc8_rohc((uint8_t*)bA.data(), bA.length());
    } break;
    case 5: {
        res = crc8_maxim((uint8_t*)bA.data(), bA.length());
    } break;
    case 6: {
        res = crc16_ibm((uint8_t*)bA.data(), bA.length());
    } break;
    case 7: {
        res = crc16_usb((uint8_t*)bA.data(), bA.length());
    } break;
    case 8: {
        res = crc16_modbus((uint8_t*)bA.data(), bA.length());
    } break;
    case 9: {
        res = crc16_ccitt((uint8_t*)bA.data(), bA.length());
    } break;
    case 10: {
        res = crc16_ccitt_false((uint8_t*)bA.data(), bA.length());
    } break;
    case 11: {
        res = crc16_x25((uint8_t*)bA.data(), bA.length());
    } break;
    case 12: {
        res = crc16_xmodem((uint8_t*)bA.data(), bA.length());
    } break;
    case 13: {
        res = crc16_dnp((uint8_t*)bA.data(), bA.length());
    } break;
    case 14: {
        res = crc32((uint8_t*)bA.data(), bA.length());
    } break;
    case 15: {
        //LRC
        res = LRCSum((uint8_t*)bA.data(), bA.length());
    } break;
    case 16: {
        //BCC
        res = BBChecksum((uint8_t*)bA.data(), bA.length());
    } break;
    default:
        res = 0x0;
      break;
    }

    return QString::number(QChar(res).unicode(), 16).toUpper();
}
