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


#include <jdebug.h>
#include <JniEnvWrapper.h>

#include "com_nokia_microedition_media_control_RecordControl.h"
#include "cmmaeventsource.h"
#include "cmmarecordcontrol.h"


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
    DEBUG("Java_com_nokia_microedition_media_control_RecordControl__1startRecord");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        JavaUnhand< CMMARecordControl >(aRecordControlHandle);

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
    DEBUG("Java_com_nokia_microedition_media_control_RecordControl__1stopRecord");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        JavaUnhand< CMMARecordControl >(aRecordControlHandle);

    return eventSource->ExecuteTrap(&VVoidFuncL,
                                    recordControl,
                                    &CMMARecordControl::StopRecordL);
}

void CMMAOutputStreamFunc(CMMARecordControl* aControl, void(CMMARecordControl::*aFunc)(CMMAOutputStream*),
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
    DEBUG("Java_com_nokia_microedition_media_control_RecordControl__1setRecordStream");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        JavaUnhand< CMMARecordControl >(aRecordControlHandle);

    jobject writerRef = aJni->NewWeakGlobalRef(aOutputWriter);
    CMMAOutputStream* outputStream;

    TInt err = eventSource->ExecuteTrap(CMMAOutputStream::CreateL,
                                        &outputStream,
                                        aJni,
                                        (MMMAEventPoster*)eventSource,
                                        writerRef);

    if (err == KErrNone)
    {
        eventSource->ExecuteV(&CMMAOutputStreamFunc,
                              recordControl,
                              &CMMARecordControl::SetRecordStream,
                              outputStream);

        return JavaMakeHandle(outputStream);
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
    DEBUG("Java_com_nokia_microedition_media_control_RecordControl__1getContentType");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        JavaUnhand< CMMARecordControl >(aRecordControlHandle);

    const HBufC* contentType = NULL;
    eventSource->ExecuteTrap(&HBufCFunc,
                             recordControl,
                             &CMMARecordControl::GetContentTypeL,
                             (HBufC**)&contentType);

    if (contentType != NULL)
    {
        return CreateJavaString(*aJniEnv, *contentType);
    }
    // ContentType is not known (Player not started?)
    return CreateJavaString(*aJniEnv, KNullDesC);
}

/*
 * Class:     com_nokia_microedition_media_RecordControl
 * Method:    int _commit( int aEventSourceHandle, int aRecordControlHandle );
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_microedition_media_control_RecordControl__1commit
(JNIEnv *, jclass, jint aEventSourceHandle, jint aRecordControlHandle)
{
    DEBUG("Java_com_nokia_microedition_media_control_RecordControl__1commit");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        JavaUnhand< CMMARecordControl >(aRecordControlHandle);

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
    DEBUG("Java_com_nokia_microedition_media_control_RecordControl__1setRecordSizeLimit");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        JavaUnhand< CMMARecordControl >(aRecordControlHandle);

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
    DEBUG("Java_com_nokia_microedition_media_control_RecordControl__1reset");

    CMMAEventSource* eventSource =
        JavaUnhand< CMMAEventSource >(aEventSourceHandle);

    CMMARecordControl* recordControl =
        JavaUnhand< CMMARecordControl >(aRecordControlHandle);

    return eventSource->ExecuteTrap(&VVoidFuncL,
                                    recordControl,
                                    &CMMARecordControl::ResetL);
}

//  END OF FILE
