#ifndef SECINTERFACE_H
#define SECINTERFACE_H
#include "logon.h"
#include <QMainWindow>
#include "mainwindow.h"
#include "seamtest.h"
namespace Ui {
class Secinterface;
}

class Secinterface : public QMainWindow
{
    Q_OBJECT

public:
    explicit Secinterface(QWidget *parent = nullptr);
    ~Secinterface();
    MainWindow w;
    seamtest w2;
private:
    Ui::Secinterface *ui;

    //登录界面类的对象作为指针
    logon * m_log;

};

#endif // SECINTERFACE_H
