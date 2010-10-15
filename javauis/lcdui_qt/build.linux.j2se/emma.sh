#!/bin/sh

# This runs LCDUI MTs and takes coverage measurements.

# Start Xephyr and metacity window manager inside it
Xephyr -ac -screen 1024x768x24 -dpi 96 -br -reset -terminate -host-cursor 2> /dev/null :2 &
sleep 3
DISPLAY=:2 xterm -geometry 0x0 -hold -iconic &
DISPLAY=:2 metacity &
DISPLAY=:2 setxkbmap fi &
sleep 3

# Once the environment is set up, run the tests
DISPLAY=:2 java -Djava.library.path=../../eswt_qt/org.eclipse.swt.qt.linux.x86.se/release emmarun -r txt,html -ix javax.microedition*,com.nokia.mid.ui* -cp ../../eswt_qt/nokiatests/junit-3.8.1.1.jar:../../eswt_qt/org.eclipse.swt.qt.linux.x86.se/eswtqt.jar:../../eswt_qt/org.eclipse.swt.qt.linux.x86.se/eswt-qt-tests.jar:openlcdui.jar:openlcdui_tests.jar:../../eswt_qt/org.eclipse.swt/bin com.nokia.openlcdui.mt.AllTests

