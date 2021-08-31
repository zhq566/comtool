#ifndef FRMCOMTOOL_H
#define FRMCOMTOOL_H

#include <QWidget>
#include "qtcpsocket.h"
#include "qextserialport.h"

namespace Ui
{
class frmComTool;
}

#define HIGH_LIGHT_COLOR_1      1
#define HIGH_LIGHT_COLOR_2      2
#define HIGH_LIGHT_COLOR_3      3
#define HIGH_LIGHT_TITLE_1 "高亮：颜色1"
#define HIGH_LIGHT_CANCEL_TITLE_1 "取消高亮：颜色1"
#define HIGH_LIGHT_TITLE_2 "高亮：颜色2"
#define HIGH_LIGHT_CANCEL_TITLE_2 "取消高亮：颜色2"
#define HIGH_LIGHT_TITLE_3 "高亮：颜色3"
#define HIGH_LIGHT_CANCEL_TITLE_3 "取消高亮：颜色3"

class frmComTool : public QWidget
{
    Q_OBJECT

public:
    explicit frmComTool(QWidget *parent = 0);
    ~frmComTool();

private:
    Ui::frmComTool *ui;

    bool comOk;                 //串口是否打开
    QextSerialPort *com;        //串口通信对象
    QTimer *timerRead;          //定时读取串口数据
    QTimer *timerSend;          //定时发送串口数据
    QTimer *timerSave;          //定时保存串口数据

    int sleepTime;              //接收延时时间
    int sendCount;              //发送数据计数
    int receiveCount;           //接收数据计数
    bool isShow;                //是否显示数据

    bool tcpOk;                 //网络是否正常
    QTcpSocket *socket;         //网络连接对象
    QTimer *timerConnect;       //定时器重连

    QAction *highLight1;        //高亮1
    QAction *highLight2;        //高亮2
    QAction *highLight3;        //高亮3

    QString highLight1_str;
    QString highLight2_str;
    QString highLight3_str;

    void HightLight(int type, QString highLightStr);    //高亮关键词
    void update_comName(void);

private slots:
    void initForm();            //初始化窗体数据
    void initConfig();          //初始化配置文件
    void saveConfig();          //保存配置文件
    void readData();            //读取串口数据
    void sendData();            //发送串口数据
    void sendData(QString data, int hexCheck);//发送串口数据带参数
    void saveData();            //保存串口数据

    void changeEnable(bool b);  //改变状态
    void append(int type, const QString &data, bool clear = false, bool isCheckHex = true);

    bool isCheckHex(int no);

private slots:
    void connectNet();
    void readDataNet();
    void readErrorNet();

private slots:
    void on_btnOpen_clicked();
    void on_btnStopShow_clicked();
    void on_btnSendCount_clicked();
    void on_btnReceiveCount_clicked();
    void on_btnClear_clicked();
    void on_btnData_clicked();
    void on_btnStart_clicked();
    void on_ckAutoSend_stateChanged(int arg1);
    void on_ckAutoSave_stateChanged(int arg1);
    void on_action_HighLight1_triggered();
    void on_action_HighLight2_triggered();
    void on_action_HighLight3_triggered();
    void on_expandButton_clicked();
    void on_sendButton_01_clicked();
    void on_sendButton_02_clicked();
    void on_sendButton_03_clicked();
    void on_sendButton_04_clicked();
    void on_sendButton_05_clicked();
    void on_sendButton_06_clicked();
    void on_sendButton_07_clicked();
    void on_sendButton_08_clicked();
    void on_sendButton_09_clicked();
    void on_cboxPortName_clicked();
};

#endif // FRMCOMTOOL_H
