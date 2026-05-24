#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QIODevice>
#include "globaleventbus.h"
#include "seamtest.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialPort(new QSerialPort(this))
{
    ui->setupUi(this);
    initSerialPort();
}

MainWindow::~MainWindow()
{
    if (serialPort->isOpen()) {
        serialPort->close();
    }
    delete ui;
}

void MainWindow::initSerialPort()
{


    // 连接信号和槽
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::portDetect);  // 检查串口按钮
    connect(ui->pushButton_2, &QPushButton::clicked, this, &MainWindow::portOpenClose);  // 打开/关闭串口按钮
    connect(ui->pushButton_4, &QPushButton::clicked, this, &MainWindow::sendMessage);  // 发送按钮
//    connect(ui->)
    ui->horizontalSlider_1->setRange(500, 2500);
    ui->horizontalSlider_2->setRange(500, 2500);
    ui->horizontalSlider_3->setRange(500, 2500);
    ui->horizontalSlider_4->setRange(500, 2500);
    ui->horizontalSlider_5->setRange(500, 2500);
    ui->horizontalSlider_6->setRange(500, 2500);
    ui->horizontalSlider_1->setValue(1000);
    ui->horizontalSlider_2->setValue(1500);
    ui->horizontalSlider_3->setValue(1500);
    ui->horizontalSlider_4->setValue(1500);
    ui->horizontalSlider_5->setValue(1500);
    ui->horizontalSlider_6->setValue(1500);


    
    // 连接6个舵机滑块的信号，使用lambda表达式简化代码
    connect(ui->horizontalSlider_1, &QSlider::valueChanged, this, [this](int value) { showSliderValue(value); });
     connect(ui->horizontalSlider_1, &QSlider::valueChanged, this, &MainWindow::slotUpdateJVarsValue);

    connect(ui->horizontalSlider_2, &QSlider::valueChanged, this, [this](int value) { showSliderValue(value); });
    connect(ui->horizontalSlider_2, &QSlider::valueChanged, this, &MainWindow::slotUpdateJVarsValue);


    connect(ui->horizontalSlider_3, &QSlider::valueChanged, this, [this](int value) { showSliderValue(value); });
    connect(ui->horizontalSlider_3, &QSlider::valueChanged, this, &MainWindow::slotUpdateJVarsValue);

    connect(ui->horizontalSlider_4, &QSlider::valueChanged, this, [this](int value) { showSliderValue(value); });
    connect(ui->horizontalSlider_4, &QSlider::valueChanged, this, &MainWindow::slotUpdateJVarsValue);

    connect(ui->horizontalSlider_5, &QSlider::valueChanged, this, [this](int value) { showSliderValue(value); });
    connect(ui->horizontalSlider_5, &QSlider::valueChanged, this, &MainWindow::slotUpdateJVarsValue);

    connect(ui->horizontalSlider_6, &QSlider::valueChanged, this, [this](int value) { showSliderValue(value); });
    connect(ui->horizontalSlider_6, &QSlider::valueChanged, this, &MainWindow::slotUpdateJVarsValue);
    // 设置所有舵机滑块的范围（500-2000），对应舵机角度范围



    connect(this,&MainWindow::sigJoinValueChanged,this,&MainWindow::slotJVarsValueChange);


    // 初始化串口参数选项
    // 波特率选项：从100到256000
    ui->comboBox_2->addItems({"100", "300", "600", "1200", "2400", "4800", "9600", 
                             "14400", "19200", "38400", "56000", "57600", "115200", 
                             "128000", "256000"});
    ui->comboBox_2->setCurrentText("9600");  // 设置默认波特率

    // 数据位选项：8、7、6、5位
    ui->comboBox_3->addItems({"8", "7", "6", "5"});
    ui->comboBox_3->setCurrentText("8");  // 设置默认数据位

    // 校验位选项：N（无校验）、E（偶校验）、O（奇校验）
    ui->comboBox_4->addItems({"N", "E", "O"});
    ui->comboBox_4->setCurrentText("N");  // 设置默认校验位

    // 停止位选项：1、1.5、2位
    ui->comboBox_5->addItems({"1", "1.5", "2"});
    ui->comboBox_5->setCurrentText("1");  // 设置默认停止位


    //将识别模块的信号与当前移动到固定位置进行连接
    connect(globaleventbus::getInstance(),SIGNAL(seamDetectionSuccess()),this,SLOT(move_Learm()));
}
void MainWindow::move_Learm(){
    qDebug()<<"成功接收信号";
    ui->horizontalSlider_1->setValue(1015);
    ui->horizontalSlider_2->setValue(1162);
    ui->horizontalSlider_3->setValue(1757);
    ui->horizontalSlider_4->setValue(1647);
    ui->horizontalSlider_5->setValue(1412);
    ui->horizontalSlider_6->setValue(1640);

}
// 处理关节角度变化
void MainWindow::slotJVarsValueChange(int index, int value) {
    int mappedIndex=7-index;
    // 将 [500, 2000] 范围的值映射到 [-180, 180] 范围
    double mappedValue = mapRange(value, 500, 2500, -90, 90);
//    if(index==2){
//        mappedValue=mappedValue+180;
//    }
    if(index==5){
        mappedValue=mappedValue+90;
//        qDebug()<<"第五个舵机:"<<mappedValue;
    }
    if(index==3){
        mappedValue=mappedValue-90;
        mappedValue = -180 - mappedValue;
//        qDebug()<<"第五个舵机:"<<mappedValue;
    }
    if(index==4){
        mappedValue=-mappedValue;
    }

    ui->robot3D_virtual->mRobotConfig.JVars[mappedIndex] = mappedValue;
//    // 第2关节角度需要取反
//    if (index == 5) {
//        ui->robot3D_virtual->mRobotConfig.JVars[mappedIndex] = -mappedValue;
//    }
    ui->robot3D_virtual->update();

}
// 范围映射函数
double MainWindow::mapRange(double value, double inMin, double inMax, double outMin, double outMax) {
    // 确保值在输入范围内
    if (value < inMin) value = inMin;
    if (value > inMax) value = inMax;

    // 线性映射公式
    return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}
