#include "frmcomtool.h"
#include "ui_frmcomtool.h"
#include "quihelper.h"
#include "frame.h"

frmComTool::frmComTool(QWidget *parent) : QWidget(parent), ui(new Ui::frmComTool)
{
    ui->setupUi(this);
    this->initForm();
    this->initConfig();
    QUIHelper::setFormInCenter(this);

    highLight1_str = "";
    highLight2_str = "";
    highLight3_str = "";

    /* 添加高亮菜单 */
    highLight1 = new QAction("高亮：颜色1",ui->txtMain);
    ui->txtMain->addAction(highLight1);
    connect(highLight1,SIGNAL(triggered(bool)),this,SLOT(on_action_HighLight1_triggered()));

    highLight2 = new QAction("高亮：颜色2",ui->txtMain);
    ui->txtMain->addAction(highLight2);
    connect(highLight2,SIGNAL(triggered(bool)),this,SLOT(on_action_HighLight2_triggered()));

    highLight3 = new QAction("高亮：颜色3",ui->txtMain);
    ui->txtMain->addAction(highLight3);
    connect(highLight3,SIGNAL(triggered(bool)),this,SLOT(on_action_HighLight3_triggered()));

    ui->txtMain->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->widget_sendUI->hide();
}

frmComTool::~frmComTool()
{
    delete ui;
}

