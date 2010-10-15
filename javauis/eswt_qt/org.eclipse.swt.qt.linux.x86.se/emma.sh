#!/bin/sh

# This runs eSWT MTs and takes coverage measurements.

# Start Xephyr and metacity window manager inside it
Xephyr -ac -screen 1024x768x24 -dpi 96 -br -reset -terminate -host-cursor 2> /dev/null :2 &
sleep 3
DISPLAY=:2 xterm -geometry 0x0 -hold -iconic &
DISPLAY=:2 metacity &
DISPLAY=:2 setxkbmap fi &
sleep 3

# Once the environment is set up, run the tests
DISPLAY=:2 java -Djava.library.path=./release emmarun -r txt,html -ix org.eclipse.swt*,org.eclipse.ercp.swt.mobile -cp ../nokiatests/junit-3.8.1.1.jar:eswtqt.jar:eswt-qt-tests.jar:../org.eclipse.swt/bin com.nokia.eswt.mt.AllTests 

