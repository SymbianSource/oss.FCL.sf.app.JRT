/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Thread dump server of J9 VM in S60,
*
*/


#ifndef J9THREADDUMPER_H
#define J9THREADDUMPER_H

#include "jni.h"
#include "jvmri.h"

#include "threaddumper.h"

#include "monitor.h"

class DgRasInterface;

namespace java
{

namespace util
{

enum DumpTask
{
    NO_TASK,
    DO_DUMP,
    CLOSE
};

enum DumperState
{
    CREATED,
    ACTIVE,
    CLOSED
};

OS_NONSHARABLE_CLASS(J9ThreadDumper)
{
public:
    J9ThreadDumper(JavaVM* vm, DgRasInterface* jvmri);
    ~J9ThreadDumper();

    void doRun();

    void doDump();
    void close();

private:
    void wakeUp();
    void moveGeneratedDumpFile();

private:
    JavaVM*                            mJavaVM; //Not owned
    JNIEnv*                            mJNIEnv; //Not owned
    DgRasInterface*                    mJvmriInterface; //Not owned
    DumpTask                           mTask;
    DumperState                        mState;
    std::auto_ptr<java::util::Monitor> mMonitor;

};

} // namespace util
} // namespace java


#ifdef __SYMBIAN32__
IMPORT_C FuncPtr JNICALL jni_lookup(const char* aName);
#endif // __SYMBIAN32__

#endif // J9THREADDUMPER_H