void frmComTool::initForm()
{
    comOk = false;
    com = 0;
    sleepTime = 10;
    receiveCount = 0;
    sendCount = 0;
    isShow = true;

    ui->cboxSendInterval->addItems(AppConfig::Intervals);
    ui->cboxData->addItems(AppConfig::Datas);

    //读取数据
    timerRead = new QTimer(this);
    timerRead->setInterval(100);
    connect(timerRead, SIGNAL(timeout()), this, SLOT(readData()));

    //发送数据
    timerSend = new QTimer(this);
    connect(timerSend, SIGNAL(timeout()), this, SLOT(sendData()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(sendData()));

    //保存数据
    timerSave = new QTimer(this);
    connect(timerSave, SIGNAL(timeout()), this, SLOT(saveData()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(saveData()));

    ui->tabWidget->setCurrentIndex(0);
    changeEnable(false);

    tcpOk = false;
    socket = new QTcpSocket(this);
    socket->abort();
    connect(socket, SIGNAL(readyRead()), this, SLOT(readDataNet()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(readErrorNet()));

    timerConnect = new QTimer(this);
    connect(timerConnect, SIGNAL(timeout()), this, SLOT(connectNet()));
    timerConnect->setInterval(3000);
    timerConnect->start();

    if(!FrameHelper::GetInfoFromFile()) {
        frmComTool::append(6, QStringLiteral("FrameJson文件解析失败！"));
    }

#ifdef __arm__
    ui->widgetRight->setFixedWidth(280);
#endif
}

void frmComTool::initConfig()
{
    QStringList comList;
    for (int i = 1; i <= 20; i++) {
        comList << QString("COM%1").arg(i);
    }

    comList << "ttyUSB0" << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4";
    comList << "ttymxc1" << "ttymxc2" << "ttymxc3" << "ttymxc4";
    comList << "ttySAC1" << "ttySAC2" << "ttySAC3" << "ttySAC4";
    ui->cboxPortName->addItems(comList);
    ui->cboxPortName->setCurrentIndex(ui->cboxPortName->findText(AppConfig::PortName));
    connect(ui->cboxPortName, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList baudList;
    baudList << "50" << "75" << "100" << "134" << "150" << "200" << "300" << "600" << "1200"
             << "1800" << "2400" << "4800" << "9600" << "14400" << "19200" << "38400"
             << "56000" << "57600" << "76800" << "115200" << "128000" << "256000";

    ui->cboxBaudRate->addItems(baudList);
    ui->cboxBaudRate->setCurrentIndex(ui->cboxBaudRate->findText(QString::number(AppConfig::BaudRate)));
    connect(ui->cboxBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList dataBitsList;
    dataBitsList << "5" << "6" << "7" << "8";

    ui->cboxDataBit->addItems(dataBitsList);
    ui->cboxDataBit->setCurrentIndex(ui->cboxDataBit->findText(QString::number(AppConfig::DataBit)));
    connect(ui->cboxDataBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList parityList;
    parityList << "无" << "奇" << "偶";
#ifdef Q_OS_WIN
    parityList << "标志";
#endif
    parityList << "空格";

    ui->cboxParity->addItems(parityList);
    ui->cboxParity->setCurrentIndex(ui->cboxParity->findText(AppConfig::Parity));
    connect(ui->cboxParity, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    QStringList stopBitsList;
    stopBitsList << "1";
#ifdef Q_OS_WIN
    stopBitsList << "1.5";
#endif
    stopBitsList << "2";

    ui->cboxStopBit->addItems(stopBitsList);
    ui->cboxStopBit->setCurrentIndex(ui->cboxStopBit->findText(QString::number(AppConfig::StopBit)));
    connect(ui->cboxStopBit, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexSend->setChecked(AppConfig::HexSend);
    connect(ui->ckHexSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckHexReceive->setChecked(AppConfig::HexReceive);
    connect(ui->ckHexReceive, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckDebug->setChecked(AppConfig::Debug);
    connect(ui->ckDebug, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoClear->setChecked(AppConfig::AutoClear);
    connect(ui->ckAutoClear, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSend->setChecked(AppConfig::AutoSend);
    connect(ui->ckAutoSend, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoSave->setChecked(AppConfig::AutoSave);
    connect(ui->ckAutoSave, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    QStringList sendInterval;
    QStringList saveInterval;
    sendInterval << "100" << "300" << "500";

    for (int i = 1000; i <= 10000; i = i + 1000) {
        sendInterval << QString::number(i);
        saveInterval << QString::number(i);
    }

    ui->cboxSendInterval->addItems(sendInterval);
    ui->cboxSaveInterval->addItems(saveInterval);

    ui->cboxSendInterval->setCurrentIndex(ui->cboxSendInterval->findText(QString::number(AppConfig::SendInterval)));
    connect(ui->cboxSendInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    ui->cboxSaveInterval->setCurrentIndex(ui->cboxSaveInterval->findText(QString::number(AppConfig::SaveInterval)));
    connect(ui->cboxSaveInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    timerSend->setInterval(AppConfig::SendInterval);
    timerSave->setInterval(AppConfig::SaveInterval);

    if (AppConfig::AutoSend) {
        timerSend->start();
    }

    if (AppConfig::AutoSave) {
        timerSave->start();
    }

    //串口转网络部分
    ui->cboxMode->setCurrentIndex(ui->cboxMode->findText(AppConfig::Mode));
    connect(ui->cboxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->txtServerIP->setText(AppConfig::ServerIP);
    connect(ui->txtServerIP, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtServerPort->setText(QString::number(AppConfig::ServerPort));
    connect(ui->txtServerPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    ui->txtListenPort->setText(QString::number(AppConfig::ListenPort));
    connect(ui->txtListenPort, SIGNAL(textChanged(QString)), this, SLOT(saveConfig()));

    QStringList values;
    values << "0" << "10" << "50";

    for (int i = 100; i < 1000; i = i + 100) {
        values << QString("%1").arg(i);
    }

    ui->cboxSleepTime->addItems(values);

    ui->cboxSleepTime->setCurrentIndex(ui->cboxSleepTime->findText(QString::number(AppConfig::SleepTime)));
    connect(ui->cboxSleepTime, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));

    ui->ckAutoConnect->setChecked(AppConfig::AutoConnect);
    connect(ui->ckAutoConnect, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->checkBox_hex01->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex01, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex02->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex02, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex03->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex03, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex04->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex04, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex05->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex05, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex06->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex06, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex07->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex07, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex08->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex08, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));
    ui->checkBox_hex09->setChecked(AppConfig::sendTxt01_hex);
    connect(ui->checkBox_hex09, SIGNAL(stateChanged(int)), this, SLOT(saveConfig()));

    ui->textEdit01->setText(AppConfig::sendTxt01);
    ui->textEdit02->setText(AppConfig::sendTxt02);
    ui->textEdit03->setText(AppConfig::sendTxt03);
    ui->textEdit04->setText(AppConfig::sendTxt04);
    ui->textEdit05->setText(AppConfig::sendTxt05);
    ui->textEdit06->setText(AppConfig::sendTxt06);
    ui->textEdit07->setText(AppConfig::sendTxt07);
    ui->textEdit08->setText(AppConfig::sendTxt08);
    ui->textEdit09->setText(AppConfig::sendTxt09);
    connect(ui->sendButton_01, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_02, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_03, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_04, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_05, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_06, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_07, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_08, SIGNAL(clicked()), this, SLOT(saveConfig()));
    connect(ui->sendButton_09, SIGNAL(clicked()), this, SLOT(saveConfig()));

}

void frmComTool::saveConfig()
{
    AppConfig::PortName = ui->cboxPortName->currentText();
    AppConfig::BaudRate = ui->cboxBaudRate->currentText().toInt();
    AppConfig::DataBit = ui->cboxDataBit->currentText().toInt();
    AppConfig::Parity = ui->cboxParity->currentText();
    AppConfig::StopBit = ui->cboxStopBit->currentText().toDouble();

    AppConfig::HexSend = ui->ckHexSend->isChecked();
    AppConfig::HexReceive = ui->ckHexReceive->isChecked();
    AppConfig::Debug = ui->ckDebug->isChecked();
    AppConfig::AutoClear = ui->ckAutoClear->isChecked();

    AppConfig::AutoSend = ui->ckAutoSend->isChecked();
    AppConfig::AutoSave = ui->ckAutoSave->isChecked();

    int sendInterval = ui->cboxSendInterval->currentText().toInt();
    if (sendInterval != AppConfig::SendInterval) {
        AppConfig::SendInterval = sendInterval;
        timerSend->setInterval(AppConfig::SendInterval);
    }

    int saveInterval = ui->cboxSaveInterval->currentText().toInt();
    if (saveInterval != AppConfig::SaveInterval) {
        AppConfig::SaveInterval = saveInterval;
        timerSave->setInterval(AppConfig::SaveInterval);
    }

    AppConfig::Mode = ui->cboxMode->currentText();
    AppConfig::ServerIP = ui->txtServerIP->text().trimmed();
    AppConfig::ServerPort = ui->txtServerPort->text().toInt();
    AppConfig::ListenPort = ui->txtListenPort->text().toInt();
    AppConfig::SleepTime = ui->cboxSleepTime->currentText().toInt();
    AppConfig::AutoConnect = ui->ckAutoConnect->isChecked();

    AppConfig::sendTxt01 = ui->textEdit01->toPlainText();
    AppConfig::sendTxt02 = ui->textEdit02->toPlainText();
    AppConfig::sendTxt03 = ui->textEdit03->toPlainText();
    AppConfig::sendTxt04 = ui->textEdit04->toPlainText();
    AppConfig::sendTxt05 = ui->textEdit05->toPlainText();
    AppConfig::sendTxt06 = ui->textEdit06->toPlainText();
    AppConfig::sendTxt07 = ui->textEdit07->toPlainText();
    AppConfig::sendTxt08 = ui->textEdit08->toPlainText();
    AppConfig::sendTxt09 = ui->textEdit09->toPlainText();

    AppConfig::sendTxt01_hex = ui->checkBox_hex01->isChecked();
    AppConfig::sendTxt02_hex = ui->checkBox_hex02->isChecked();
    AppConfig::sendTxt03_hex = ui->checkBox_hex03->isChecked();
    AppConfig::sendTxt04_hex = ui->checkBox_hex04->isChecked();
    AppConfig::sendTxt05_hex = ui->checkBox_hex05->isChecked();
    AppConfig::sendTxt06_hex = ui->checkBox_hex06->isChecked();
    AppConfig::sendTxt07_hex = ui->checkBox_hex07->isChecked();
    AppConfig::sendTxt08_hex = ui->checkBox_hex08->isChecked();
    AppConfig::sendTxt09_hex = ui->checkBox_hex09->isChecked();

    AppConfig::writeConfig();
}

void frmComTool::changeEnable(bool b)
{
    ui->cboxBaudRate->setEnabled(!b);
    ui->cboxDataBit->setEnabled(!b);
    ui->cboxParity->setEnabled(!b);
    ui->cboxPortName->setEnabled(!b);
    ui->cboxStopBit->setEnabled(!b);
    ui->btnSend->setEnabled(b);
    ui->ckAutoSend->setEnabled(b);
    ui->ckAutoSave->setEnabled(b);
}

void frmComTool::append(int type, const QString &data, bool clear, bool isCheckHex)
{
    static int currentCount = 0;
    static int maxCount = 100;

    if (clear) {
        ui->txtMain->clear();
        currentCount = 0;
        return;
    }

    if (currentCount >= maxCount) {
        ui->txtMain->clear();
        currentCount = 0;
    }

    if (!isShow) {
        return;
    }

    //过滤回车换行符
    QString strData = data;
    strData = strData.replace("\r", "");
    strData = strData.replace("\n", "");

    //不同类型不同颜色显示
    QString strType;
    if (type == 0) {
        if (isCheckHex) {
            strType = "串口发送[HEX] >>";
        } else {
            strType = "串口发送[ASCII] >>";
        }
        ui->txtMain->setTextColor(QColor("dodgerblue"));
    } else if (type == 1) {
        strType = "串口接收 <<";
        ui->txtMain->setTextColor(QColor("red"));
    } else if (type == 2) {
        strType = "处理延时 >>";
        ui->txtMain->setTextColor(QColor("gray"));
    } else if (type == 3) {
        strType = "正在校验 >>";
        ui->txtMain->setTextColor(QColor("green"));
    } else if (type == 4) {
        strType = "网络发送 >>";
        ui->txtMain->setTextColor(QColor(24, 189, 155));
    } else if (type == 5) {
        strType = "网络接收 <<";
        ui->txtMain->setTextColor(QColor(255, 107, 107));
    } else if (type == 6) {
        strType = "提示信息 >>";
        ui->txtMain->setTextColor(QColor(100, 184, 255));
    }

    ui->txtMain->append(QString("时间[%1] %2 %3").arg(TIMEMS).arg(strType).arg(strData));

    if (isCheckHex && ui->checkBox_trans->isChecked() && FrameHelper::jsonIsCorrect) {
        strData = FrameHelper::TranslateStr(strData);
        if (strData != "") {
            ui->txtMain->append(strData);
        }
    }

    currentCount++;
}

void frmComTool::readData()
{
    if (com->bytesAvailable() <= 0) {
        return;
    }

    QUIHelper::sleep(sleepTime);
    QByteArray data = com->readAll();
    int dataLen = data.length();
    if (dataLen <= 0) {
        return;
    }

    if (isShow) {
        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QUIHelper::byteArrayToHexStr(data);
        } else {
            //buffer = QUIHelper::byteArrayToAsciiStr(data);
            buffer = QString::fromLocal8Bit(data);
        }

        //启用调试则模拟调试数据
        if (ui->ckDebug->isChecked()) {
            int count = AppConfig::Keys.count();
            for (int i = 0; i < count; i++) {
                if (buffer.startsWith(AppConfig::Keys.at(i))) {
                    sendData(AppConfig::Values.at(i), 0);
                    break;
                }
            }
        }

        append(1, buffer, false, ui->ckHexReceive->isChecked());
        receiveCount = receiveCount + data.size();
        ui->btnReceiveCount->setText(QString("接收 : %1 字节").arg(receiveCount));

        //启用网络转发则调用网络发送数据
        if (tcpOk) {
            socket->write(data);
            append(4, QString(buffer), false, false);
        }
    }
}

void frmComTool::sendData()
{
    QString str = ui->cboxData->currentText();
    if (str.isEmpty()) {
        ui->cboxData->setFocus();
        return;
    }

    sendData(str, 0);

    if (ui->ckAutoClear->isChecked()) {
        ui->cboxData->setCurrentIndex(-1);
        ui->cboxData->setFocus();
    }
}

void frmComTool::sendData(QString data, int hexCheckNo)
{
    if (com == 0 || !com->isOpen()) {
        return;
    }

    //短信猫调试
    if (data.startsWith("AT")) {
        data += "\r";
    }

    QByteArray buffer;
    bool checkResult = isCheckHex(hexCheckNo);

    if (checkResult) {
        buffer = QUIHelper::hexStrToByteArray(data);
    } else {
        buffer = QUIHelper::asciiStrToByteArray(data);
    }

    com->write(buffer);
    append(0, data, false, checkResult);
    sendCount = sendCount + buffer.size();
    ui->btnSendCount->setText(QString("发送 : %1 字节").arg(sendCount));
}

void frmComTool::saveData()
{
    QString tempData = ui->txtMain->toPlainText();

    if (tempData == "") {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    QString name = now.toString("yyyy-MM-dd-HH-mm-ss");
    QString fileName = QString("%1/%2.txt").arg(QUIHelper::appPath()).arg(name);

    QFile file(fileName);
    file.open(QFile::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << tempData;
    file.close();

    on_btnClear_clicked();
}

void frmComTool::on_btnOpen_clicked()
{
    if (ui->btnOpen->text() == "打开串口") {
        com = new QextSerialPort(ui->cboxPortName->currentText(), QextSerialPort::Polling);
        comOk = com->open(QIODevice::ReadWrite);

        if (comOk) {
            //清空缓冲区
            com->flush();
            //设置波特率
            com->setBaudRate((BaudRateType)ui->cboxBaudRate->currentText().toInt());
            //设置数据位
            com->setDataBits((DataBitsType)ui->cboxDataBit->currentText().toInt());
            //设置校验位
            com->setParity((ParityType)ui->cboxParity->currentIndex());
            //设置停止位
            com->setStopBits((StopBitsType)ui->cboxStopBit->currentIndex());
            com->setFlowControl(FLOW_OFF);
            com->setTimeout(10);

            changeEnable(true);
            ui->btnOpen->setText("关闭串口");
            timerRead->start();
        }
    } else {
        timerRead->stop();
        com->close();
        com->deleteLater();
        
        changeEnable(false);
        ui->btnOpen->setText("打开串口");
        //on_btnClear_clicked(); /* 关闭串口不清数据 */
        comOk = false;
    }
}

void frmComTool::on_btnSendCount_clicked()
{
    sendCount = 0;
    ui->btnSendCount->setText("发送 : 0 字节");
}

void frmComTool::on_btnReceiveCount_clicked()
{
    receiveCount = 0;
    ui->btnReceiveCount->setText("接收 : 0 字节");
}

void frmComTool::on_btnStopShow_clicked()
{
    if (ui->btnStopShow->text() == "停止显示") {
        isShow = false;
        ui->btnStopShow->setText("开始显示");
    } else {
        isShow = true;
        ui->btnStopShow->setText("停止显示");
    }
}

void frmComTool::on_btnData_clicked()
{
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg("send.txt");
    QFile file(fileName);
    if (!file.exists()) {
        return;
    }

    if (ui->btnData->text() == "管理数据") {
        ui->txtMain->setReadOnly(false);
        ui->txtMain->clear();
        file.open(QFile::ReadOnly | QIODevice::Text);
        QTextStream in(&file);
        ui->txtMain->setText(in.readAll());
        file.close();
        ui->btnData->setText("保存数据");
    } else {
        ui->txtMain->setReadOnly(true);
        file.open(QFile::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out << ui->txtMain->toPlainText();
        file.close();
        ui->txtMain->clear();
        ui->btnData->setText("管理数据");
        AppConfig::readSendData();
    }
}

void frmComTool::on_btnClear_clicked()
{
    append(0, "", true, false);
}

void frmComTool::on_btnStart_clicked()
{
    if (ui->btnStart->text() == "启动") {
        if (AppConfig::ServerIP == "" || AppConfig::ServerPort == 0) {
            append(6, "IP地址和远程端口不能为空", false, false);
            return;
        }

        socket->connectToHost(AppConfig::ServerIP, AppConfig::ServerPort);
        if (socket->waitForConnected(100)) {
            ui->btnStart->setText("停止");
            append(6, "连接服务器成功", false, false);
            tcpOk = true;
        }
    } else {
        socket->disconnectFromHost();
        if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(100)) {
            ui->btnStart->setText("启动");
            append(6, "断开服务器成功", false, false);
            tcpOk = false;
        }
    }
}

void frmComTool::on_ckAutoSend_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->cboxSendInterval->setEnabled(false);
        timerSend->stop();
    } else {
        ui->cboxSendInterval->setEnabled(true);
        timerSend->start();
    }
}

void frmComTool::on_ckAutoSave_stateChanged(int arg1)
{
    if (arg1 == 0) {
        ui->cboxSaveInterval->setEnabled(false);
        timerSave->stop();
    } else {
        ui->cboxSaveInterval->setEnabled(true);
        timerSave->start();
    }
}

void frmComTool::on_action_HighLight1_triggered()
{
    if (highLight1->text() == HIGH_LIGHT_TITLE_1) {
        if (ui->txtMain->textCursor().selectedText() == "") {
            return;
        }
        if (ui->txtMain->textCursor().selectedText() == highLight2_str
                || ui->txtMain->textCursor().selectedText() == highLight3_str) {
            return;
        }
        highLight1_str = ui->txtMain->textCursor().selectedText();
        HightLight(HIGH_LIGHT_COLOR_1, highLight1_str);
        highLight1->setText(HIGH_LIGHT_CANCEL_TITLE_1);
    } else {
        HightLight(0, highLight1_str);
        highLight1_str = "";
        highLight1->setText(HIGH_LIGHT_TITLE_1);
    }
    return;
}

void frmComTool::on_action_HighLight2_triggered()
{
    if (highLight2->text() == HIGH_LIGHT_TITLE_2) {
        if (ui->txtMain->textCursor().selectedText() == "") {
            return;
        }
        if (ui->txtMain->textCursor().selectedText() == highLight1_str
                || ui->txtMain->textCursor().selectedText() == highLight3_str) {
            return;
        }
        highLight2_str = ui->txtMain->textCursor().selectedText();
        HightLight(HIGH_LIGHT_COLOR_2, highLight2_str);
        highLight2->setText(HIGH_LIGHT_CANCEL_TITLE_2);
    } else {
        HightLight(0, highLight2_str);
        highLight2_str = "";
        highLight2->setText(HIGH_LIGHT_TITLE_2);
    }
    return;
}

void frmComTool::on_action_HighLight3_triggered()
{
    if (highLight3->text() == HIGH_LIGHT_TITLE_3) {
        if (ui->txtMain->textCursor().selectedText() == "") {
            return;
        }
        if (ui->txtMain->textCursor().selectedText() == highLight1_str
                || ui->txtMain->textCursor().selectedText() == highLight2_str) {
            return;
        }
        highLight3_str = ui->txtMain->textCursor().selectedText();
        HightLight(HIGH_LIGHT_COLOR_3, highLight3_str);
        highLight3->setText(HIGH_LIGHT_CANCEL_TITLE_3);
    } else {
        HightLight(0, highLight3_str);
        highLight3_str = "";
        highLight3->setText(HIGH_LIGHT_TITLE_3);
    }
    return;
}

void frmComTool::connectNet()
{
    if (!tcpOk && AppConfig::AutoConnect && ui->btnStart->text() == "启动") {
        if (AppConfig::ServerIP != "" && AppConfig::ServerPort != 0) {
            socket->connectToHost(AppConfig::ServerIP, AppConfig::ServerPort);
            if (socket->waitForConnected(100)) {
                ui->btnStart->setText("停止");
                append(6, "连接服务器成功", false, false);
                tcpOk = true;
            }
        }
    }
}

void frmComTool::readDataNet()
{
    if (socket->bytesAvailable() > 0) {
        QUIHelper::sleep(AppConfig::SleepTime);
        QByteArray data = socket->readAll();

        QString buffer;
        if (ui->ckHexReceive->isChecked()) {
            buffer = QUIHelper::byteArrayToHexStr(data);
        } else {
            buffer = QUIHelper::byteArrayToAsciiStr(data);
        }

        append(5, buffer, false, false);

        //将收到的网络数据转发给串口
        if (comOk) {
            com->write(data);
            append(0, buffer, false, false);
        }
    }
}

void frmComTool::readErrorNet()
{
    ui->btnStart->setText("启动");
    append(6, QString("连接服务器失败,%1").arg(socket->errorString()), false, false);
    socket->disconnectFromHost();
    tcpOk = false;
}

void frmComTool::HightLight(int type, QString highLightStr)
{
    int color;
    if (highLightStr == "") {
        return;
    }
    switch(type) {
        case HIGH_LIGHT_COLOR_1: {
            color = Qt::yellow;
            break;
        }
        case HIGH_LIGHT_COLOR_2: {
            color = Qt::lightGray;
            break;
        }
        case HIGH_LIGHT_COLOR_3: {
            color = Qt::green;
            break;
        }
        default: /* 取消高亮，默认背景颜色白色 */
            color = Qt::white;
            break;
        break;
    }

    QTextDocument  *document = ui->txtMain->document();
    QTextCursor highlight_cursor(document);
    QTextCursor cursor(document);

    cursor.beginEditBlock();
    QTextCharFormat color_format;

    while (!highlight_cursor.isNull() && !highlight_cursor.atEnd()) {
        highlight_cursor = document->find(highLightStr,
                                          highlight_cursor, QTextDocument::FindWholeWords);
        if (!highlight_cursor.isNull()) {
            color_format = highlight_cursor.charFormat();
            color_format.setBackground((Qt::GlobalColor)color); /* 高亮词的颜色 */
            highlight_cursor.mergeCharFormat(color_format);
        }
    }
    cursor.endEditBlock();
    color_format.setBackground(Qt::white); /* 本QTextEdit背景默认颜色 */

    return;
}

bool frmComTool::isCheckHex(int no)
{
    bool isHex = false;
    switch(no)
    {
    case 0:{
        if(ui->ckHexSend->isChecked())
            isHex = true;
            break;
       }
    case 1:{
       if(ui->checkBox_hex01->isChecked())
           isHex = true;
           break;
       }
    case 2:{
       if(ui->checkBox_hex02->isChecked())
           isHex = true;
           break;
       }
    case 3:{
       if(ui->checkBox_hex03->isChecked())
           isHex = true;
           break;
       }
    case 4:{
       if(ui->checkBox_hex04->isChecked())
           isHex = true;
           break;
       }
    case 5:{
       if(ui->checkBox_hex05->isChecked())
           isHex = true;
           break;
       }
    case 6:{
       if(ui->checkBox_hex06->isChecked())
           isHex = true;
           break;
       }
    case 7:{
       if(ui->checkBox_hex07->isChecked())
           isHex = true;
           break;
       }
    case 8:{
       if(ui->checkBox_hex08->isChecked())
           isHex = true;
           break;
       }
    case 9:{
       if(ui->checkBox_hex09->isChecked())
           isHex = true;
           break;
       }
    }
    return isHex;
}

void frmComTool::on_expandButton_clicked()
{
    if(ui->widget_sendUI->isHidden()) {
        ui->widget_sendUI->show();
        ui->expandButton->setText("收起");
    } else {
        ui->widget_sendUI->hide();
        ui->expandButton->setText("展开");
    }
}


void frmComTool::on_sendButton_01_clicked()
{
    QString str = ui->textEdit01->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 1);
}


void frmComTool::on_sendButton_02_clicked()
{
    QString str = ui->textEdit02->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 2);
}


void frmComTool::on_sendButton_03_clicked()
{
    QString str = ui->textEdit03->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 3);
}


void frmComTool::on_sendButton_04_clicked()
{
    QString str = ui->textEdit04->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 4);
}


void frmComTool::on_sendButton_05_clicked()
{
    QString str = ui->textEdit05->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 5);
}


void frmComTool::on_sendButton_06_clicked()
{
    QString str = ui->textEdit06->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 6);
}


void frmComTool::on_sendButton_07_clicked()
{
    QString str = ui->textEdit07->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 7);
}


void frmComTool::on_sendButton_08_clicked()
{
    QString str = ui->textEdit08->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 8);
}


void frmComTool::on_sendButton_09_clicked()
{
    QString str = ui->textEdit09->toPlainText();
    if (str.isEmpty()) {
        return;
    }

    sendData(str, 9);
}

