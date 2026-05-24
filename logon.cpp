#include "logon.h"
#include "ui_logon.h"
#include <QMessageBox>
#include<QFile>
#include<QDebug>
#include<QJsonDocument>
#include<QJsonObject>
#include "runtime_paths.h"

namespace {
const QString kDefaultUserName = "admin";
const QString kDefaultPassword = "admin";
}

logon::logon(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::logon)
{
    ui->setupUi(this);
    //重置按钮信号槽连接
    connect(ui->clean,SIGNAL(clicked()),this,SLOT(btn_clear_clicked()));

    //触发登录按钮信号槽连接
    connect(ui->login,SIGNAL(clicked()),this,SLOT(btn_log_clicked()));

    //发出信号后关闭登录界面的信号与槽连接
    connect(this,SIGNAL(close_window()),this,SLOT(close()));
    m_username = kDefaultUserName;
    m_password = kDefaultPassword;

    read_json();
}
void logon::read_json()
{
    QFile file(RuntimePaths::outputPath("config.json"));
    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"Config file not found";
        message_init(QString(), QString());
        return;

    }
    else
        qDebug()<<"File open successfully";
    QJsonDocument jdc(QJsonDocument::fromJson(file.readAll()));
    QJsonObject obj=jdc.object();
    QString save_name_flag=obj.value("SAVE_NAME").toString();
    QString save_password_flag=obj.value("SAVE_PASSWORD").toString();
    message_init(save_name_flag,save_password_flag);

}

void logon::write_json()
{
    QFile file(RuntimePaths::outputPath("config.json"));
        if(!file.open(QIODevice::WriteOnly)) {
            qDebug() << "File open failed!";
        } else {
            qDebug() <<"File open successfully!";
        }
        QJsonObject obj;
        bool flag=ui->re_account->isChecked();
        if(flag==true)
        {
            obj["SAVE_NAME"]="1";
        }
        else
            obj["SAVE_NAME"]="0";

        flag=ui->re_password->isChecked();
        if(flag==true)
        {
            obj["SAVE_PASSWORD"]="1";
        }
        else
            obj["SAVE_PASSWORD"]="0";

        QJsonDocument jdoc(obj);
        file.write(jdoc.toJson());
        file.flush();
}
//根据json内容决定是否填充输入框
void logon::message_init(QString flag1,QString flag2)
{
    //qDebug() << flag1 << "^^^" << flag2 ;
    if (flag1 == "1")
    {
        ui->user_name->setText(kDefaultUserName);
        ui->re_account->setChecked(true);
    }
    if(flag2 == "1")
    {
        ui->user_password->setText(kDefaultPassword);
        ui->re_password->setChecked(true);
    }
}

void logon::btn_clear_clicked()
{
    ui->user_name->clear();
    ui->user_password->clear();
}
void logon::btn_log_clicked()
{

    QString name= ui->user_name->text();
    QString password=ui->user_password->text();
    if(name==m_username&&password==m_password)
    {
        //发出登录信号
        emit(login());
        write_json();
        //发出关闭窗口信号
        emit(close_window());

    }
    else
        QMessageBox::information(this,"Warning","Username or Password is wrong ");


}

logon::~logon()
{
    delete ui;
}
