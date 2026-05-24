#include "segmentation.h"
#include <QPixmap>
#include <QDebug>
#include <QProcess>

Segmentation::Segmentation(QWidget *parent)
    : QWidget(parent)
    , pythonProcess(new QProcess(this))
{
    initUi();
    
    // 连接进程信号
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &Segmentation::handleProcessFinished);
    connect(pythonProcess, &QProcess::errorOccurred,
            this, &Segmentation::handleProcessError);
}

Segmentation::~Segmentation()
{
}

void Segmentation::initUi()
{
    // 创建主布局
    mainLayout = new QVBoxLayout(this);

    // 创建选择图片按钮
    selectImageButton = new QPushButton("选择图片", this);
    connect(selectImageButton, &QPushButton::clicked, this, &Segmentation::selectImage);
    mainLayout->addWidget(selectImageButton);

    // 创建图像显示布局
    imageLayout = new QHBoxLayout();

    // 创建原始图像布局
    originalLayout = new QVBoxLayout();
    originalImageLabel = new QLabel(this);
    originalImageLabel->setFixedSize(300, 300);
    originalImageLabel->setScaledContents(true);
    originalLayout->addWidget(originalImageLabel);
    originalImageTitle = new QLabel("原图", this);
    originalImageTitle->setAlignment(Qt::AlignCenter);
    originalLayout->addWidget(originalImageTitle);

    // 创建分割结果布局
    resultLayout = new QVBoxLayout();
    resultLabel = new QLabel(this);
    resultLabel->setFixedSize(300, 300);
    resultLabel->setScaledContents(true);
    resultLayout->addWidget(resultLabel);
    resultImageTitle = new QLabel("UNet 分割", this);
    resultImageTitle->setAlignment(Qt::AlignCenter);
    resultLayout->addWidget(resultImageTitle);

    // 将原始图像和分割结果的布局添加到水平布局中
    imageLayout->addLayout(originalLayout);
    imageLayout->addLayout(resultLayout);

    mainLayout->addLayout(imageLayout);
    setLayout(mainLayout);
}

void Segmentation::selectImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "选择图片", "",
        "Images (*.png *.jpg *.jpeg *.bmp);;All Files (*)");

    if (!fileName.isEmpty()) {
        displayOriginalImage(fileName);
        QString resultImagePath = processImage(fileName);
        displayResult(resultImagePath);
    }
}

void Segmentation::displayOriginalImage(const QString &imagePath)
{
    QPixmap pixmap(imagePath);
    originalImageLabel->setPixmap(pixmap);
}

QString Segmentation::processImage(const QString &imagePath)
{
    currentImagePath = imagePath;
    
    // 准备Python脚本路径和参数
    QStringList arguments;
    arguments << "predict.py" << imagePath;
    
    // 启动Python进程
    pythonProcess->start("python", arguments);
    
    // 等待进程完成
    if (!pythonProcess->waitForFinished(5000)) {  // 等待最多5秒
        qDebug() << "Process timeout";
        return imagePath;
    }
    
    // 读取输出
    QByteArray output = pythonProcess->readAllStandardOutput();
    QString result = QString::fromUtf8(output).trimmed();
    
    return result.isEmpty() ? imagePath : result;
}

void Segmentation::displayResult(const QString &resultImagePath)
{
    QPixmap pixmap(resultImagePath);
    resultLabel->setPixmap(pixmap);
}

void Segmentation::handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        QByteArray output = pythonProcess->readAllStandardOutput();
        QString result = QString::fromUtf8(output).trimmed();
        if (!result.isEmpty()) {
            displayResult(result);
        }
    } else {
        qDebug() << "Process failed with exit code:" << exitCode;
    }
}

void Segmentation::handleProcessError(QProcess::ProcessError error)
{
    qDebug() << "Process error:" << error;
} 