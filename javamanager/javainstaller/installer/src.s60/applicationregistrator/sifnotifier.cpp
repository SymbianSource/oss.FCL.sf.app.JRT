/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: The JNI code for Java Installer component
*              SifNotifier.
*
*/

#include "com_nokia_mj_impl_installer_applicationregistrator_SifNotifier.h"
#include "javacommonutils.h"
#include "javasymbianoslayer.h" // for CleanupResetAndDestroyPushL
#include "logger.h"

#if defined(SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK) && defined(RD_JAVA_USIF_NOTIFY_PROGRESS)

#include <usif/sifnotification.h>
#include <usif/usifcommon.h>

// Helper macro for logging a TDesC.
#define LOG_TDESC_L(compIdParam, logLevelParam, msgParam, tdescParam) \
    {                                                               \
        HBufC8* tdescBuf = HBufC8::NewLC(tdescParam.Length() + 1);  \
        TPtr8 tdescPtr(tdescBuf->Des());                            \
        tdescPtr.Append(tdescParam);                                \
        LOG1(compIdParam, logLevelParam, msgParam, tdescPtr.PtrZ());\
        CleanupStack::PopAndDestroy(tdescBuf);                      \
    }

// NAMESPACE DECLARATION
using namespace java;
using namespace Usif;

IMPORT_C HBufC* CreateHBufCFromJavaStringLC(JNIEnv* aEnv, jstring aString);


/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _sifNotifierEnabled
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1sifNotifierEnabled
(JNIEnv *, jclass)
{
    return ETrue;
}

/**
 * See JNI method __1notifyStart.
 * This method makes calls that may leave (the actual notifying).
 */
