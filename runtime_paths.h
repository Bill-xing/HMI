#ifndef RUNTIME_PATHS_H
#define RUNTIME_PATHS_H

#include <QString>

namespace RuntimePaths {

QString envValue(const char *name, const QString &fallback = QString());
QString projectRoot();
QString resourcePath(const QString &relativePath);
QString pythonHome();
QString pythonExecutable();
QString pythonStringLiteral(const QString &value);
QString outputDir();
QString outputPath(const QString &fileName);

}

#endif // RUNTIME_PATHS_H
