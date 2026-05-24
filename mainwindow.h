#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSlider>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void slotUpdateJVarsValue(int value);
    void slotJVarsValueChange(int index, int value);



private slots:
    void portDetect();
    void portOpenClose();
    void sendMessage();
    void showSliderValue(int value);
    void move_Learm();

private:
    void initSerialPort();
    void sendData(const QByteArray &data);
    QString getPortName();
    // 范围映射函数
    double mapRange(double value, double inMin, double inMax, double outMin, double outMax);

    Ui::MainWindow *ui;
    QSerialPort *serialPort;
    QMap<QString, QString> portDict;
signals:
    void sigJoinValueChanged(int index, int value); // robot
};
#endif // MAINWINDOW_H
