#ifndef SEGMENTATION_H
#define SEGMENTATION_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QProcess>

class Segmentation : public QWidget
{
    Q_OBJECT

public:
    explicit Segmentation(QWidget *parent = nullptr);
    ~Segmentation();

private slots:
    void selectImage();
    void displayOriginalImage(const QString &imagePath);
    void displayResult(const QString &resultImagePath);
    QString processImage(const QString &imagePath);
    void handleProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleProcessError(QProcess::ProcessError error);

private:
    void initUi();

    // UI组件
    QPushButton *selectImageButton;
    QLabel *originalImageLabel;
    QLabel *resultLabel;
    QLabel *originalImageTitle;
    QLabel *resultImageTitle;

    // 布局
    QVBoxLayout *mainLayout;
    QHBoxLayout *imageLayout;
    QVBoxLayout *originalLayout;
    QVBoxLayout *resultLayout;

    // 进程相关
    QProcess *pythonProcess;
    QString currentImagePath;
};

#endif // SEGMENTATION_H 