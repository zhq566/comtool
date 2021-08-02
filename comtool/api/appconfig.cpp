#include "appconfig.h"
#include "quihelper.h"

QString AppConfig::ConfigFile = "config.ini";
QString AppConfig::SendFileName = "send.txt";
QString AppConfig::DeviceFileName = "device.txt";

QString AppConfig::PortName = "COM1";
int AppConfig::BaudRate = 9600;
int AppConfig::DataBit = 8;
QString AppConfig::Parity = QString::fromUtf8("无");
double AppConfig::StopBit = 1;

bool AppConfig::HexSend = false;
bool AppConfig::HexReceive = false;
bool AppConfig::Debug = false;
bool AppConfig::AutoClear = false;

bool AppConfig::AutoSend = false;
int AppConfig::SendInterval = 1000;
bool AppConfig::AutoSave = false;
int AppConfig::SaveInterval = 5000;

QString AppConfig::Mode = "Tcp_Client";
QString AppConfig::ServerIP = "127.0.0.1";
int AppConfig::ServerPort = 6000;
int AppConfig::ListenPort = 6000;
int AppConfig::SleepTime = 100;
bool AppConfig::AutoConnect = false;

QString AppConfig::sendTxt01 = "";
QString AppConfig::sendTxt02 = "";
QString AppConfig::sendTxt03 = "";
QString AppConfig::sendTxt04 = "";
QString AppConfig::sendTxt05 = "";
QString AppConfig::sendTxt06 = "";
QString AppConfig::sendTxt07 = "";
QString AppConfig::sendTxt08 = "";
QString AppConfig::sendTxt09 = "";
bool AppConfig::sendTxt01_hex = false;
bool AppConfig::sendTxt02_hex = false;
bool AppConfig::sendTxt03_hex = false;
bool AppConfig::sendTxt04_hex = false;
bool AppConfig::sendTxt05_hex = false;
bool AppConfig::sendTxt06_hex = false;
bool AppConfig::sendTxt07_hex = false;
bool AppConfig::sendTxt08_hex = false;
bool AppConfig::sendTxt09_hex = false;

void AppConfig::readConfig()
{
    if (!QUIHelper::checkIniFile(AppConfig::ConfigFile)) {
        writeConfig();
        return;
    }

    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    AppConfig::PortName = set.value("PortName", AppConfig::PortName).toString();
    AppConfig::BaudRate = set.value("BaudRate", AppConfig::BaudRate).toInt();
    AppConfig::DataBit = set.value("DataBit", AppConfig::DataBit).toInt();
    AppConfig::Parity = set.value("Parity", AppConfig::Parity).toString();
    AppConfig::StopBit = set.value("StopBit", AppConfig::StopBit).toInt();

    AppConfig::HexSend = set.value("HexSend", AppConfig::HexSend).toBool();
    AppConfig::HexReceive = set.value("HexReceive", AppConfig::HexReceive).toBool();
    AppConfig::Debug = set.value("Debug", AppConfig::Debug).toBool();
    AppConfig::AutoClear = set.value("AutoClear", AppConfig::AutoClear).toBool();

    AppConfig::AutoSend = set.value("AutoSend", AppConfig::AutoSend).toBool();
    AppConfig::SendInterval = set.value("SendInterval", AppConfig::SendInterval).toInt();
    AppConfig::AutoSave = set.value("AutoSave", AppConfig::AutoSave).toBool();
    AppConfig::SaveInterval = set.value("SaveInterval", AppConfig::SaveInterval).toInt();
    set.endGroup();

    set.beginGroup("NetConfig");
    AppConfig::Mode = set.value("Mode", AppConfig::Mode).toString();
    AppConfig::ServerIP = set.value("ServerIP", AppConfig::ServerIP).toString();
    AppConfig::ServerPort = set.value("ServerPort", AppConfig::ServerPort).toInt();
    AppConfig::ListenPort = set.value("ListenPort", AppConfig::ListenPort).toInt();
    AppConfig::SleepTime = set.value("SleepTime", AppConfig::SleepTime).toInt();
    AppConfig::AutoConnect = set.value("AutoConnect", AppConfig::AutoConnect).toBool();
    set.endGroup();

    set.beginGroup("SendTxtConfig");
    AppConfig::sendTxt01 = set.value("sendTxt01", AppConfig::sendTxt01).toString();
    AppConfig::sendTxt02 = set.value("sendTxt02", AppConfig::sendTxt02).toString();
    AppConfig::sendTxt03 = set.value("sendTxt03", AppConfig::sendTxt03).toString();
    AppConfig::sendTxt04 = set.value("sendTxt04", AppConfig::sendTxt04).toString();
    AppConfig::sendTxt05 = set.value("sendTxt05", AppConfig::sendTxt05).toString();
    AppConfig::sendTxt06 = set.value("sendTxt06", AppConfig::sendTxt06).toString();
    AppConfig::sendTxt07 = set.value("sendTxt07", AppConfig::sendTxt07).toString();
    AppConfig::sendTxt08 = set.value("sendTxt08", AppConfig::sendTxt08).toString();
    AppConfig::sendTxt09 = set.value("sendTxt09", AppConfig::sendTxt09).toString();

    AppConfig::sendTxt01_hex = set.value("sendTxt01_hex", AppConfig::sendTxt01_hex).toBool();
    AppConfig::sendTxt02_hex = set.value("sendTxt02_hex", AppConfig::sendTxt02_hex).toBool();
    AppConfig::sendTxt03_hex = set.value("sendTxt03_hex", AppConfig::sendTxt03_hex).toBool();
    AppConfig::sendTxt04_hex = set.value("sendTxt04_hex", AppConfig::sendTxt04_hex).toBool();
    AppConfig::sendTxt05_hex = set.value("sendTxt05_hex", AppConfig::sendTxt05_hex).toBool();
    AppConfig::sendTxt06_hex = set.value("sendTxt06_hex", AppConfig::sendTxt06_hex).toBool();
    AppConfig::sendTxt07_hex = set.value("sendTxt07_hex", AppConfig::sendTxt07_hex).toBool();
    AppConfig::sendTxt08_hex = set.value("sendTxt08_hex", AppConfig::sendTxt08_hex).toBool();
    AppConfig::sendTxt09_hex = set.value("sendTxt09_hex", AppConfig::sendTxt09_hex).toBool();
    set.endGroup();
}

