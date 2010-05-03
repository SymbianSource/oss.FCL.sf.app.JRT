/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JNI context for SoftNotificationImpl Java class.
 *
*/


//  INCLUDE FILES
#include <jutils.h>
#include <JniEnvWrapper.h>
#include <jdebug.h>
#include "com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl.h"
#include "CSoftNotification.h"
#include "CSoftNotificationEventSource.h"

/*
 * Class: com_nokia_mid_ui_SoftNotification
 * Method: _createEventSource
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1createEventSource(
    JNIEnv* aJniEnv,
    jobject aPeer,
    jint aEventServerHandle)
{
    DEBUG("SoftNotification::_createEventSource +");

    TInt eventSourceHandle = CSoftNotificationEventSource::New(
                                 *aJniEnv,
                                 aPeer,
                                 aEventServerHandle);

    DEBUG_INT("SoftNotification::_createEventSource -, handle=%d",
              eventSourceHandle);

    return eventSourceHandle;
}

/*
 * Wrapper for CSoftNotification::NewL().
 */
LOCAL_C void CreateSoftNotificationL(
    TInt* aSoftNotificationHandle,
    CSoftNotificationEventSource* aEventSource,
    jobject aPeer,
    TInt aMidletId,
    TInt aNotificationId)
{
    DEBUG("SoftNotification::CreateSoftNotificationL +");

    JNIEnv* jniEnv = JniEnvWrapper::GetValidJniRef();

    // Get method id for the notify function.
    jclass classRef = jniEnv->GetObjectClass(aPeer);
    jmethodID methodId = jniEnv->GetMethodID(
                             classRef, "notificationCallback", "(I)V");

    // Handle possible exception occurred during the previous operation.
    if (jniEnv->ExceptionCheck())
    {
        // Print the stack trace and clear the pending exception from
        // the Java VM.
        jniEnv->ExceptionDescribe();
        jniEnv->ExceptionClear();
        User::Leave(KErrGeneral);
    }

    // create a soft notification instance
    CSoftNotification* softNote = CSoftNotification::NewL(
                                      aMidletId,
                                      aNotificationId,
                                      *aEventSource);

    // The weak reference is stored in the created soft notification object.
    // The reference will be deleted when the object is destroyed.
    softNote->SetPeerObserver(aPeer, methodId);

    // Get handle to the created soft notification.
    *aSoftNotificationHandle = JavaMakeHandle(softNote);

    DEBUG("SoftNotification::CreateSoftNotificationL -");
}

/*
 * Class: com_nokia_mid_ui_SoftNotification
 * Method: _createNativePeer
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1createNativePeer(
    JNIEnv* aJniEnv,
    jobject aPeer,
    jint aEventSourceHandle,
    jint aMIDletID,
    jint aNotificationId)
{
    DEBUG("SoftNotification::_createNativePeer +");

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    // Create a weak reference since the object is transferred between
    // two threads. The reference is deleted when the native class is destroyed.
    jobject peer = aJniEnv->NewWeakGlobalRef(aPeer);

    // Handle possible exception occurred during the previous operation.
    if (aJniEnv->ExceptionCheck())
    {
        // Print the stack trace and clear the pending exception from
        // the Java VM.
        aJniEnv->ExceptionDescribe();
        aJniEnv->ExceptionClear();
        return KErrNoMemory;
    }

    TInt softNotificationHandle = 0;

    TInt error = eventSource->ExecuteTrap(
                     &CreateSoftNotificationL,
                     &softNotificationHandle,
                     eventSource,
                     peer,
                     aMIDletID,
                     aNotificationId);

    // Delete the created global reference if object creation failed.
    if (error != KErrNone)
    {
        softNotificationHandle = error;
        aJniEnv->DeleteWeakGlobalRef((jweak)peer);
    }

    DEBUG_INT("SoftNotification::_createNativePeer -, handle=%d",
              softNotificationHandle);

    return softNotificationHandle;
}

/*
 * Destroys the given soft notification and global reference that it owns.
 */
