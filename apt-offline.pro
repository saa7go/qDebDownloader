TARGET = apt-offline

HEADERS += \
    aptweb.h \
    form.h \
    downloadtablemodel.h \
    downloaddata.h \
    downloaddataprivate.h \
    downloader.h \
    progressbardelegate.h \
    proxydialog.h \
    aboutdialog.h \
    filesizedownloader.h \
    helper.h \
    sortproxymodel.h

SOURCES += \
    aptweb.cpp \
    form.cpp \
    mainApp.cpp \
    downloadtablemodel.cpp \
    downloaddata.cpp \
    downloader.cpp \
    progressbardelegate.cpp \
    proxydialog.cpp \
    aboutdialog.cpp \
    filesizedownloader.cpp \
    helper.cpp \
    sortproxymodel.cpp

QT += network

FORMS += \
    form.ui \
    proxydialog.ui \
    aboutdialog.ui

RESOURCES += \
    image.qrc

win32:RC_FILE = apt-offline.rc
