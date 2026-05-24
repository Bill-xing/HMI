#include "mainwindow.h"
#include "seamtest.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QLineEdit>
#include <QTimer>
#include "logon.h"
#include "runtime_paths.h"
#include "secinterface.h"

namespace {
bool checkSelfTestCondition(bool condition, const QString &message)
{
    if (!condition) {
        qCritical() << "SELF_TEST_FAIL:" << message;
        return false;
    }
    return true;
}

int runSelfTest(QApplication &app)
{
    const QString projectRoot = RuntimePaths::projectRoot();
    const QString testImage = QString::fromLocal8Bit(qgetenv("HMI_TEST_IMAGE"));

    qInfo() << "SELF_TEST_START";
    qInfo() << "projectRoot=" << projectRoot;
    qInfo() << "testImage=" << testImage;

    bool ok = true;
    ok &= checkSelfTestCondition(QFile::exists(QDir(projectRoot).filePath("predict.py")),
                                 QStringLiteral("predict.py not found under project root"));
    ok &= checkSelfTestCondition(QFile::exists(QDir(projectRoot).filePath("model_data/seam_unet.pth")),
                                 QStringLiteral("model_data/seam_unet.pth not found"));
    ok &= checkSelfTestCondition(!testImage.isEmpty(),
                                 QStringLiteral("HMI_TEST_IMAGE is not set"));
    ok &= checkSelfTestCondition(QFile::exists(testImage),
                                 QStringLiteral("test image not found"));
    if (!ok) {
        return 1;
    }

    logon loginWindow;
    bool loginEmitted = false;
    QObject::connect(&loginWindow, &logon::login, [&loginEmitted]() {
        loginEmitted = true;
    });

    QLineEdit *userName = loginWindow.findChild<QLineEdit *>(QStringLiteral("user_name"));
    QLineEdit *password = loginWindow.findChild<QLineEdit *>(QStringLiteral("user_password"));
    ok &= checkSelfTestCondition(userName != nullptr, QStringLiteral("login user_name field not found"));
    ok &= checkSelfTestCondition(password != nullptr, QStringLiteral("login user_password field not found"));
    if (!ok) {
        return 1;
    }

    userName->setText(QStringLiteral("admin"));
    password->setText(QStringLiteral("admin"));
    QMetaObject::invokeMethod(&loginWindow, "btn_log_clicked", Qt::DirectConnection);
    app.processEvents();
    ok &= checkSelfTestCondition(loginEmitted, QStringLiteral("default admin login did not emit login signal"));
    if (!ok) {
        return 1;
    }
    qInfo() << "SELF_TEST_LOGIN_OK";

    MainWindow controlWindow;
    controlWindow.hide();
    qInfo() << "SELF_TEST_MAINWINDOW_OK";

    seamtest seamWindow;
    seamWindow.hide();
    bool seamSignalEmitted = false;
    QObject::connect(&seamWindow, &seamtest::seam_detect_success, [&]() {
        seamSignalEmitted = true;
        app.quit();
    });

    QFile::remove(RuntimePaths::outputPath("segmented_image.png"));
    seamWindow.processImage(testImage);
    QTimer::singleShot(90000, &app, &QCoreApplication::quit);
    app.exec();

    const QString resultPath = RuntimePaths::outputPath("segmented_image.png");
    ok &= checkSelfTestCondition(seamSignalEmitted, QStringLiteral("seam_detect_success signal was not emitted"));
    ok &= checkSelfTestCondition(QFile::exists(resultPath), QStringLiteral("segmented_image.png was not generated"));
    ok &= checkSelfTestCondition(QFile(resultPath).size() > 0, QStringLiteral("segmented_image.png is empty"));
    if (!ok) {
        return 1;
    }

    qInfo() << "SELF_TEST_SEAM_PROCESS_OK" << resultPath;
    qInfo() << "SELF_TEST_PASS";
    return 0;
}
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (a.arguments().contains(QStringLiteral("--self-test"))) {
        return runSelfTest(a);
    }
//    MainWindow w;
//    seamtest w2;
//    w.show();
//    w2.show();
    Secinterface w;
//    w.show();

    return a.exec();
}
