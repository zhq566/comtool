#ifndef TAILCHECK_H
#define TAILCHECK_H

#include <QWidget>
#include <QHideEvent>

namespace Ui
{
    class TailCheck;
}

#define TC_BTN_NUM      17

class TailCheck : public QWidget
{
    Q_OBJECT

public:
    explicit TailCheck(QWidget *parent = nullptr);
    ~TailCheck();

    int switchCheck;
    bool isLittleMode;
    QString TailCheckData(QByteArray data);

private:
    Ui::TailCheck *ui;
    void InitTailCheckForm(void);

protected:
    void hideEvent(QHideEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void radioBtn_Clicked(int);

signals:
    void hide_signal();
    void close_signal();
};

#endif // TAILCHECK_H
