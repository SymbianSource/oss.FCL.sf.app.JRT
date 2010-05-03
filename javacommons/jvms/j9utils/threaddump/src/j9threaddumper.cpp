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


#include <string> // Needed for strcmp

#include "logger.h"
#include "fileutilities.h"
#include "exceptionbase.h"
#include "javacommonutils.h"
#include "javasymbianoslayer.h"

#include "j9threaddumper.h"

using namespace java::util;

// ======== STATIC VARIABLES ========

OS_NONSHARABLE_CLASS(ThreadDumperGlobals)
{
public:
    ThreadDumperGlobals() : mDumper(0)
    {
    }

public:
    J9ThreadDumper*  mDumper;
};


#if defined(__WINSCW__)

#include <pls.h>
ThreadDumperGlobals* getThreadDumperGlobals()
{
    // Access the PLS of this process
    ThreadDumperGlobals* globals  =
        Pls<ThreadDumperGlobals>(TUid::Uid(0x20022E77));
    return globals;
}

#else

static ThreadDumperGlobals* sGlobals = 0;

ThreadDumperGlobals* getThreadDumperGlobals()
{
    if (sGlobals == 0)
    {
        sGlobals = new ThreadDumperGlobals();
    }
    return sGlobals;
}
#endif


// Function prototypes
void JNICALL agentThread(void* args);
jint JNICALL JVM_OnLoad(JavaVM* vm, char* options, void* reserved);



J9ThreadDumper::J9ThreadDumper(JavaVM* vm, DgRasInterface* jvmri) :
        mJavaVM(vm), mJNIEnv(0), mJvmriInterface(jvmri),
        mTask(NO_TASK), mState(CREATED)
{
    JELOG2(EJavaRuntime);
}

J9ThreadDumper::~J9ThreadDumper()
{
    JELOG2(EJavaRuntime);
}


void JNICALL agentThread(void* /*args*/)
{
#ifdef __SYMBIAN32__
    RThread().SetPriority(EPriorityMore);
#endif // __SYMBIAN32__
    JELOG2(EJavaRuntime);
    getThreadDumperGlobals()->mDumper->doRun();
    delete getThreadDumperGlobals()->mDumper;
    getThreadDumperGlobals()->mDumper = 0;
#ifndef __WINSCW__
    delete sGlobals;
    sGlobals = 0;
#endif

}

void J9ThreadDumper::doDump()
{
    JELOG2(EJavaRuntime);
    mTask = DO_DUMP;
    wakeUp();
}

void J9ThreadDumper::close()
{
    JELOG2(EJavaRuntime);
    mTask = CLOSE;
    wakeUp();
    mState = CLOSED;
}



void J9ThreadDumper::moveGeneratedDumpFile()
{
#ifdef __SYMBIAN32__
    JELOG2(EJavaRuntime);
    const wchar_t* const midpPrivateDataCage = L"c:\\private\\102033E6";
    const wchar_t* const destRoot = L"c:\\logs\\java\\";


    // Ensure that the target dir exists.
    std::wstring dest(destRoot);
    std::replace(dest.begin(), dest.end(), '\\', '/');
    java::fileutils::FileUtilities::makeDirAll(dest);

    // Get all the files of the private data cage.
    std::list<std::wstring> dirList = java::fileutils::FileUtilities::getDirContentsList(midpPrivateDataCage);

    // Loop all existing files and try to find generated core dumps.
    std::list<std::wstring>::const_iterator it;
    for (it = dirList.begin(); it != dirList.end(); it++)
    {
        if (it->find(L"javacore.") == 0)
        {
            std::wstring oldName(midpPrivateDataCage);
            oldName += L"\\";
            oldName += *it;
            std::wstring newName(destRoot);
            newName += *it;

            char* oldUtf8Name = JavaCommonUtils::wstringToUtf8(oldName);
            char* newUtf8Name = JavaCommonUtils::wstringToUtf8(newName);

            int error = rename(oldUtf8Name, newUtf8Name);
            delete[] oldUtf8Name;
            delete[] newUtf8Name;
        }
    }
#endif // __SYMBIAN32__
}

