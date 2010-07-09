TEMPLATE = lib
TARGET = swt-animations
QT += core \
    gui 

unix {
    INCLUDEPATH +=$(JAVA_HOME)/include \
        $(JAVA_HOME)/include/linux
}


# Input
HEADERS += os.h \
           swt.h

SOURCES += os.cpp \