LOCAL_C void DestroyNotification(CSoftNotification* aNotification)
{
    DEBUG("SoftNotification::DestroyNotification +");

    JNIEnv* jniEnv = JniEnvWrapper::GetValidJniRef();

    // Delete the weak reference stored in the class.
    aNotification->RemovePeerObserver(*jniEnv);

    delete aNotification;

    DEBUG("SoftNotification::DestroyNotification -");
}

/*
 * Class: com_nokia_mid_ui_SoftNotification
 * Method: _destroy
 */
JNIEXPORT void JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1destroy(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aSoftNotificationHandle)
{
    DEBUG_INT("SoftNotification::_destroy +, handle=%d",
              aSoftNotificationHandle);

    // There are no native resources if event server is not yet created or
    // is already destroyed.
    if (aEventSourceHandle <= KErrNone)
    {
        DEBUG("SoftNotification::_destroy -, nothing to do");
        return;
    }

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    // Destroy the soft notification. The destruction has to be done in the
    // event server thread because all used resources are not shared between
    // different threads.
    if (aSoftNotificationHandle > KErrNone)
    {
        CSoftNotification* notification =
            JavaUnhand< CSoftNotification >(aSoftNotificationHandle);

        eventSource->Execute(&DestroyNotification, notification);
    }

    // Destroy the event source.
    eventSource->Dispose(*aJniEnv);

    DEBUG("SoftNotification::_destroy -");
}

/*
 * wrapper for CSoftNotification::ShowSoftNotificationL()
 */
LOCAL_C void ShowSoftNotificationL(
    CSoftNotification* aSoftNotification)
{
    aSoftNotification->ShowSoftNotificationL();
}

/*
 * Class: com_nokia_mid_ui_SoftNotification
 * Method: _showSoftNotification
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1showSoftNotification(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aSoftNotificationHandle)
{
    DEBUG("SoftNotification::_showSoftNotification +");

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    CSoftNotification* softNotification =
        JavaUnhand< CSoftNotification >(aSoftNotificationHandle);


    TInt error = eventSource->ExecuteTrap(
                     &ShowSoftNotificationL,
                     softNotification);

    DEBUG("SoftNotification::_showSoftNotification -");

    return error;
}

/*
 * wrapper for CSoftNotification::RemoveSoftNotificationL()
 */
LOCAL_C void RemoveSoftNotificationL(
    CSoftNotification* aSoftNotification)
{
    aSoftNotification->RemoveSoftNotificationL();
}

/*
 * Class: com_nokia_mid_ui_SoftNotification
 * Method: _removeSoftNotification
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1removeSoftNotification(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aSoftNotificationHandle)
{
    DEBUG("SoftNotification::_removeSoftNotification +");

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    CSoftNotification* softNotification =
        JavaUnhand< CSoftNotification >(aSoftNotificationHandle);


    TInt error = eventSource->ExecuteTrap(
                     &RemoveSoftNotificationL,
                     softNotification);

    DEBUG("SoftNotification::_removeSoftNotification -");

    return error;
}

/*
 * wrapper for CSoftNotification::SetTextL()
 */
LOCAL_C void SetTextL(
    CSoftNotification* aSoftNotification,
    TDesC* aText,
    TDesC* aGroupText)
{
    aSoftNotification->SetTextL(*aText, *aGroupText);
}

/*
 * Class:     com_nokia_mid_ui_SoftNotificationImpl
 * Method:    _setText
 */
JNIEXPORT int JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1setText(
    JNIEnv* aJni,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aSoftNotificationHandle,
    jstring aText,
    jstring aGroupText)
{
    DEBUG("SoftNotification::_setText +");

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    CSoftNotification* softNotification =
        JavaUnhand< CSoftNotification >(aSoftNotificationHandle);


    RJString text(*aJni, aText);
    RJString grouptext(*aJni, aGroupText);

    TInt error = eventSource->ExecuteTrap(
                     &SetTextL,
                     softNotification,
                     (TDesC*)&text,
                     (TDesC*)&grouptext);

    DEBUG("SoftNotification::_setText -");

    return error;
}

/*
 * wrapper for CSoftNotification::SetSoftkeyLabels()
 */
LOCAL_C void SetSoftkeyLabelsL(
    CSoftNotification* aSoftNotification,
    TDesC* aSoftkey1Label,
    TDesC* aSoftkey2Label)
{
    aSoftNotification->SetSoftkeyLabelsL(*aSoftkey1Label, *aSoftkey2Label);
}

