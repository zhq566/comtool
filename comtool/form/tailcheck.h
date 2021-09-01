#ifndef TAILCHECK_H
#define TAILCHECK_H

#include <QWidget>

namespace Ui
{
    class TailCheck;
}

class TailCheck : public QWidget
{
    Q_OBJECT

public:
    explicit TailCheck(QWidget *parent = nullptr);
    ~TailCheck();

private:
    Ui::TailCheck *ui;

friend class frmComTool;
};

#endif // TAILCHECK_H
