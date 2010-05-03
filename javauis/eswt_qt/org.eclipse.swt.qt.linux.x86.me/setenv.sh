#!/bin/sh
export JAVA_SRC_ROOT=~/java
export JAVA_BIN_ROOT=~/java_bin
export JAVA_HOME=/usr/lib/jvm/java-1.5.0-sun
export JAVA_VM_HOME=$JAVA_HOME
export PATH=/usr/local/Trolltech/Qt-4.4.3/bin:$PATH
export PATH=$JAVA_VM_HOME/jre/lib/i386/client:$PATH:$JAVA_BIN_ROOT/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JAVA_BIN_ROOT/lib:$JAVA_VM_HOME/jre/lib/i386/client:$JAVA_VM_HOME/jre/lib/i386
