#include "secinterface.h"
#include "ui_secinterface.h"

Secinterface::Secinterface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Secinterface)
{
    ui->setupUi(this);

    m_log= new logon;
    m_log->show();


    //建立信号，当登录界面关闭时，调用主窗口的show函数
    connect(m_log,SIGNAL(login()),this,SLOT(show()));
    connect(ui->control,&QPushButton::clicked,this,[this](){
        w.show();
    });
    connect(ui->seem_identify,&QPushButton::clicked,this,[this](){
        w2.show();
    });
}

Secinterface::~Secinterface()
{
    delete ui;
}
