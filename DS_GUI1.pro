QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Backend/Message.cpp \
    Backend/Peer.cpp \
    Backend/Server.cpp \
    Backend/UDPClientSocket.cpp \
    Backend/UDPServerSocket.cpp \
    Backend/UDPSocket.cpp \
    ParseUtil.cpp \
    browser.cpp \
    globals.cpp \
    home.cpp \
    login_scr.cpp \
    main.cpp \
    mainwindow.cpp \
    reg_scr.cpp \
    visitprofile.cpp

HEADERS += \
    Backend/Image.h \
    Backend/Message.h \
    Backend/ParseUtil.h \
    Backend/Peer.h \
    Backend/Server.h \
    Backend/UDPClientSocket.h \
    Backend/UDPServerSocket.h \
    Backend/UDPSocket.h \
    browser.h \
    globals.h \
    home.h \
    login_scr.h \
    mainwindow.h \
    reg_scr.h \
    visitprofile.h

FORMS += \
    browser.ui \
    home.ui \
    login_scr.ui \
    mainwindow.ui \
    reg_scr.ui \
    visitprofile.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Backend/Client.out \
    Backend/Operations.docx \
    Backend/Report V1.docx \
    Backend/Report V1.pdf \
    Backend/Report V2.docx \
    Backend/Report V2.pdf \
    Backend/SPACE.JPG \
    Backend/Server.out \
    Backend/Tests/MessageTest.exe \
    Backend/_gitignore \
    Backend/brokerCompileRun \
    Backend/fadipic.png \
    Backend/peerCompileRun \
    Backend/skull2.png