void AppConfig::writeConfig()
{    
    QSettings set(AppConfig::ConfigFile, QSettings::IniFormat);

    set.beginGroup("ComConfig");
    set.setValue("PortName", AppConfig::PortName);
    set.setValue("BaudRate", AppConfig::BaudRate);
    set.setValue("DataBit", AppConfig::DataBit);
    set.setValue("Parity", AppConfig::Parity);
    set.setValue("StopBit", AppConfig::StopBit);

    set.setValue("HexSend", AppConfig::HexSend);
    set.setValue("HexReceive", AppConfig::HexReceive);
    set.setValue("Debug", AppConfig::Debug);
    set.setValue("AutoClear", AppConfig::AutoClear);

    set.setValue("AutoSend", AppConfig::AutoSend);
    set.setValue("SendInterval", AppConfig::SendInterval);
    set.setValue("AutoSave", AppConfig::AutoSave);
    set.setValue("SaveInterval", AppConfig::SaveInterval);
    set.endGroup();

    set.beginGroup("NetConfig");
    set.setValue("Mode", AppConfig::Mode);
    set.setValue("ServerIP", AppConfig::ServerIP);
    set.setValue("ServerPort", AppConfig::ServerPort);
    set.setValue("ListenPort", AppConfig::ListenPort);
    set.setValue("SleepTime", AppConfig::SleepTime);
    set.setValue("AutoConnect", AppConfig::AutoConnect);
    set.endGroup();
}

QStringList AppConfig::Intervals = QStringList();
QStringList AppConfig::Datas = QStringList();
QStringList AppConfig::Keys = QStringList();
QStringList AppConfig::Values = QStringList();

void AppConfig::readSendData()
{
    //读取发送数据列表
    AppConfig::Datas.clear();
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg(AppConfig::SendFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                AppConfig::Datas.append(line);
            }
        }

        file.close();
    }

    if (AppConfig::Datas.count() == 0) {
        AppConfig::Datas << "16 FF 01 01 E0 E1" << "16 FF 01 01 E1 E2";
    }
}

void AppConfig::readDeviceData()
{
    //读取转发数据列表
    AppConfig::Keys.clear();
    AppConfig::Values.clear();
    QString fileName = QString("%1/%2").arg(QUIHelper::appPath()).arg(AppConfig::DeviceFileName);
    QFile file(fileName);
    if (file.size() > 0 && file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QString line = file.readLine();
            line = line.trimmed();
            line = line.replace("\r", "");
            line = line.replace("\n", "");
            if (!line.isEmpty()) {
                QStringList list = line.split(";");
                QString key = list.at(0);
                QString value;
                for (int i = 1; i < list.count(); i++) {
                    value += QString("%1;").arg(list.at(i));
                }

                //去掉末尾分号
                value = value.mid(0, value.length() - 1);
                AppConfig::Keys.append(key);
                AppConfig::Values.append(value);
            }
        }

        file.close();
    }
}
