/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class has JNI wrappers for CMMARecordControl
*
*/


#include <logger.h>

#include <JniEnvWrapper.h>

#include "com_nokia_microedition_media_control_RecordControl.h"
#include "mmafunctionserver.h"
#include "cmmarecordcontrol.h"
#include "s60commonutils.h"
#include "jstringutils.h"
using namespace java::util;


void VVoidFuncL(CMMARecordControl* aControl, void(CMMARecordControl::*aFunc)())
{
    (aControl->*aFunc)();
}

void TIntFuncL(CMMARecordControl* aControl, TInt(CMMARecordControl::*aFunc)(TInt),
               TInt* aReturnValue)
{
    *aReturnValue = (aControl->*aFunc)(*aReturnValue);
}
/*
 * Class:     com_nokia_microedition_media_RecordControl
 * Method:    int _startRecord( int aEventSourceHandle, int aRecordControlHandle );
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_RecordControl__1startRecord
(JNIEnv *, jclass, jint aEventSourceHandle, jint aRecordControlHandle)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_RecordControl__1startRecord");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        reinterpret_cast< CMMARecordControl* >(aRecordControlHandle);

    return eventSource->ExecuteTrap(&VVoidFuncL,
                                    recordControl,
                                    &CMMARecordControl::StartRecordL);
}

/*
 * Class:     com_nokia_microedition_media_RecordControl
 * Method:    int _stopRecord( int aEventSourceHandle, int aRecordControlHandle );
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_RecordControl__1stopRecord
(JNIEnv *, jclass, jint aEventSourceHandle, jint aRecordControlHandle)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_RecordControl__1stopRecord");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    CMMARecordControl* recordControl =
        reinterpret_cast< CMMARecordControl* >(aRecordControlHandle);

    return eventSource->ExecuteTrap(&VVoidFuncL,
                                    recordControl,
                                    &CMMARecordControl::StopRecordL);
}

void CMMAOutputStreamFunc(void(CMMARecordControl::*aFunc)(CMMAOutputStream*),CMMARecordControl* aControl,
                          CMMAOutputStream* aOutputStream)
{
    (aControl->*aFunc)(aOutputStream);
}
/*
 * Class:     com_nokia_microedition_media_RecordControl
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_RecordControl__1setRecordStream
(JNIEnv *aJni, jclass, jint aEventSourceHandle, jint aRecordControlHandle, jobject aOutputWriter)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_RecordControl__1setRecordStream");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    CMMARecordControl* recordControl =
        reinterpret_cast< CMMARecordControl* >(aRecordControlHandle);

    jobject writerRef = aJni->NewWeakGlobalRef(aOutputWriter);
    CMMAOutputStream* outputStream;

    TInt err = eventSource->ExecuteTrap(CMMAOutputStream::CreateL,
                                        &outputStream,
                                        eventSource,
                                        aJni,
                                        (MMMAEventPoster*)eventSource,
                                        writerRef);

    if (err == KErrNone)
    {
        eventSource->ExecuteV(&CMMAOutputStreamFunc,
                              &CMMARecordControl::SetRecordStream,
                              recordControl,
                              outputStream);

        return reinterpret_cast<TInt>(outputStream);
    }
    else
    {
        aJni->DeleteWeakGlobalRef((jweak)writerRef);
        return err;
    }
}

void HBufCFunc(CMMARecordControl* aControl, HBufC*(CMMARecordControl::*aFunc)(),
               HBufC** aBuf)
{
    *aBuf = (aControl->*aFunc)();
}

/*
 * Class:     com_nokia_microedition_media_RecordControl
 * Method:    String _getContentType( int aEventSourceHandle, int aRecordControlHandle );
 * Signature: (II)I
 */
JNIEXPORT jstring JNICALL Java_com_nokia_microedition_media_control_RecordControl__1getContentType
(JNIEnv *aJniEnv, jclass, jint aEventSourceHandle, jint aRecordControlHandle)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_RecordControl__1getContentType");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer* >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        reinterpret_cast< CMMARecordControl* >(aRecordControlHandle);

    const HBufC* contentType = NULL;
    eventSource->ExecuteTrap(&HBufCFunc,
                             recordControl,
                             &CMMARecordControl::GetContentTypeL,
                             (HBufC**)&contentType);

    if (contentType != NULL)
    {
        return S60CommonUtils::NativeToJavaString(*aJniEnv, *contentType);
    }
    // ContentType is not known (Player not started?)
    return S60CommonUtils::NativeToJavaString(*aJniEnv, KNullDesC);
}

/*
 * Class:     com_nokia_microedition_media_RecordControl
 * Method:    int _commit( int aEventSourceHandle, int aRecordControlHandle );
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_RecordControl__1commit
(JNIEnv *, jclass, jint aEventSourceHandle, jint aRecordControlHandle)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_RecordControl__1commit");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    CMMARecordControl* recordControl =
        reinterpret_cast< CMMARecordControl* >(aRecordControlHandle);

    return eventSource->ExecuteTrap(&VVoidFuncL,
                                    recordControl,
                                    &CMMARecordControl::CommitL);
}

/*
 * Class:     com_nokia_microedition_media_RecordControl
 * Method:    int _setRecordSizeLimit( int aEventSourceHandle, int aRecordControlHandle, int aSize );
 * Signature: (III)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_RecordControl__1setRecordSizeLimit
(JNIEnv *, jclass, jint aEventSourceHandle, jint aRecordControlHandle, jint aSize)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_RecordControl__1setRecordSizeLimit");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    CMMARecordControl* recordControl =
        reinterpret_cast< CMMARecordControl *>(aRecordControlHandle);

    TInt err = eventSource->ExecuteTrap(&TIntFuncL,
                                        recordControl,
                                        &CMMARecordControl::SetRecordSizeLimitL,
                                        &aSize);
    if (err != KErrNone)
    {
        return err;
    }

    // return size of the set record size
    return aSize;
}

/*
 * Class:     com_nokia_microedition_media_RecordControl
 * Method:    int _reset( int aEventSourceHandle, int aRecordControlHandle );
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_RecordControl__1reset
(JNIEnv *, jclass, jint aEventSourceHandle, jint aRecordControlHandle)
{
    LOG(EJavaMMAPI, EInfo, "Java_com_nokia_microedition_media_control_RecordControl__1reset");

    MMAFunctionServer* eventSource =
        reinterpret_cast< MMAFunctionServer *>(aEventSourceHandle);

    CMMARecordControl* recordControl =
        reinterpret_cast< CMMARecordControl *>(aRecordControlHandle);

    return eventSource->ExecuteTrap(&VVoidFuncL,
                                    recordControl,
                                    &CMMARecordControl::ResetL);
}

//  END OF FILE
