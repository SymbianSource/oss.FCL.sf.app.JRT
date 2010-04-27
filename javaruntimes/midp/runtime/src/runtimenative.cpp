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
* Description:  Native methods of the MIDP runtime.
 *
*/


#include <iostream>
#include <fstream>

#include "exceptionbase.h"
#include "javacommonutils.h"
#include "javainifileutils.h"
#include "javajniutils.h"
#include "javaoslayer.h"
#include "logger.h"
#include "javauid.h"

#include "platformimpl.h"

#include "midpruntimeinternalsupport.h"

#include "commsclientendpoint.h"
#include "commsmessage.h"

#include "midpauthenticationmodule.h"

#include "platformrequestinterface.h"

#include "com_nokia_mj_impl_rt_main_Main.h"
#include "com_nokia_mj_impl_rt_midp_DrmUtil.h"
#include "com_nokia_mj_impl_rt_midp_MidletLifeCycle.h"
#include "com_nokia_mj_impl_rt_midp_MemoryLogger.h"
#include "com_nokia_mj_impl_rt_midp_RuntimeErrorDialog.h"
#include "javax_microedition_midlet_MIDlet.h"

/**
 * A collection of MIDP runtime native methods.
 */

using namespace java::util;
using namespace java::runtime;


/**
 * A method for informing native starter about the UID of the MIDlet. This
 * is used when UID is received in Java part in pre warm start. The native
 * starter needs the UID in order to be able to serve AppilcationInfo
 * native interface.
 * @param jmuid UID of the MIDlet as String
 * @param jmsuid UID of the MIDlet suite as String
 * @param handle A pointer to native starter.
 */
JNIEXPORT
void JNICALL Java_com_nokia_mj_impl_rt_midp_MidletLifeCycle__1setUids
(JNIEnv* env, jobject /*obj*/, jstring jmuid, jstring jmsuid, jint handle)
{
    JELOG2(EJavaRuntime);

    // Send the UID to native.
    Uid midletUid(JniUtils::jstringToWstring(env, jmuid));
    Uid midletSuiteUid(JniUtils::jstringToWstring(env, jmsuid));

    try
    {
        MidpStarterInternalSupport* starter =
            reinterpret_cast<MidpStarterInternalSupport*>(handle);

        starter->setUids(midletUid, midletSuiteUid);
    }

    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaRuntime,"ERROR in setting UID. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {

        ELOG1(EJavaRuntime,"ERROR in setting UID. std::exception: %s",
              e.what());
    }
}

/**
 * A method for informing native starter that a application* is about to close.
 * @param handle A pointer to native starter.
 */
void closeIndImpl(jint handle)
{
    JELOG2(EJavaRuntime);
//    throw -1;
    try
    {
        MidpStarterInternalSupport* starter =
            reinterpret_cast<MidpStarterInternalSupport*>(handle);

        starter->closeRuntimeInd();
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaRuntime,"ERROR in close ind. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {

        ELOG1(EJavaRuntime,"ERROR in close ind. std::exception: %s",
              e.what());
    }
}

/**
 * A method for informing native starter that a application* is about to close.
 * @param handle A pointer to native starter.
 */
JNIEXPORT
void JNICALL Java_com_nokia_mj_impl_rt_main_Main__1closeInd
(JNIEnv* /*env*/, jclass /*peer*/, jint handle)
{
    JELOG2(EJavaRuntime);
    closeIndImpl(handle);
}

/**
 * A method for informing native starter that a MIDlet is entered into
 * destroyed state.
 * @param handle A pointer to native starter.
 */
JNIEXPORT
void JNICALL Java_com_nokia_mj_impl_rt_midp_MidletLifeCycle__1closeInd
(JNIEnv* /*env*/, jclass /*peer*/, jint handle)
{
    JELOG2(EJavaRuntime);
    closeIndImpl(handle);
}

/**
 * Consume the rigth objects of the MIDlet if needed.
 * @param uri Uri to the platform request handler.
 * @param drmContentId Content id of the file.
 * @param startPhase true if starting the MIDlet, false on closing.
 * @param handle A handle to platform dependent object. Must be 0 on MIDlet
 *        start and valid pointer on MIDlet stop. On success start case
 *        the valid handle is reurned into Java side.
 * @return Integer object containing valid pointer to native object and
 *         String containing error string in failure cases.
 */
JNIEXPORT
jobject JNICALL Java_com_nokia_mj_impl_rt_midp_DrmUtil__1consumeRights
(JNIEnv* env, jclass, jstring uri, jstring drmContentId, jboolean startPhase,
 jint handle)
{
    JELOG2(EJavaRuntime);

    std::string status;
    try
    {
        consumeRigthsImpl(JniUtils::jstringToWstring(env, uri),
                          JniUtils::jstringToWstring(env, drmContentId),
                          status, startPhase, handle);
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaRuntime, "ERROR in PlatformRequest: ExceptionBase: %s",
              ex.toString().c_str());
        status = "Internal error";
    }
    catch (std::exception& e)
    {
        status = "Internal Error";
        ELOG1(EJavaRuntime,"ERROR in PlatformRequest. std::exception: %s",
              e.what());
    }
    if (status.length() == 0)
    {
        jclass integerClass = env->FindClass("java/lang/Integer");
        if (integerClass != 0)
        {
            jmethodID constructor =
                env->GetMethodID(integerClass, "<init>", "(I)V");
            if (integerClass != 0)
            {
                return env->NewObject(integerClass, constructor, handle);
            }
            else
            {
                status = "J2ME internal error (DRM 1)";
            }
        }
        else
        {
            status = "J2ME internal error (DRM 2)";
        }
        ELOG(EJavaRuntime, status.c_str());
    }
    return env->NewStringUTF(status.c_str());
}