void NotifyStartL(
    JNIEnv *aEnv, CPublishSifOperationInfo *aNotifier,
    jstring aGlobalComponentId, jstring aComponentName,
    jobjectArray aApplicationNames, jint aComponentSize,
    jstring aComponentIconPath)
{
    __UHEAP_MARK;
    HBufC *globalComponentId = CreateHBufCFromJavaStringLC(aEnv, aGlobalComponentId);
    HBufC *componentName = CreateHBufCFromJavaStringLC(aEnv, aComponentName);
    HBufC *componentIconPath = CreateHBufCFromJavaStringLC(aEnv, aComponentIconPath);

    RPointerArray<HBufC> applicationNames;
    CleanupResetAndDestroyPushL(applicationNames);
    RPointerArray<HBufC> applicationIcons;
    CleanupResetAndDestroyPushL(applicationIcons);

    TInt appsCount = aEnv->GetArrayLength(aApplicationNames);
    for (TInt i = 0; i < appsCount; i++)
    {
        HBufC *appName = CreateHBufCFromJavaStringLC(
                             aEnv, (jstring)aEnv->GetObjectArrayElement(aApplicationNames, i));
        applicationNames.AppendL(appName);
        CleanupStack::Pop(appName);
    }

    CSifOperationStartData *startData = CSifOperationStartData::NewLC(
                                            *globalComponentId, *componentName, applicationNames, applicationIcons,
                                            aComponentSize, /*aIconPath=*/ *componentIconPath,
                                            /*aComponentIcon=*/ KNullDesC(), Usif::KSoftwareTypeJava);

    User::LeaveIfError(aNotifier->PublishStart(*startData));

    CleanupStack::PopAndDestroy(startData);

    CleanupStack::PopAndDestroy(&applicationIcons);
    CleanupStack::PopAndDestroy(&applicationNames);

    CleanupStack::PopAndDestroy(componentIconPath);
    CleanupStack::PopAndDestroy(componentName);
    CleanupStack::PopAndDestroy(globalComponentId);
    __UHEAP_MARKEND;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _notifyStart
 * Signature: (IILjava/lang/String;Ljava/lang/String;[Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1notifyStart
(JNIEnv *aEnv, jclass, jint aHandle, jstring aGlobalComponentId,
 jstring aComponentName, jobjectArray aApplicationNames, jint aComponentSize,
 jstring aComponentIconPath)
{
    CPublishSifOperationInfo *pNotifier =
        reinterpret_cast<CPublishSifOperationInfo*>(aHandle<<2);
    TRAPD(err, NotifyStartL(aEnv, pNotifier, aGlobalComponentId, aComponentName,
                            aApplicationNames, aComponentSize,
                            aComponentIconPath));
    return err;
}

/**
 * See JNI method __1notifyEnd.
 * This method makes calls that may leave (the actual notifying).
 */
void NotifyEndL(
    JNIEnv *aEnv, CPublishSifOperationInfo *aNotifier,
    jstring aGlobalComponentId, jint aErrCategory, jint aErrCode,
    jstring aErrMsg, jstring aErrMsgDetails)
{
    __UHEAP_MARK;
    HBufC *globalComponentId = CreateHBufCFromJavaStringLC(aEnv, aGlobalComponentId);
    HBufC *errMsg = NULL;
    if (NULL != aErrMsg)
    {
        errMsg = CreateHBufCFromJavaStringLC(aEnv, aErrMsg);
    }
    HBufC *errMsgDetails = NULL;
    if (NULL != aErrMsgDetails)
    {
        errMsgDetails = CreateHBufCFromJavaStringLC(aEnv, aErrMsgDetails);
    }

    CSifOperationEndData *endData = CSifOperationEndData::NewLC(
                                        *globalComponentId, (TErrorCategory)aErrCategory, aErrCode,
                                        *errMsg, *errMsgDetails);

    User::LeaveIfError(aNotifier->PublishCompletion(*endData));

    CleanupStack::PopAndDestroy(endData);

    if (NULL != aErrMsg)
    {
        CleanupStack::PopAndDestroy(errMsg);
    }
    if (NULL != aErrMsgDetails)
    {
        CleanupStack::PopAndDestroy(errMsgDetails);
    }
    CleanupStack::PopAndDestroy(globalComponentId);
    __UHEAP_MARKEND;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _notifyEnd
 * Signature: (ILjava/lang/String;IILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1notifyEnd
(JNIEnv *aEnv, jclass, jint aHandle, jstring aGlobalComponentId,
 jint aErrCategory, jint aErrCode, jstring aErrMsg, jstring aErrMsgDetails)
{
    CPublishSifOperationInfo *pNotifier =
        reinterpret_cast<CPublishSifOperationInfo*>(aHandle<<2);
    TRAPD(err, NotifyEndL(aEnv, pNotifier, aGlobalComponentId,
                          aErrCategory, aErrCode, aErrMsg, aErrMsgDetails));
    return err;
}

/**
 * See JNI method __1notifyProgress.
 * This method makes calls that may leave (the actual notifying).
 */
void NotifyProgressL(
    JNIEnv *aEnv, CPublishSifOperationInfo *aNotifier,
    jstring aGlobalComponentId, jint aOperation, jint aSubOperation,
    jint aCurrent, jint aTotal)
{
    __UHEAP_MARK;
    HBufC *globalComponentId = CreateHBufCFromJavaStringLC(aEnv, aGlobalComponentId);
    CSifOperationProgressData *progressData = CSifOperationProgressData::NewLC(
                *globalComponentId, (TSifOperationPhase)aOperation,
                (TSifOperationSubPhase)aSubOperation, aCurrent, aTotal);

    User::LeaveIfError(aNotifier->PublishProgress(*progressData));

    CleanupStack::PopAndDestroy(progressData);
    CleanupStack::PopAndDestroy(globalComponentId);
    __UHEAP_MARKEND;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _notifyProgress
 * Signature: (ILjava/lang/String;IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1notifyProgress
(JNIEnv *aEnv, jclass, jint aHandle, jstring aGlobalComponentId,
 jint aOperation, jint aSubOperation, jint aCurrent, jint aTotal)
{
    CPublishSifOperationInfo *pNotifier =
        reinterpret_cast<CPublishSifOperationInfo*>(aHandle<<2);
    TRAPD(err, NotifyProgressL(aEnv, pNotifier, aGlobalComponentId,
                               aOperation, aSubOperation, aCurrent, aTotal));
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1init
(JNIEnv *, jclass)
{
    CPublishSifOperationInfo *pNotifier = NULL;
    TRAPD(err, pNotifier = CPublishSifOperationInfo::NewL());
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "SifNotifier.init: Creating CPublishSifOperationInfo failed, error %d",
              err);
        return err;
    }
    // Return handle to the object. Utilize the fact that in Symbian
    // all pointer addresses are MOD 4 so the last 2 bits are 0
    // and can be shifted out. This way the returned handle is
    // always positive whereas Symbian error codes are always negative.
    return reinterpret_cast<TUint>(pNotifier)>>2;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _destroy
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1destroy
(JNIEnv *, jclass, jint aHandle)
{
    CPublishSifOperationInfo *pNotifier =
        reinterpret_cast<CPublishSifOperationInfo*>(aHandle<<2);
    delete pNotifier;
    return KErrNone;
}

#else // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK && RD_JAVA_USIF_NOTIFY_PROGRESS

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _sifNotifierEnabled
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1sifNotifierEnabled
(JNIEnv *, jclass)
{
    return EFalse;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _notifyStart
 * Signature: (IILjava/lang/String;Ljava/lang/String;[Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1notifyStart
(JNIEnv *, jclass, jint, jstring, jstring, jobjectArray, jint, jstring)
{
    LOG(EJavaInstaller, EInfo, "SifNotifier.notifyStart");
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _notifyEnd
 * Signature: (ILjava/lang/String;IILjava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1notifyEnd
(JNIEnv *, jclass, jint, jstring, jint aErrCategory, jint aErrCode, jstring, jstring)
{
    LOG2(EJavaInstaller, EInfo,
         "SifNotifier.notifyEnd: errCategory=%d, errCode=%d",
         aErrCategory, aErrCode);
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _notifyProgress
 * Signature: (ILjava/lang/String;IIII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1notifyProgress
(JNIEnv *, jclass, jint, jstring,
 jint aOperation, jint aSubOperation, jint aCurrent, jint aTotal)
{
    LOG4(EJavaInstaller, EInfo,
         "SifNotifier.notifyProgress: op=%d, subop=%d, current=%d, total=%d",
         aOperation, aSubOperation, aCurrent, aTotal);
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1init
(JNIEnv *, jclass)
{
    LOG(EJavaInstaller, EInfo, "SifNotifier.init");
    return 1; // return dummy object handle
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _destroy
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1destroy
(JNIEnv *, jclass, jint)
{
    LOG(EJavaInstaller, EInfo, "SifNotifier.destroy");
    return KErrNone;
}

#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK && RD_JAVA_USIF_NOTIFY_PROGRESS
