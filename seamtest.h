#ifndef SEAMTEST_H
#define SEAMTEST_H

#include <QMainWindow>

#include<QProcess>
namespace Ui {
class seamtest;
}

class seamtest : public QMainWindow
{
    Q_OBJECT

public:
    explicit seamtest(QWidget *parent = nullptr);
    ~seamtest();
    void displayOriginalImage(const QString &imagePath);
    void selectImage();
    void processImage(const QString &imagePath);
    void displayResult(const QString &resultImagePath);

signals:
    void seam_detect_success();

private:
    Ui::seamtest *ui;
//    QProcess *process;  // 添加成员变量
};

#endif // SEAMTEST_H
