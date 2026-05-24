#include "seamtest.h"
#include "ui_seamtest.h"
#include <math.h>
#pragma push_macro("slots")
#undef slots
#include <Python.h>
#pragma pop_macro("slots")
#include<QDebug>
#include <iostream>
#include <string>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QPixmap>
#include <QStandardPaths>
#include <QStringList>
#include<QThread>
#include<QProcess>
#include<QTimer>
#include "globaleventbus.h"
#include "runtime_paths.h"

namespace {
}

seamtest::seamtest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::seamtest)
{
    ui->setupUi(this);
//下面这个语句解决出现的闪退Fatal
//    Python error: Py_Initialize: unable to load the file system codec
//ModuleNotFoundError: No module named 'encodings'
    const QString pythonHome = RuntimePaths::pythonHome();
    static std::wstring pythonHomeStorage;
    if (!pythonHome.isEmpty()) {
        pythonHomeStorage = QDir::toNativeSeparators(pythonHome).toStdWString();
        Py_SetPythonHome(const_cast<wchar_t *>(pythonHomeStorage.c_str()));
    }

    Py_Initialize();
    if (!Py_IsInitialized())  // 检查 Python 是否成功初始化
    {
        std::cout << "python init failed" << std::endl;

    }
    // 添加Python模块搜索路径
    PyRun_SimpleString("import sys");
    const QByteArray appendProjectPath = QString("sys.path.append('%1')")
            .arg(RuntimePaths::pythonStringLiteral(RuntimePaths::projectRoot()))
            .toUtf8();
    PyRun_SimpleString(appendProjectPath.constData());

    connect(ui->pushButton, &QPushButton::clicked, this, &seamtest::selectImage);
    connect(this,SIGNAL(seam_detect_success()),globaleventbus::getInstance(),SLOT(onSeamDetectionSuccess()));
}
void seamtest::selectImage()
{
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (defaultDir.isEmpty() || !QDir(defaultDir).exists()) {
        defaultDir = RuntimePaths::projectRoot();
    }

//    QString fileName = QFileDialog::getOpenFileName(this, "选择图片", "", "Images (*.png *.jpg *.jpeg *.bmp);;All Files (*)");
    QString fileName = QFileDialog::getOpenFileName(
            this,
            "选择图片",
            defaultDir,  // 使用设置的默认目录
            "Images (*.png *.jpg *.jpeg *.bmp);;All Files (*)"
    );
    if(!fileName.isEmpty()){
        displayOriginalImage(fileName);
        processImage(fileName);
    }
}
void seamtest::displayOriginalImage(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    ui->label->setPixmap(pixmap.scaled(ui->label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

}

void seamtest::processImage(const QString &imagePath)
{

    QProcess *process = new QProcess(this);

    const QString projectRoot = RuntimePaths::projectRoot();
    process->setWorkingDirectory(RuntimePaths::outputDir());


    // 连接更多的信号来捕获错误
        connect(process, &QProcess::errorOccurred, [=](QProcess::ProcessError error) {
            switch (error) {
                case QProcess::FailedToStart:
                    qDebug() << "进程启动失败";
                    break;
                case QProcess::Crashed:
                    qDebug() << "进程崩溃";
                    break;
                case QProcess::Timedout:
                    qDebug() << "进程超时";
                    break;
                case QProcess::WriteError:
                    qDebug() << "写入错误";
                    break;
                case QProcess::ReadError:
                    qDebug() << "读取错误";
                    break;
                default:
                    qDebug() << "未知错误";
                    break;
            }
        });
    // 连接信号槽以获取输出
        connect(process, &QProcess::readyReadStandardOutput, [=]() {
            QString output = QString::fromLocal8Bit(process->readAllStandardOutput());
            qDebug() << "标准输出:" << output;

//            if(output.contains("结果图片显示成功")){
//                qDebug()<<"检测到图像分割成功，将控制机械臂移动";

//            }
        });
        connect(process, &QProcess::readyReadStandardError, [=]() {
                QString error = QString::fromLocal8Bit(process->readAllStandardError());
                qDebug() << "错误输出:" << error;
            });
        // 处理完成的信号
        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                [=](int exitCode, QProcess::ExitStatus exitStatus) {
            Q_UNUSED(exitStatus);
            qDebug() << "进程结束，退出码:" << exitCode;

            if (exitCode == 0) {
                        // 处理成功，加载结果图片
                        QString resultPath = RuntimePaths::outputPath("segmented_image.png");
                        qDebug() << "尝试加载结果图片:" << resultPath;

                        // 使用QTimer确保文件写入完成
                        QTimer::singleShot(500, this, [=]() {
                            if (QFile::exists(resultPath)) {
                                displayResult(resultPath);  // 在这里调用显示结果
                                qDebug() << "结果图片显示成功";
                                qDebug()<<"检测到图像分割成功，将控制机械臂移动";
                                emit(seam_detect_success());
                            } else {
                                qDebug() << "结果文件不存在:" << resultPath;
                            }
                        });
                    } else {
                        qDebug() << "处理失败";
                    }

            process->deleteLater();  // 清理进程
        });

        // 转换路径中的反斜杠为正斜杠
            QString normalizedPath = imagePath;
            normalizedPath.replace(QChar('\\'), QChar('/'));
            QString pythonExecutable = RuntimePaths::pythonExecutable();
            QStringList args;
            args << QDir(projectRoot).filePath("predict.py") << normalizedPath;

        qDebug() << "执行命令:" << pythonExecutable << args;


        // 启动进程
            process->start(pythonExecutable, args);
            // 检查进程是否成功启动
                if (!process->waitForStarted()) {
                    qDebug() << "进程启动失败:" << process->errorString();
                    process->deleteLater();
//                    return;
                }

}
void seamtest::displayResult(const QString &resultImagePath)
{
    QPixmap pixmap(resultImagePath);
    ui->label_2->setPixmap(pixmap.scaled(ui->label_2->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

seamtest::~seamtest()
{

    delete ui;
}
