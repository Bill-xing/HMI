#include "runtime_paths.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>

namespace {

QString absoluteCleanPath(const QString &path)
{
    return QDir(path).absolutePath();
}

void appendExistingCandidate(QStringList &candidates, const QString &path)
{
    if (path.isEmpty()) {
        return;
    }

    const QString absolute = absoluteCleanPath(path);
    if (!candidates.contains(absolute)) {
        candidates << absolute;
    }
}

QString findRootContaining(const QString &requiredRelativePath)
{
    const QString envRoot = RuntimePaths::envValue("HMI_PROJECT_ROOT");
    if (!envRoot.isEmpty()) {
        return absoluteCleanPath(envRoot);
    }

    QStringList candidates;
    QDir appDir(QCoreApplication::applicationDirPath());
    appendExistingCandidate(candidates, QDir::currentPath());
    appendExistingCandidate(candidates, appDir.absolutePath());
    appendExistingCandidate(candidates, appDir.filePath("HMI"));
    appendExistingCandidate(candidates, appDir.filePath("runtime/HMI"));
    appendExistingCandidate(candidates, appDir.filePath("resources/HMI"));

    QDir walker(appDir);
    for (int i = 0; i < 5; ++i) {
        appendExistingCandidate(candidates, walker.absolutePath());
        appendExistingCandidate(candidates, walker.filePath("HMI"));
        appendExistingCandidate(candidates, walker.filePath("Resources/HMI"));
        appendExistingCandidate(candidates, walker.filePath("resources/HMI"));
        walker.cdUp();
    }

    for (const QString &candidate : candidates) {
        if (QFileInfo::exists(QDir(candidate).filePath(requiredRelativePath))) {
            return absoluteCleanPath(candidate);
        }
    }

    return absoluteCleanPath(QDir::currentPath());
}

QString firstExistingPythonHome()
{
    const QString envHome = RuntimePaths::envValue("PYTHON_HOME");
    if (!envHome.isEmpty()) {
        return absoluteCleanPath(envHome);
    }

    QStringList candidates;
    QDir appDir(QCoreApplication::applicationDirPath());
    appendExistingCandidate(candidates, appDir.filePath("python"));
    appendExistingCandidate(candidates, appDir.filePath("runtime/python"));
    appendExistingCandidate(candidates, appDir.filePath("resources/python"));

    QDir walker(appDir);
    for (int i = 0; i < 5; ++i) {
        appendExistingCandidate(candidates, walker.filePath("python"));
        appendExistingCandidate(candidates, walker.filePath("Resources/python"));
        appendExistingCandidate(candidates, walker.filePath("resources/python"));
        walker.cdUp();
    }

    for (const QString &candidate : candidates) {
#ifdef Q_OS_WIN
        const QString executable = QDir(candidate).filePath("python.exe");
#else
        const QString executable = QDir(candidate).filePath("bin/python");
#endif
        if (QFileInfo::exists(executable)) {
            return absoluteCleanPath(candidate);
        }
    }

    return QString();
}

}

namespace RuntimePaths {

QString envValue(const char *name, const QString &fallback)
{
    const QByteArray value = qgetenv(name);
    return value.isEmpty() ? fallback : QString::fromLocal8Bit(value);
}

QString projectRoot()
{
    return findRootContaining(QStringLiteral("predict.py"));
}

QString resourcePath(const QString &relativePath)
{
    return QDir(projectRoot()).filePath(relativePath);
}

QString pythonHome()
{
    return firstExistingPythonHome();
}

QString pythonExecutable()
{
    const QString envExecutable = envValue("PYTHON_EXECUTABLE");
    if (!envExecutable.isEmpty()) {
        return envExecutable;
    }

    const QString home = pythonHome();
    if (!home.isEmpty()) {
#ifdef Q_OS_WIN
        return QDir(home).filePath("python.exe");
#else
        return QDir(home).filePath("bin/python");
#endif
    }

    return QStringLiteral("python");
}

QString pythonStringLiteral(const QString &value)
{
    QString escaped = value;
    escaped.replace("\\", "\\\\");
    escaped.replace("'", "\\'");
    return escaped;
}

QString outputDir()
{
    const QString envDir = envValue("HMI_OUTPUT_DIR");
    QString dir = envDir;
    if (dir.isEmpty()) {
        dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    if (dir.isEmpty()) {
        dir = QDir::temp().filePath("HMI");
    }

    QDir().mkpath(dir);
    return absoluteCleanPath(dir);
}

QString outputPath(const QString &fileName)
{
    return QDir(outputDir()).filePath(fileName);
}

}
