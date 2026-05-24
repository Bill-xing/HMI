#ifndef GLOBALEVENTBUS_H
#define GLOBALEVENTBUS_H

#include<QObject>
class globaleventbus:public QObject
{
    Q_OBJECT
public:
//    globaleventbus();
    static globaleventbus* getInstance() {
            static globaleventbus instance;
            return &instance;
        }
signals:
    void seamDetectionSuccess();

public slots:
    void onSeamDetectionSuccess() {
        emit seamDetectionSuccess();
    }
};

#endif // GLOBALEVENTBUS_H
