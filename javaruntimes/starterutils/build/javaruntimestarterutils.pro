#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#

TEMPLATE=lib
TARGET=javaruntimestarterutils
CONFIG += omj stl
CONFIG -= qt

LIBS += -ljavajvmargsmodifier -ljavacomms

!symbian {

    # Copy environment values to qmake variables
    JAVA_VM_ROOT = $$(JAVA_VM_ROOT)
    TARGET_MAEMO = $$(TARGET_MAEMO)
    JAVA_VM      = $$(JAVA_VM)
                   
    !isEmpty(JAVA_VM_ROOT) {
        LIBS += -L$${JAVA_VM_ROOT}

    } else : !isEmpty(TARGET_MAEMO) {
        LIBS += -L$(JAVA_BIN_ROOT)/lib/jvm/j9_23/bin
        DEFINES += J9_CDC_VM TARGET_MAEMO

    } else : contains(JAVA_VM, J9) {
        LIBS += -L/usr/lib/jvm/java-1.4.2-ibm/jre/bin

    } else {
        LIBS += -L/usr/lib/jvm/java-1.5.0-sun/jre/lib/i386/client
    }

    contains(JAVA_VM, J9) {

        LIBS += -lj9vm23 -lj9thr23 -lj9hookable23 -lj9prt23
    } else { 
        LIBS += -ljvm
    }
}

symbian {
    LIBS += -lcentralrepository
}

include(../../../build/omj.pri)
