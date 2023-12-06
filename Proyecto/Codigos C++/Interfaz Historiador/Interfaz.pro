#Configuración del proyecto que se creo en QT Creator
#Se configura el core para las funciones de qmake y para habilitar el uso de sockets
QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#Se configura la version de C++ que se va a utilizar
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#Se definen los archivos fuente y los headers respectivos, y el código que contiene el diseño de la interfaz.
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    socket_udp.cpp

HEADERS += \
    mainwindow.h \
    socket_udp.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