void J9ThreadDumper::wakeUp()
{
    JELOG2(EJavaRuntime);
    if (mState == ACTIVE)
    {
        mMonitor->notify();
    }
}

void J9ThreadDumper::doRun()
{
    JELOG2(EJavaRuntime);
    if (mState != CREATED)
    {
        return;
    }

    int status = mJavaVM->AttachCurrentThreadAsDaemon((void**)&mJNIEnv, 0);
    if (status == 0)
    {
        mMonitor.reset(Monitor::createMonitor());
        mState = ACTIVE;
        while (mState == ACTIVE)
        {
            mMonitor->wait();
            switch (mTask)
            {
            case DO_DUMP:
                mJvmriInterface->GenerateJavacore(mJNIEnv);
                moveGeneratedDumpFile();
                break;
            case CLOSE:
                mState = CLOSED;
                break;
            }
            mTask = NO_TASK;
        }
        mJavaVM->DetachCurrentThread();
    }
    else
    {
        ELOG1(EUtils, "Thread dumper failed to attach to VM: %d", status);
    }
}

JNIEXPORT jint JNICALL JVM_OnLoad(JavaVM* vm, char* options, void* /*reserved*/)
{
    JELOG2(EJavaRuntime);
    int     rc = -1;
    JNIEnv* env;

    try
    {
        // Get the JNIEnv for the current thread
        rc = vm->GetEnv((void **)&env, JNI_VERSION_1_2);
        if (rc != JNI_OK)
        {
            // No JNIEnv; fail
            return JNI_ERR;
        }

        // Get the RAS Interface
        DgRasInterface* jvmriInterface;
        rc = vm->GetEnv((void **)&jvmriInterface, JVMRAS_VERSION_1_3);
        if (rc != JNI_OK)
        {
            // No RAS Interface available; fail
            return JNI_ERR;
        }

        getThreadDumperGlobals()->mDumper = new J9ThreadDumper(vm, jvmriInterface);

        if (options != 0)
        {
            LOG1(EUtils, EInfo, "Dump options = %s", options);
        }
        // Request creation of the agent thread
        rc = jvmriInterface->CreateThread(env, agentThread,0 , 0);
    }

    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaRuntime,"ERROR in Thread Dump. ExceptionBase: %s",ex.toString().c_str());
    }
    catch (std::exception& e)
    {

        ELOG1(EJavaRuntime,"ERROR in Thread Dump. std::exception: %s",e.what());
    }
    catch (...)
    {
        ELOG(EJavaRuntime,"ERROR in Thread Dump. Unexpected exception was caught");
    }
    return rc;
}

// Keep this in sync with typedef in threaddumper.h file
void doThreadDump()
{
    JELOG2(EJavaRuntime);
    J9ThreadDumper* instance = getThreadDumperGlobals()->mDumper;
    if (instance)
    {
        instance->doDump();
    }
#ifndef __WINSCW__
    else
    {
        delete sGlobals;
        sGlobals = 0;
    }
#endif
}

// Keep this in sync with typedef in threaddumper.h file
void closeThreadDump()
{
    JELOG2(EJavaRuntime);
    J9ThreadDumper* instance = getThreadDumperGlobals()->mDumper;
    if (instance)
    {
        instance->close();
    }
#ifndef __WINSCW__
    else
    {
        delete sGlobals;
        sGlobals = 0;
    }
#endif
}


#ifdef __SYMBIAN32__
EXPORT_C FuncPtr JNICALL jni_lookup(const char* aName)
{
    JELOG2(EJavaRuntime);
    if (!strcmp(aName, "JVM_OnLoad"))
    {
        return (FuncPtr)JVM_OnLoad;
    }

    else if (!strcmp(aName, "doThreadDump"))
    {
        return (FuncPtr)doThreadDump;
    }

    else if (!strcmp(aName, "closeThreadDump"))
    {
        return (FuncPtr)closeThreadDump;
    }

    return 0;
}
#endif // __SYMBIAN32__