void MainWindow::portDetect()
{
    portDict.clear();
    ui->comboBox->clear();
    
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QString portName = info.portName();
        QString description = info.description();
        portDict[portName] = description;
        ui->comboBox->addItem(portName + ":" + description);
    }
    
    if (portDict.isEmpty()) {
        ui->comboBox->addItem("无串口");
    }
}

void MainWindow::portOpenClose()
{
    if (ui->pushButton_2->text() == "打开串口" && !portDict.isEmpty()) {
        // 设置串口参数
        serialPort->setPortName(getPortName());  // 设置串口名称
        serialPort->setBaudRate(ui->comboBox_2->currentText().toInt());  // 设置波特率
        serialPort->setDataBits(static_cast<QSerialPort::DataBits>(ui->comboBox_3->currentText().toInt()));  // 设置数据位
        serialPort->setParity(static_cast<QSerialPort::Parity>(ui->comboBox_4->currentText()[0].toLatin1()));  // 设置校验位
        serialPort->setStopBits(static_cast<QSerialPort::StopBits>(ui->comboBox_5->currentText().toInt()));  // 设置停止位

        // 尝试打开串口
        if (serialPort->open(QIODevice::ReadWrite)) {
            ui->pushButton_2->setText("关闭串口");
            QMessageBox::information(this, "串口信息", "串口打开成功");
        } else {
            QMessageBox::critical(this, "错误", "无法打开串口！");
        }
    } else if (ui->pushButton_2->text() == "关闭串口") {
        serialPort->close();  // 关闭串口
        ui->pushButton_2->setText("打开串口");
    }
}


// 更新关节角度值的槽函数
void MainWindow::slotUpdateJVarsValue(int value) {
    QSlider *slider = (QSlider *) sender();  // 获取发送信号的滑动条
    QString objectName = slider->objectName();  // 获取滑动条的对象名称
    QString index = objectName.at(objectName.size() - 1);  // 获取关节索引
    emit sigJoinValueChanged(index.toInt(), value);  // 发射信号，更新关节角度
}

void MainWindow::sendMessage()
{
    if (!serialPort->isOpen()) {
        QMessageBox::warning(this, "警告", "请先打开串口！");
        return;
    }

    QString text = ui->lineEdit->text();
    if (text.isEmpty()) {
        return;
    }

    QByteArray data;
    if (ui->checkBox->isChecked()) {
        // HEX模式发送
        QStringList hexValues = text.split(" ");
        for (const QString &hex : hexValues) {
            bool ok;
            int value = hex.toInt(&ok, 16);
            if (ok) {
                data.append(static_cast<char>(value));
            } else {
                QMessageBox::critical(this, "错误", "请输入有效的十六进制数据！");
                return;
            }
        }
    } else {
        // 文本模式发送
        data = text.toUtf8();
    }

    sendData(data);
}