/*
 * Class:     com_nokia_mid_ui_SoftNotificationImpl
 * Method:    _setSoftkeyLabels
 */
JNIEXPORT int JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1setSoftkeyLabels(
    JNIEnv* aJni,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aSoftNotificationHandle,
    jstring aSoftkey1Label,
    jstring aSoftkey2Label)
{
    DEBUG("SoftNotification::_setSoftkeyLabels +");

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    CSoftNotification* softNotification =
        JavaUnhand< CSoftNotification >(aSoftNotificationHandle);


    RJString softkey1Label(*aJni, aSoftkey1Label);
    RJString softkey2Label(*aJni, aSoftkey2Label);

    TInt error = eventSource->ExecuteTrap(
                     &SetSoftkeyLabelsL,
                     softNotification,
                     (TDesC*)&softkey1Label,
                     (TDesC*)&softkey2Label);

    DEBUG("SoftNotification::_setSoftkeyLabels -");

    return error;
}

/*
 * wrapper for CSoftNotification::SetImage()
 */
LOCAL_C void SetImageL(
    CSoftNotification* aSoftNotification,
    const TDesC8* aImageData)
{
    aSoftNotification->SetImageL(*aImageData);
}

/*
 * Class:     com_nokia_mid_ui_SoftNotificationImpl
 * Method:    _setImage
 */
JNIEXPORT int JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1setImage(
    JNIEnv* aJni,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aSoftNotificationHandle,
    jbyteArray aImageData)
{
    DEBUG("SoftNotification::_setImage +");

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    CSoftNotification* softNotification =
        JavaUnhand< CSoftNotification >(aSoftNotificationHandle);


    // Returning just KErrNone if there is no image data
    TInt error = KErrNone;

    if (aImageData)
    {
        // Image data: Get pointer to Java header data
        jbyte* imageDataJava = aJni->GetByteArrayElements(aImageData, NULL);

        // Handle possible exception occurred during the previous operation.
        if (aJni->ExceptionCheck())
        {
            // Print the stack trace and clear the pending exception from
            // the Java VM.
            aJni->ExceptionDescribe();
            aJni->ExceptionClear();
            return KErrNoMemory;
        }

        TInt imageDataLength = aJni->GetArrayLength(aImageData);
        TPtrC8 imageData((TUint8*)imageDataJava, imageDataLength);

        error = eventSource->ExecuteTrap(
                    &SetImageL,
                    softNotification,
                    (const TDesC8*)&imageData);

        // Release image bytes got with GetByteArrayElements.
        // The function does not throw any exceptions.
        aJni->ReleaseByteArrayElements(aImageData,
                                       imageDataJava,
                                       0);
    }
    else
    {
        // No image data defined, reset previous image.
        TPtrC8 imageData(KNullDesC8);
        error = eventSource->ExecuteTrap(
                    &SetImageL,
                    softNotification,
                    (const TDesC8*)&imageData);
    }

    DEBUG("SoftNotification::_setImage -");

    return error;
}

/*
 * wrapper for CSoftNotification::GetId()
 */
LOCAL_C TInt GetId(CSoftNotification* aSoftNotification)
{
    return aSoftNotification->Id();
}

/*
 * Class:     com_nokia_mid_ui_SoftNotificationImpl
 * Method:    _getId
 */
JNIEXPORT jint JNICALL
Java_com_nokia_mj_impl_dynamicsoftnotification_SoftNotificationImpl__1getId(
    JNIEnv* /*aJniEnv*/,
    jobject /*aPeer*/,
    jint aEventSourceHandle,
    jint aSoftNotificationHandle)
{
    DEBUG("SoftNotification::_getId +");

    CSoftNotificationEventSource* eventSource =
        JavaUnhand< CSoftNotificationEventSource >(aEventSourceHandle);

    CSoftNotification* softNotification =
        JavaUnhand< CSoftNotification >(aSoftNotificationHandle);

    TInt id = eventSource->Execute(&GetId, softNotification);

    DEBUG("SoftNotification::_getId -");

    return id;
}

//  End of File
