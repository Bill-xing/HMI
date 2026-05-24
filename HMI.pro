QT       += core gui
QT       += serialport
QT       += opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS



# Python configuration.
# Set PYTHON_HOME to the root of the Python/Conda environment used by the
# embedded interpreter, for example:
#   Windows: set PYTHON_HOME=C:/Users/you/anaconda3/envs/pytorch
#   macOS/Linux: export PYTHON_HOME=/path/to/venv
PYTHON_HOME = $$(PYTHON_HOME)
PYTHON_VERSION = $$(PYTHON_VERSION)
isEmpty(PYTHON_VERSION): PYTHON_VERSION = 3.6
PYTHON_VERSION_NODOT = $$replace(PYTHON_VERSION, \\., )

isEmpty(PYTHON_HOME) {
    message("PYTHON_HOME is not set. Set it if Python.h or libpython cannot be found.")
} else {
    INCLUDEPATH += $$PYTHON_HOME/include
    unix: INCLUDEPATH += $$PYTHON_HOME/include/python$${PYTHON_VERSION}
    win32: LIBS += -L$$PYTHON_HOME/libs -lpython$${PYTHON_VERSION_NODOT}
    unix: LIBS += -L$$PYTHON_HOME/lib -lpython$${PYTHON_VERSION}
    macx: QMAKE_LFLAGS += -Wl,-rpath,$$PYTHON_HOME/lib
    macx: QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Resources/python/lib
}


SOURCES += \
    ddr6robotwidget.cpp \
    globaleventbus.cpp \
    logon.cpp \
    main.cpp \
    mainwindow.cpp \
    rrglwidget.cpp \
    runtime_paths.cpp \
    seamtest.cpp \
    secinterface.cpp \
    stlfileloader.cpp

HEADERS += \
    ddr6robotwidget.h \
    globaleventbus.h \
    logon.h \
    mainwindow.h \
    rrglwidget.h \
    runtime_paths.h \
    seamtest.h \
    secinterface.h \
    stlfileloader.h

FORMS += \
    logon.ui \
    mainwindow.ui \
    seamtest.ui \
    secinterface.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    res/binary/README.md \
    res/image/app.ico \
    res/image/bg.jpg

win32: LIBS += -lOpenGL32

RESOURCES += \
    res.qrc
