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

#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

#include <hb/hbcore/hbindicatorsymbian.h>
#include <hb/hbcore/hbsymbianvariant.h>
#include <sifuiinstallindicatordefinitions.h>
#include <usif/sif/sifnotification.h>
#include <usif/usifcommon.h>

// NAMESPACE DECLARATION
using namespace java;
using namespace Usif;

IMPORT_C HBufC* CreateHBufCFromJavaStringLC(JNIEnv* aEnv, jstring aString);

// String to be used for icon filenames when icon is not present.
_LIT(KNoIconFilename, "");

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
    JNIEnv *aEnv, CPublishSifOperationInfo *aNotifier, jint aOperation,
    jstring aGlobalComponentId, jstring aComponentName,
    jobjectArray aApplicationNames, jobjectArray aApplicationIcons,
    jint aComponentSize, jstring aIconDir, jstring /*aComponentIcon*/)
{
    __UHEAP_MARK;
    HBufC *globalComponentId = CreateHBufCFromJavaStringLC(aEnv, aGlobalComponentId);
    HBufC *componentName = CreateHBufCFromJavaStringLC(aEnv, aComponentName);
    HBufC *iconDir = NULL;
    if (NULL != aIconDir)
    {
        iconDir = CreateHBufCFromJavaStringLC(aEnv, aIconDir);
    }

    RPointerArray<HBufC> applicationNames;
    CleanupResetAndDestroyPushL(applicationNames);
    RPointerArray<HBufC> applicationIcons;
    CleanupResetAndDestroyPushL(applicationIcons);

    TInt appsCount = aEnv->GetArrayLength(aApplicationNames);
    for (TInt i = 0; i < appsCount; i++)
    {
        HBufC *appName =
            CreateHBufCFromJavaStringLC(
                aEnv, (jstring)aEnv->GetObjectArrayElement(aApplicationNames, i));
        applicationNames.AppendL(appName);
        CleanupStack::Pop(appName);
    }
    if (NULL != aApplicationIcons)
    {
        appsCount = aEnv->GetArrayLength(aApplicationIcons);
        for (TInt i = 0; i < appsCount; i++)
        {
            jstring tmpAppIcon =
                (jstring)aEnv->GetObjectArrayElement(aApplicationIcons, i);
            if (NULL != tmpAppIcon)
            {
                HBufC *appIcon = CreateHBufCFromJavaStringLC(aEnv, tmpAppIcon);
                applicationIcons.AppendL(appIcon);
                CleanupStack::Pop(appIcon);
                aEnv->DeleteLocalRef(tmpAppIcon);
            }
            else
            {
                // Add a string indicating that icon is not available
                // for this application.
                applicationIcons.AppendL(KNoIconFilename().AllocL());
            }
        }
    }

    CSifOperationStartData *startData =
        CSifOperationStartData::NewLC(
            *globalComponentId, *componentName, applicationNames, applicationIcons,
            aComponentSize, /*aIconPath=*/ (NULL != aIconDir? *iconDir: KNullDesC()),
            /*aComponentIcon=*/ KNullDesC(), Usif::KSoftwareTypeJava,
            (TSifOperationPhase)aOperation);

    aNotifier->PublishStartL(*startData);

    CleanupStack::PopAndDestroy(startData);

    CleanupStack::PopAndDestroy(&applicationIcons);
    CleanupStack::PopAndDestroy(&applicationNames);

    if (NULL != aIconDir)
    {
        CleanupStack::PopAndDestroy(iconDir);
    }
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
(JNIEnv *aEnv, jclass, jint aHandle, jint aOperation,
 jstring aGlobalComponentId, jstring aComponentName,
 jobjectArray aApplicationNames, jobjectArray aApplicationIcons,
 jint aComponentSize, jstring aIconDir, jstring aComponentIcon)
{
    CPublishSifOperationInfo *pNotifier =
        reinterpret_cast<CPublishSifOperationInfo*>(aHandle<<2);
    TRAPD(err, NotifyStartL(aEnv, pNotifier, aOperation, aGlobalComponentId,
                            aComponentName, aApplicationNames,
                            aApplicationIcons, aComponentSize,
                            aIconDir, aComponentIcon));
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
    //__UHEAP_MARK;
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

    CSifOperationEndData *endData =
        CSifOperationEndData::NewLC(
            *globalComponentId, (TErrorCategory)aErrCategory, aErrCode,
            (NULL != errMsg? *errMsg: KNullDesC()),
            (NULL != errMsgDetails? *errMsgDetails: KNullDesC()));

    // Do not use UHEAP macros around PublishCompletionL() because it
    // creates a timer object which gets deleted only when the notifier
    // object is deleted.
    aNotifier->PublishCompletionL(*endData);

    CleanupStack::PopAndDestroy(endData);

    if (NULL != errMsgDetails)
    {
        CleanupStack::PopAndDestroy(errMsgDetails);
    }
    if (NULL != errMsg)
    {
        CleanupStack::PopAndDestroy(errMsg);
    }
    CleanupStack::PopAndDestroy(globalComponentId);
    //__UHEAP_MARKEND;
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
    CActiveScheduler* newScheduler = 0;
    if (0 == CActiveScheduler::Current())
    {
        // Create ActiveScheduler as it does not yet exist.
        newScheduler = new CActiveScheduler;
        CActiveScheduler::Install(newScheduler);
    }

    CPublishSifOperationInfo *pNotifier =
        reinterpret_cast<CPublishSifOperationInfo*>(aHandle<<2);
    TRAPD(err, NotifyEndL(aEnv, pNotifier, aGlobalComponentId,
                          aErrCategory, aErrCode, aErrMsg, aErrMsgDetails));

    if (newScheduler)
    {
        delete newScheduler;
        newScheduler = 0;
    }

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

    aNotifier->PublishProgressL(*progressData);

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

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _initIndicator
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1initIndicator
(JNIEnv *, jclass)
{
    CHbIndicatorSymbian *pIndicator = NULL;
    TRAPD(err, pIndicator = CHbIndicatorSymbian::NewL());
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "SifNotifier.initIndicator: Creating indicator failed, error %d",
              err);
        return err;
    }
    // Return handle to the object. Utilize the fact that in Symbian
    // all pointer addresses are MOD 4 so the last 2 bits are 0
    // and can be shifted out. This way the returned handle is
    // always positive whereas Symbian error codes are always negative.
    return reinterpret_cast<TUint>(pIndicator)>>2;
}