/**
 * A method for informing native starter about the UID of the MIDlet. This
 * is used when UID is received in Java part in pre warm start. The native
 * starter needs the UID in order to be able to serve AppilcationInfo
 * native interface.
 * @param juid UID of the MIDlet as String
 * @param handle A pointer to native starter.
 */
JNIEXPORT
void JNICALL Java_com_nokia_mj_impl_rt_midp_MidletLifeCycle__1restoreNormalProcessPriority
(JNIEnv* /*env*/, jobject /*obj*/)
{
    JELOG2(EJavaRuntime);
#ifdef __SYMBIAN32__
    RProcess proc;
    proc.SetPriority(EPriorityForeground);
#endif // __SYMBIAN32__
}
/**
 * Do the platform request.
 * @param uri Uri to the platform request handler.
 */
JNIEXPORT
void JNICALL Java_javax_microedition_midlet_MIDlet__1managePlatformRequest
(JNIEnv* env, jobject, jstring uri)
{
    JELOG2(EJavaRuntime);

    try
    {
        std::auto_ptr<PlatformRequestInterface>
        handler(getPlatformRequestHandlerObj());
        if (handler.get())
        {
            handler->handleUri(JniUtils::jstringToWstring(env,uri));
        }
        else
        {
            JniUtils::throwNewException(env,
                                        "javax/microedition/io/ConnectionNotFoundException",
                                        "URL parsing failed");
        }
    }
    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaRuntime, "ERROR in PlatformRequest: ExceptionBase: %s",
              ex.toString().c_str());
        if (PlatformRequestInterface::CONNECTION_NOT_SUPPORTED == ex.mErrCode)
        {
            JniUtils::throwNewException(env,
                                        "javax/microedition/io/ConnectionNotFoundException",
                                        "Unsupported scheme");
        }
        else
        {
            JniUtils::throwNewException(env,
                                        "javax/microedition/io/ConnectionNotFoundException",
                                        ex.toString());
        }
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime,"ERROR in PlatformRequest. std::exception: %s",
              e.what());
    }
}

/**
 * A utility for writing the heap size into a file.
 * @param file A file to be written in.
 * @param heapSize The heap size in bytes.
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_rt_midp_MemoryLogger__1writeFile(
    JNIEnv* jniEnv, jobject, jstring file, jint heapSize)
{
    JELOG2(EJavaRuntime);
    try
    {
        jboolean iscopy;
        const char* fileName = jniEnv->GetStringUTFChars(file, &iscopy);
        LOG2(EJavaRuntime, EInfo, "Writing heap size %d to file %s.", heapSize, fileName);

        std::ofstream heapFile;
        heapFile.open(fileName);
        heapFile << heapSize;
        heapFile.close();
        jniEnv->ReleaseStringUTFChars(file, fileName);

    }

    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaRuntime,"ERROR in write file. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime,"ERROR in write file. std::exception: %s",
              e.what());
    }
}

/**
 * A utility for reading the heap size from a file.
 * @param file A file to be read from.
 * @retrun heapSize The heap size in bytes.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_rt_midp_MemoryLogger__1readFile(
    JNIEnv* jniEnv, jclass, jstring file)
{
    JELOG2(EJavaRuntime);
    jint heapSize = 0;
    try
    {
        jboolean iscopy;
        const char* fileName = jniEnv->GetStringUTFChars(file, &iscopy);

        std::ifstream heapFile;
        heapFile.open(fileName, std::ifstream::in);
        heapFile >> heapSize;
        heapFile.close();
        jniEnv->ReleaseStringUTFChars(file, fileName);
        LOG2(EJavaRuntime, EInfo, "Heap size %d read from file %s.", heapSize, fileName);
    }

    catch (ExceptionBase& ex)
    {
        ELOG1(EJavaRuntime,"ERROR in read file. ExceptionBase: %s",
              ex.toString().c_str());
    }
    catch (std::exception& e)
    {
        ELOG1(EJavaRuntime,"ERROR in read file. std::exception: %s",
              e.what());
    }
    return heapSize;
}

/**
 * A  utility for getting the stack trace for further processing.
 * destroyed state.
 * @param handle A pointer to native starter.
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_rt_midp_RuntimeErrorDialog__1getStackTrace
(JNIEnv* jniEnv, jclass, jthrowable th, jobject printStream)
{
    /*
     * call Throwable.printStackTrace(java.io.PrintStream)
     * this method is not part of CLDC spec, but it's supported by VM vendors
     */
    jclass classThrowable = jniEnv->GetObjectClass(th);
    jmethodID methodId = jniEnv->GetMethodID(classThrowable, "printStackTrace",
                         "(Ljava/io/PrintStream;)V");
    jniEnv->CallVoidMethod(th, methodId, printStream);
}

