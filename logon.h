#ifndef LOGON_H
#define LOGON_H

#include <QMainWindow>

namespace Ui {
class logon;
}

class logon : public QMainWindow
{
    Q_OBJECT

public:
    explicit logon(QWidget *parent = nullptr);
    ~logon();

    void read_json(); //读json
    void write_json();//写json
    void message_init(QString flag1,QString flag2);//根据json内容决定是否填充输入框
    QString m_username;//账号
    QString m_password;//密码


signals:
    void login(); //登录主界面信号
    void close_window(); //关闭登录界面信号
public slots:
    void btn_clear_clicked(); //重置按钮按下后触发的事件
    void btn_log_clicked(); //登录按钮按下后触发的事件

private:
    Ui::logon *ui;


};

#endif // LOGON_H
