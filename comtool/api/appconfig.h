#ifndef APPCONFIG_H
#define APPCONFIG_H

#include "head.h"

class AppConfig
{
public:
    static QString ConfigFile;          //配置文件路径
    static QString SendFileName;        //发送配置文件名
    static QString DeviceFileName;      //模拟设备数据文件名

    static QString PortName;            //串口号
    static int BaudRate;                //波特率
    static int DataBit;                 //数据位
    static QString Parity;              //校验位
    static double StopBit;              //停止位

    static bool HexSend;                //16进制发送
    static bool HexReceive;             //16进制接收
    static bool Debug;                  //模拟设备
    static bool AutoClear;              //自动清空

    static bool AutoSend;               //自动发送
    static int SendInterval;            //自动发送间隔
    static bool AutoSave;               //自动保存
    static int SaveInterval;            //自动保存间隔

    static QString Mode;                //转换模式
    static QString ServerIP;            //服务器IP
    static int ServerPort;              //服务器端口
    static int ListenPort;              //监听端口
    static int SleepTime;               //延时时间
    static bool AutoConnect;            //自动重连

    //读写配置参数及其他操作
    static void readConfig();           //读取配置参数
    static void writeConfig();          //写入配置参数

    static QStringList Intervals;
    static QStringList Datas;
    static QStringList Keys;
    static QStringList Values;
    static void readSendData();
    static void readDeviceData();

    //常用发送存储
    static QString sendTxt01;
    static QString sendTxt02;
    static QString sendTxt03;
    static QString sendTxt04;
    static QString sendTxt05;
    static QString sendTxt06;
    static QString sendTxt07;
    static QString sendTxt08;
    static QString sendTxt09;
    static bool sendTxt01_hex;
    static bool sendTxt02_hex;
    static bool sendTxt03_hex;
    static bool sendTxt04_hex;
    static bool sendTxt05_hex;
    static bool sendTxt06_hex;
    static bool sendTxt07_hex;
    static bool sendTxt08_hex;
    static bool sendTxt09_hex;

};

#endif // APPCONFIG_H
