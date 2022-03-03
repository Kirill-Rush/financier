QT       += core gui
QT += axcontainer
//QT += webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    additionalwindow.cpp \
    export_to_xls.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    additionalwindow.h \
    export_to_xls.h \
    mainwindow.h

FORMS += \
    additionalwindow.ui \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../AdditionalLibraries/PDFNetC64/Lib/ -lPDFNetC
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../AdditionalLibraries/PDFNetC64/Lib/ -lPDFNetC
else:unix: LIBS += -L$$PWD/../../../AdditionalLibraries/PDFNetC64/Lib/ -lPDFNetC

INCLUDEPATH += $$PWD/../../../AdditionalLibraries/PDFNetC64/Headers
DEPENDPATH += $$PWD/../../../AdditionalLibraries/PDFNetC64/Headers

DISTFILES += \
    Dependencies/json/include/meson.build