/**
 * See JNI method __1updateIndicator.
 * This method makes calls that may leave (the actual notifying).
 */
void UpdateIndicatorL(
    JNIEnv *aEnv, CHbIndicatorSymbian *pIndicator, jstring aName, jint aPhase, jint aProgress)
{
    HBufC *name = CreateHBufCFromJavaStringLC(aEnv, aName);

    CHbSymbianVariantMap *variantMap = CHbSymbianVariantMap::NewL();
    CleanupStack::PushL(variantMap);
    CHbSymbianVariant *variantName = CHbSymbianVariant::NewL(name, CHbSymbianVariant::EDes);
    variantMap->Add(KSifUiInstallIndicatorAppName, variantName);
    CHbSymbianVariant *variantPhase = CHbSymbianVariant::NewL(&aPhase, CHbSymbianVariant::EInt);
    variantMap->Add(KSifUiInstallIndicatorPhase, variantPhase);
    CHbSymbianVariant *variantProgress = CHbSymbianVariant::NewL(&aProgress, CHbSymbianVariant::EInt);
    variantMap->Add(KSifUiInstallIndicatorProgress, variantProgress);

    CHbSymbianVariant *variant = CHbSymbianVariant::NewL(variantMap, CHbSymbianVariant::EVariantMap);
    CleanupStack::PushL(variant);

    TInt err = KErrNone;
    if (!pIndicator->Activate(KSifUiInstallIndicatorType, variant))
    {
        err = pIndicator->Error();
        ELOG1(EJavaInstaller,
              "SifNotifier.updateIndicator: activating indicator failed, error %d",
              err);
        err = KErrGeneral;
    }

    CleanupStack::PopAndDestroy(variant);
    CleanupStack::PopAndDestroy(variantMap);
    CleanupStack::PopAndDestroy(name);
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _updateIndicator
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1updateIndicator
(JNIEnv *aEnv, jclass, jint aHandle, jstring aName, jint aPhase, jint aProgress)
{
    CHbIndicatorSymbian *pIndicator =
        reinterpret_cast<CHbIndicatorSymbian*>(aHandle<<2);
    TRAPD(err, UpdateIndicatorL(aEnv, pIndicator, aName, aPhase, aProgress));
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _destroyIndicator
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1destroyIndicator
(JNIEnv *, jclass, jint aHandle, jint aState)
{
    CHbIndicatorSymbian *pIndicator =
        reinterpret_cast<CHbIndicatorSymbian*>(aHandle<<2);
    TInt err = KErrNone;
    if (aState && !pIndicator->Deactivate(KSifUiInstallIndicatorType))
    {
        err = pIndicator->Error();
        ELOG1(EJavaInstaller,
              "SifNotifier.destroyIndicator: Deactivating indicator failed, error %d",
              err);
        err = KErrGeneral;
    }
    delete pIndicator;
    return err;
}

#else // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

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
(JNIEnv *, jclass, jint, jint, jstring, jstring, jobjectArray, jobjectArray, jint, jstring, jstring)
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
    (void)aErrCategory; // suppress compilation warning about unused argument
    (void)aErrCode; // suppress compilation warning about unused argument
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
    (void)aOperation; // suppress compilation warning about unused argument
    (void)aSubOperation; // suppress compilation warning about unused argument
    (void)aCurrent; // suppress compilation warning about unused argument
    (void)aTotal; // suppress compilation warning about unused argument
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

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _initIndicator
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1initIndicator
(JNIEnv *, jclass)
{
    LOG(EJavaInstaller, EInfo, "SifNotifier.initIndicator");
    return 1; // return dummy object handle
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _updateIndicator
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1updateIndicator
(JNIEnv *, jclass, jint, jstring, jint, jint)
{
    LOG(EJavaInstaller, EInfo, "SifNotifier.updateIndicator");
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifNotifier
 * Method:    _destroyIndicator
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifNotifier__1destroyIndicator
(JNIEnv *, jclass, jint, jint)
{
    LOG(EJavaInstaller, EInfo, "SifNotifier.destroyIndicator");
    return KErrNone;
}

#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