void MainWindow::showSliderValue(int value)
{
    // 获取发送信号的滑块对象
    QSlider *slider = qobject_cast<QSlider*>(sender());
    if (!slider) return;

    // 确定是哪个舵机的滑块
    int sliderNumber = 0;
    if (slider == ui->horizontalSlider_1) sliderNumber = 1;
    else if (slider == ui->horizontalSlider_2) sliderNumber = 2;
    else if (slider == ui->horizontalSlider_3) sliderNumber = 3;
    else if (slider == ui->horizontalSlider_4) sliderNumber = 4;
    else if (slider == ui->horizontalSlider_5) sliderNumber = 5;
    else if (slider == ui->horizontalSlider_6) sliderNumber = 6;

    // 更新对应的显示框
    QLineEdit *display = nullptr;
    switch(sliderNumber) {
        case 1: display = ui->lineEdit_2; break;
        case 2: display = ui->lineEdit_3; break;
        case 3: display = ui->lineEdit_4; break;
        case 4: display = ui->lineEdit_5; break;
        case 5: display = ui->lineEdit_6; break;
        case 6: display = ui->lineEdit_7; break;
    }
    
    // 更新显示框的值
    if (display) {
        display->setText(QString::number(value));
    }

    // 构建舵机控制命令
    QByteArray data;
    // 使用static_cast<char>明确指定数据类型，避免append函数重载的歧义
    data.append(static_cast<char>(0x55));  // 帧头1：固定值0x55
    data.append(static_cast<char>(0x55));  // 帧头2：固定值0x55
    data.append(static_cast<char>(0x08));  // 长度：数据长度8字节
    data.append(static_cast<char>(0x03));  // 命令：舵机控制命令
    data.append(static_cast<char>(0x01));  // 参数1：固定值0x01
    data.append(static_cast<char>(0xC8));  // 参数2：固定值0xC8
    data.append(static_cast<char>(0x00));  // 参数3：固定值0x00
    data.append(static_cast<char>(sliderNumber));  // 舵机编号：1-6
    data.append(static_cast<char>(value & 0xFF));  // 低字节：舵机角度值的低8位
    data.append(static_cast<char>((value >> 8) & 0xFF));  // 高字节：舵机角度值的高8位


    // 发送数据到串口
    sendData(data);
}

void MainWindow::sendData(const QByteArray &data)
{
//     if (serialPort->isOpen()) {
//         qDebug()<<"进行信息输出";
        
//         // 打印发送的数据内容
// //        QString hexData;
// //        for (int i = 0; i < data.size(); ++i) {
// //            hexData.append(QString("%1 ").arg((uchar)data[i], 2, 16, QChar('0')).toUpper());
// //        }
//         qDebug() << "发送数据内容:" << data;
//         int write_tf;
//         write_tf=  serialPort->write(data);
//         if(write_tf==-1)
//         {
//             qDebug()<<"发送失败";
//         }
//     }
    if (serialPort->isOpen()) {
        qDebug() << "发送数据包大小:" << data.size() << "字节";
        
        // 打印发送的数据内容
        QString hexData;
        for (int i = 0; i < data.size(); ++i) {
            hexData.append(QString("%1 ").arg((uchar)data[i], 2, 16, QChar('0')).toUpper());
        }
        qDebug() << "发送数据内容:" << hexData;
        qDebug() << "data发送数据内容:" << data;
        
        qint64 writeResult = serialPort->write(data);
        if(writeResult == -1) {
            qDebug() << "发送失败:" << serialPort->errorString();
            QMessageBox::warning(this, "发送错误", "数据发送失败！");
        } else {
            // 等待数据发送完成
            if (!serialPort->waitForBytesWritten(1000)) {  // 等待最多1秒
                qDebug() << "等待数据发送超时";
                QMessageBox::warning(this, "发送错误", "数据发送超时！");
            } else {
                qDebug() << "发送成功，发送字节数:" << writeResult;
            }
//            emit serialPort->bytesWritten(writeResult);
        }
    }
}

QString MainWindow::getPortName()
{
    QString fullName = ui->comboBox->currentText();
    return fullName.left(fullName.indexOf(':'));
}

