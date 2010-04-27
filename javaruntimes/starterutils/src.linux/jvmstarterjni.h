/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is meant for starting the J9 JVM.
*
*/


#ifndef JVMSTARTERJNI_H
#define JVMSTARTERJNI_H

#include <vector>
#include <jni.h>

#include "jvmstarterimpl.h"

typedef std::vector<JavaVMOption> JvmOptionArgs_t;

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

/**
 * Provides a utilities for starting the Sun JVM in Linux.
 */
class JvmStarterJni : public JvmStarterImpl
{
public:
    /**
     * Default constructor of the JvmStarterJni.
     */
    JvmStarterJni();

    /**
     * Constructor of the JvmStarterJni with arguments. For the argument
     * description @see jvmstarte.h#getJvmStarterInstance method with
     * same arguments
     */
    JvmStarterJni(const Configuration configuration,
                  const std::wstring& indetifier);

    /**
     * Destructor of the JvmStarterJni.
     */
    virtual ~JvmStarterJni();

    /**
     * @see jvmstarter.h
     */
    virtual int startJvm();
    /**
     * @see jvmstarter.h
     */
    virtual int startJvm(int argc, char** argv);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideOldHeapSize(int heapSize);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideNewHeapSize(int heapSize);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideNativeStackSize(int stackSize);

    /**
     * @see jvmstarter.h
     */
    virtual void overrideJavaStackSize(int stackSize);

private:
    /**
     * Sets the default arguments optimized for the MIDP runtime
     */
    void setDefaultArguments();

    /**
     * Adds the classpaths, main class, JIT configurations and thread
     * configurations to be part of the internal lists mJvmArgs and
     * mAppAndArgs.
     */
    void completeArgumentContainers();

    /**
     * Creates a jobjectArray containing application arguments as
     * Java Strings.
     * @param env A pointer to valid JNIEnv.
     * @return jobjectArray containing application arguments as
     *         Java Strings.
     */
    jobjectArray getApplicationArguments(JNIEnv* env);

    /**
     * Starts finally the JVM. This method assumes that all the JVM
     * arguments are passed as arguments, but the the list must not
     * contain the main class nor the arguments for the Java application.
     * The method assumes that these can be found from member variable
     * mAppAndArgs. This method assumes that arguments for the JVM will
     * be set to memeber variables mArgCount and mArgs. The method should
     * not be run in the primordial thread.
     * @return status, 0 in success case.
     */
    int startJvmImpl();

    /**
     * Creates a new thread for starting the JVM.
     * @argc Number of JVM arguments.
     * @argv JVM arguments in UTF-8 format.
     * @return status, 0 in success case.
     */
    int startJvmInSeparateThread(int argc, char** argv);



    /**
     * Sun's JVM requires that the JVM is started in non-primordial thread
     * (http://blogs.sun.com/ksrini/entry/hotspot_primordial_thread_jni_stack).
     * This is an entry point of the new thread where the JVM is started.
     * @param arg A pointer instance of this class.
     * @return status of the call.
     */
    static void* javaThreadMain(void* arg);

private:

    /** Number of JVM arguments. */
    int          mArgCount;

    /** JVM arguments in UTF-8 format. Not owned. */
    char**       mArgs;
};
} // end namespace runtime
} // end namespace java

#endif // JVMSTARTERJNI_H

