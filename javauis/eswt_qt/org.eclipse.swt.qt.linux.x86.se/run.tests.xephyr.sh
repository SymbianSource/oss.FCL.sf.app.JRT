#!/bin/bash
Xephyr -ac -screen 1024x768x24 -dpi 96 -br -reset -terminate -host-cursor 2> /dev/null :2 &
sleep 3
echo $! > Xephyr.pid
DISPLAY=:2 xterm -geometry 0x0 -hold -iconic &
DISPLAY=:2 metacity &
DISPLAY=:2 setxkbmap fi &
sleep 3
DISPLAY=:2 java -cp ../nokiatests/bin:../xlibutils/java/bin:../org.eclipse.swt/bin:../eclipsetests/bin:../nokiatests/junit-3.8.1.1.jar -Djava.library.path=../xlibutils/native/release:release com.nokia.eswt.mt.AllTests > test.results.log
kill -9 `cat Xephyr.pid`
