/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The JNI code for Java Installer component
*                SifRegistrator.
*
*/

#include <apacmdln.h>
#include <apaid.h>
#include <apgcli.h>
#include <apgtask.h>
#include <s32mem.h>
#include <w32std.h>

#include "com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator.h"
#include "com_nokia_mj_impl_utils_InstallerErrorMessage.h"
#include "javacommonutils.h"
#include "javasymbianoslayer.h" // for CleanupResetAndDestroyPushL
#include "logger.h"

// SCR usage is enabled if this macro has been defined.
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

#include <usif/scr/scr.h>
#include <usif/scr/screntries.h>
#include <usif/scr/appreginfo.h>
#include <usif/usiferror.h>
#include <xqappmgr.h>

// Helper macro for logging a TDesC.
#define LOG_TDESC_L(compIdParam, logLevelParam, msgParam, tdescParam) \
    {                                                                 \
        std::wstring ws((wchar_t*)tdescParam.Ptr(), tdescParam.Length()); \
        LOG1(compIdParam, logLevelParam, msgParam, ws.c_str());       \
    }

// NAMESPACE DECLARATION
using namespace java;
using namespace Usif;

IMPORT_C HBufC* CreateHBufCFromJavaStringLC(JNIEnv* aEnv, jstring aString);

// Properties registered to SCR.
_LIT(KUid, "Uid");
_LIT(KMediaId, "Media-Id");
_LIT(KMIDletInfoURL, "MIDlet-Info-URL");
_LIT(KMIDletDescription, "MIDlet-Description");
_LIT(KDownloadURL, "Download-URL");
_LIT(KUpdateURL, "Update-URL");
_LIT(KSettingsPlugin, "SettingsName");
_LIT(KSettingsPluginValue, "javaapplicationsettingsview");

// Symbian file path separator.
_LIT(KPathSeperator, "\\");
// Postfix for the fake application name generated for AppArc.
_LIT(KAppPostfix, ".fakeapp");

/**
 * Internal helper method for checking if specified application
 * is already running.
 *
 * @param aUid application uid
 * @param aBringToForeground if application is running and this
 *   flag is true the application is brought to foreground.
 * @return ETrue if application is running
 */
TBool IsAppRunningL(TUid aUid, TBool aBringToForeground)
{
    TBool result = EFalse;
    // Getting list of current processes requires window
    // server session, create it now.
    RWsSession wsSession;
    TInt err = wsSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "IsAppRunning: Cannot connect to window server, error %d",
              err);
        return result;
    }
    CleanupClosePushL(wsSession);
    // Get list of all processes in the device.
    TApaTaskList taskList(wsSession);
    TApaTask task = taskList.FindApp(aUid);
    if (task.Exists())
    {
        // Application is already running.
        result = ETrue;
        if (aBringToForeground)
        {
            // Bring the application to foreground.
            task.BringToForeground();
        }
    }
    CleanupStack::PopAndDestroy(&wsSession);
    return result;
}

/**
 * Internal helper method for starting specified application.
 *
 * @param aUid application uid
 * @return 1 if application was already running,
 *   0 if application was started,
 *   negative number (Symbian error code) in case of error.
 */
TInt StartAppL(TUid aUid)
{
    if (IsAppRunningL(aUid, ETrue))
    {
        // Application is already running, do nothing.
        LOG1(EJavaInstaller, EInfo,
             "StartAppL: Application 0x%x already running", aUid.iUid);
        return 1;
    }
    TInt err = KErrNone;
    LOG1(EJavaInstaller, EInfo,
         "StartAppL: Starting application 0x%x", aUid.iUid);
    TApaAppInfo appInfo;
    RApaLsSession lsSession;
    err = lsSession.Connect();
    CleanupClosePushL(lsSession);
    if (KErrNone == lsSession.GetAppInfo(appInfo, aUid))
    {
        CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
        cmdLine->SetExecutableNameL(appInfo.iFullName);
        //cmdLine->SetCommandL(EApaCommandBackground);
        //cmdLine->SetTailEndL(_L8(""));
        err = lsSession.StartApp(*cmdLine);
        CleanupStack::PopAndDestroy(cmdLine);
    }
    CleanupStack::PopAndDestroy(&lsSession);
    LOG1(EJavaInstaller, EInfo,
         "StartAppL: Result of starting application is %d", err);
    return err;
}

/**
 * Creates an instance of RSoftwareComponentRegistry and connects to it.
 */
RSoftwareComponentRegistry *CreateScrL()
{
    RSoftwareComponentRegistry *pScr = new RSoftwareComponentRegistry;
    if (NULL == pScr)
    {
        ELOG(EJavaInstaller,
             "CreateScrL: Creating RSoftwareComponentRegistry failed");
        User::Leave(KErrGeneral);
    }
    TInt err = pScr->Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "CreateScrL: Connecting to RSoftwareComponentRegistry failed, error %d",
              err);
        delete pScr;
        User::Leave(err);
    }
    return pScr;
}

/**
 * See JNI method __1notifyAppChange.
 * This method makes calls that may leave (the actual registering).
 */
void NotifyAppChangeL(JNIEnv *aEnv, jintArray aAppUids, jint aAppChange)
{
    RApaLsSession apaSession;
    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "NotifyAppChangeL: Error %d when connecting AppArc", err);
        User::Leave(err);
    }
    CleanupClosePushL(apaSession);

    RArray<TApaAppUpdateInfo> appUpdateInfos;
    CleanupClosePushL(appUpdateInfos);
    TInt appUidCount = aEnv->GetArrayLength(aAppUids);
    jint* appUids = aEnv->GetIntArrayElements(aAppUids, NULL);
    for (TInt i = 0; i < appUidCount; i++)
    {
        TApaAppUpdateInfo appUpdateInfo;
        appUpdateInfo.iAppUid = TUid::Uid(appUids[i]);
        appUpdateInfo.iAction = (TApaAppUpdateInfo::TApaAppAction)aAppChange;
        appUpdateInfos.AppendL(appUpdateInfo);
        LOG2(EJavaInstaller, EInfo, "NotifyAppChangeL uid: 0x%x, action: %d",
             appUpdateInfo.iAppUid, appUpdateInfo.iAction);
    }
    aEnv->ReleaseIntArrayElements(aAppUids, appUids, 0);
    User::LeaveIfError(apaSession.UpdateAppListL(appUpdateInfos));
    CleanupStack::PopAndDestroy(&appUpdateInfos);

    CleanupStack::PopAndDestroy(&apaSession);
    LOG(EJavaInstaller, EInfo, "NotifyAppChangeL completed");
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _notifyAppChange
 * Signature: ([II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1notifyAppChange
(JNIEnv *aEnv, jclass, jintArray aAppUids, jint aAppChange)
{
    TRAPD(err, NotifyAppChangeL(aEnv, aAppUids, aAppChange));
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _launchAppView
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1launchAppView
(JNIEnv *, jclass)
{
    TInt err = KErrNone;

    CActiveScheduler* newScheduler = 0;
    if (0 == CActiveScheduler::Current())
    {
        // Create ActiveScheduler as it does not yet exist.
        newScheduler = new CActiveScheduler;
        CActiveScheduler::Install(newScheduler);
    }

    QUrl openRecentView("appto://20022F35?activityname=AppLibRecentView");
    XQApplicationManager applicationManager;
    XQAiwRequest *request = applicationManager.create(openRecentView);
    if (request)
    {
        LOG(EJavaInstaller, EInfo, "launchAppView: launching AppLib");
        bool result = request->send();
        if (!result)
        {
            int error = request->lastError();
            ELOG1(EJavaInstaller,
                  "launchAppView: launching AppLib failed, error %d", error);
            err = KErrGeneral;
        }
        else
        {
            LOG(EJavaInstaller, EInfo, "launchAppView: launching AppLib succeeded");
        }
        delete request;
    }

    if (newScheduler)
    {
        delete newScheduler;
        newScheduler = 0;
    }

    if (KErrNone == err)
    {
        // Start AppLib and bring it to foreground.
        const TUid KAppLibUid = { 0x20022F35 };
        TRAP(err, StartAppL(KAppLibUid));
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _startSession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1startSession
(JNIEnv *, jclass, jboolean aTransaction)
{
    RSoftwareComponentRegistry *pScr = NULL;
    TRAPD(err, pScr = CreateScrL());
    if (KErrNone != err)
    {
        return err;
    }
    if (aTransaction)
    {
        // Prepare for Java application registrations / unregistrations.
        TRAP(err, pScr->CreateTransactionL());
        if (KErrNone != err)
        {
            // Close session and return error
            ELOG1(EJavaInstaller,
                  "startSession: Creating transaction failed, error %d", err);
            pScr->Close();
            return err;
        }
    }
    // Return handle to session. Utilize the fact that in Symbian
    // all pointer addresses are MOD 4 so the last 2 bits are 0
    // and can be shifted out. This way the returned handle is
    // always positive whereas Symbian error codes are always negative.
    return reinterpret_cast<TUint>(pScr)>>2;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _commitSession
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1commitSession
(JNIEnv *, jclass, jint aSessionHandle)
{
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TRAPD(err, pScr->CommitTransactionL());
    if (KErrNone == err)
    {
        pScr->Close();
        delete pScr;
    }
    else
    {
        ELOG1(EJavaInstaller,
              "commitSession: Commiting transaction failed, error %d", err);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _rollbackSession
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1rollbackSession
(JNIEnv *, jclass, jint aSessionHandle)
{
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TRAPD(err, pScr->RollbackTransactionL());
    pScr->Close();
    delete pScr;
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _closeSession
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1closeSession
(JNIEnv *, jclass, jint aSessionHandle)
{
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    pScr->Close();
    delete pScr;
}

/**
 * Sets component property if given property value is not NULL.
 */
void SetComponentPropertyL(
    JNIEnv *aEnv, RSoftwareComponentRegistry *aScr,
    TComponentId aComponentId, const TDesC &aName, jstring aValue)
{
    if (NULL != aValue)
    {
        HBufC *valueBuf = CreateHBufCFromJavaStringLC(aEnv, aValue);
        aScr->SetComponentPropertyL(aComponentId, aName, *valueBuf);
        //LOG_TDESC_L(EJavaInstaller, EInfo,
        //            "SetComponentPropertyL: name %S", aName);
        //LOG_TDESC_L(EJavaInstaller, EInfo,
        //            "SetComponentPropertyL: value %S", valueBuf->Des());
        CleanupStack::PopAndDestroy(valueBuf);
    }
}

/**
 * See JNI method __1registerComponent.
 * This method makes calls that may leave (the actual registering).
 */
TComponentId RegisterComponentL(
    JNIEnv *aEnv, RSoftwareComponentRegistry *aScr, jint aUid,
    jstring aSuiteName, jstring aVendor, jstring aVersion, jstring aGlobalId,
    jobjectArray aComponentFiles, TInt64 aComponentSize,
    TBool aIsRemovable, TBool aIsDrmProtected,
    TBool aIsOriginVerified, TBool aIsUpdate, jint aMediaId,
    jstring aMidletInfoUrl, jstring aMidletDescription,
    jstring aDownloadUrl, jstring aUpdateUrl)
{
    HBufC *name = CreateHBufCFromJavaStringLC(aEnv, aSuiteName);
    HBufC *vendor = CreateHBufCFromJavaStringLC(aEnv, aVendor);
    HBufC *version = CreateHBufCFromJavaStringLC(aEnv, aVersion);
    HBufC *globalId = CreateHBufCFromJavaStringLC(aEnv, aGlobalId);

    TComponentId componentId = aScr->AddComponentL(
                                   *name, *vendor, Usif::KSoftwareTypeJava, globalId,
                                   (aIsUpdate? EScrCompUpgrade: EScrCompInstall));
    //LOG1(EJavaInstaller, EInfo,
    //     "RegisterComponentL: added component %d", componentId);
    aScr->SetComponentVersionL(componentId, *version);
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: version set");
    aScr->SetComponentSizeL(componentId, aComponentSize);
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: size set");
    aScr->SetIsComponentRemovableL(componentId, aIsRemovable);
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: isRemovable set");
    aScr->SetIsComponentDrmProtectedL(componentId, aIsDrmProtected);
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: DrmProtected set");
    aScr->SetIsComponentOriginVerifiedL(componentId, aIsOriginVerified);
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: isOriginVerified set");
    aScr->SetComponentPropertyL(componentId, KUid(), aUid);
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: Uid property set");
    aScr->SetComponentPropertyL(componentId, KMediaId(), aMediaId);
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: Media-Id property set");
    aScr->SetComponentPropertyL(componentId, KSettingsPlugin(), KSettingsPluginValue());
    //LOG(EJavaInstaller, EInfo, "RegisterComponentL: Settings plugin property set");

    SetComponentPropertyL(aEnv, aScr, componentId, KMIDletInfoURL(), aMidletInfoUrl);
    SetComponentPropertyL(aEnv, aScr, componentId, KMIDletDescription(), aMidletDescription);
    SetComponentPropertyL(aEnv, aScr, componentId, KDownloadURL(), aDownloadUrl);
    SetComponentPropertyL(aEnv, aScr, componentId, KUpdateURL(), aUpdateUrl);

    CleanupStack::PopAndDestroy(globalId);
    CleanupStack::PopAndDestroy(version);
    CleanupStack::PopAndDestroy(vendor);
    CleanupStack::PopAndDestroy(name);

    TInt fileCount = aEnv->GetArrayLength(aComponentFiles);
    for (TInt i = 0; i < fileCount; i++)
    {
        HBufC *componentFile = CreateHBufCFromJavaStringLC(
                                   aEnv, (jstring)aEnv->GetObjectArrayElement(aComponentFiles, i));
        aScr->RegisterComponentFileL(componentId, *componentFile);
        CleanupStack::PopAndDestroy(componentFile);
    }

    return componentId;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _registerComponent
 * Signature: (ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;JZZZZLcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1registerComponent
(JNIEnv *aEnv, jclass, jint aSessionHandle, jint aUid, jstring aSuiteName,
 jstring aVendor, jstring aVersion, jstring aGlobalId,
 jobjectArray aComponentFiles, jlong aComponentSize, jboolean aIsRemovable,
 jboolean aIsDrmProtected, jboolean aIsOriginVerified, jboolean aIsUpdate,
 jint aMediaId, jstring aMidletInfoUrl, jstring aMidletDescription,
 jstring aDownloadUrl, jstring aUpdateUrl, jobject aComponentId)
{
    __UHEAP_MARK;
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TComponentId componentId = -1;
    TRAPD(err, componentId = RegisterComponentL(
                                 aEnv, pScr, aUid, aSuiteName, aVendor, aVersion, aGlobalId,
                                 aComponentFiles, aComponentSize, aIsRemovable,
                                 aIsDrmProtected, aIsOriginVerified, aIsUpdate, aMediaId,
                                 aMidletInfoUrl, aMidletDescription,
                                 aDownloadUrl, aUpdateUrl));
    __UHEAP_MARKEND;
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aComponentId);
        jmethodID methodSetId = aEnv->GetMethodID(clazz, "setId", "(I)V");
        aEnv->CallVoidMethod(aComponentId, methodSetId, componentId);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _unregisterComponent
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1unregisterComponent
(JNIEnv *, jclass, jint aSessionHandle, jint aComponentId)
{
    __UHEAP_MARK;
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TInt err = KErrNone;
    TRAP(err, pScr->DeleteApplicationEntriesL(aComponentId));
    if (KErrNone == err)
    {
        TRAP(err, pScr->DeleteComponentL(aComponentId));
    }
    __UHEAP_MARKEND;
    return err;
}

/**
 * See JNI method __1registerApplication.
 * This method makes calls that may leave.
 */
void RegisterApplicationL(
    JNIEnv *aEnv, RSoftwareComponentRegistry *aScr,
    jint aComponentId, jint aAppUid,
    jstring aAppName, jstring aAppFilename, jstring aGroupName,
    jstring aIconFilename, jint aNumberOfIcons,
    jintArray aLanguages, jobjectArray aAppNames)
{
    __UHEAP_MARK;
    // Application Uid.
    TUid appUid = TUid::Uid(aAppUid);

    // Generate the executable name using the same algorithm as used
    // earlier in S60 platform in case some external S60 application
    // needs to parse the Uid from the executable name.
    TFileName appName;
    appName.Copy(KPathSeperator);
    appName.AppendNum(appUid.iUid);
    appName.Append(KAppPostfix);

    HBufC *caption = CreateHBufCFromJavaStringLC(aEnv, aAppName);
    HBufC *appFilename = CreateHBufCFromJavaStringLC(aEnv, aAppFilename);
    HBufC *groupName = CreateHBufCFromJavaStringLC(aEnv, aGroupName);
    HBufC *iconFilename = NULL;
    TInt numberOfAppIcons = 0;
    if (NULL != aIconFilename)
    {
        iconFilename = CreateHBufCFromJavaStringLC(aEnv, aIconFilename);
        numberOfAppIcons = aNumberOfIcons;
    }

    RPointerArray<HBufC> ownedFileArray; // codescanner::resourcenotoncleanupstack
    RPointerArray<Usif::CServiceInfo> serviceArray; // codescanner::resourcenotoncleanupstack
    RPointerArray<Usif::CPropertyEntry> appPropertiesArray; // codescanner::resourcenotoncleanupstack
    RPointerArray<Usif::CAppViewData> viewDataList; // codescanner::resourcenotoncleanupstack

    RPointerArray<Usif::COpaqueData> opaqueDataArray;
    CleanupResetAndDestroyPushL(opaqueDataArray);
    // Write application Uid to opaque data (needed by MIDlet launcher).
    TBuf8<4> opaqueDataBuf; // Opaque data will contain one signed 32-bit int.
    RDesWriteStream writeStream(opaqueDataBuf);
    writeStream.WriteInt32L(aAppUid);
    writeStream.CommitL();
    COpaqueData *opaqueData = COpaqueData::NewLC(opaqueDataBuf, KNonLocalized);
    opaqueDataArray.AppendL(opaqueData);
    CleanupStack::Pop(opaqueData);

    RPointerArray<Usif::CLocalizableAppInfo> localizableAppInfoList;
    CleanupResetAndDestroyPushL(localizableAppInfoList);
    // Add non-localized application name (caption) and icon.
    CCaptionAndIconInfo *captionAndIconInfo =
        CCaptionAndIconInfo::NewLC(
            /*aCaption=*/ *caption,
            /*aIconFileName=*/ (NULL != aIconFilename? *iconFilename: KNullDesC()),
            /*aNumOfAppIcons=*/ numberOfAppIcons);
    CLocalizableAppInfo *locAppInfo =
        CLocalizableAppInfo::NewLC(
            /*aShortCaption=*/ KNullDesC, /*aApplicationLanguage=*/ KNonLocalized,
            /*aGroupName=*/ KNullDesC, /*aCaptionAndIconInfo=*/ captionAndIconInfo,
            /*aViewDataList=*/ viewDataList);
    localizableAppInfoList.AppendL(locAppInfo);
    CleanupStack::Pop(locAppInfo);
    CleanupStack::Pop(captionAndIconInfo);

    // Add localized application names (captions).
    RPointerArray<HBufC> captionsArray;
    CleanupResetAndDestroyPushL(captionsArray);
    TInt langCount = aEnv->GetArrayLength(aLanguages);
    jint* languages = aEnv->GetIntArrayElements(aLanguages, NULL);
    for (TInt i = 0; i < langCount; i++)
    {
        TLanguage tmpLanguage = (TLanguage)languages[i];
        HBufC *tmpCaption =
            CreateHBufCFromJavaStringLC(
                aEnv, (jstring)aEnv->GetObjectArrayElement(aAppNames, i));
        captionsArray.AppendL(tmpCaption);
        CleanupStack::Pop(tmpCaption);
        //LOG1(EJavaInstaller, EInfo,
        //     "RegisterApplicationL: language %d", tmpLanguage);
        //LOG_TDESC_L(EJavaInstaller, EInfo,
        //            "RegisterApplicationL: caption %S", tmpCaption->Des());
        CCaptionAndIconInfo *tmpCaptionAndIconInfo =
            CCaptionAndIconInfo::NewLC(
                /*aCaption=*/ *tmpCaption,
                /*aIconFileName=*/ KNullDesC,
                /*aNumOfAppIcons=*/ 0);
        CLocalizableAppInfo *tmpLocAppInfo =
            CLocalizableAppInfo::NewLC(
                /*aShortCaption=*/ KNullDesC,
                /*aApplicationLanguage=*/ tmpLanguage,
                /*aGroupName=*/ KNullDesC,
                /*aCaptionAndIconInfo=*/ tmpCaptionAndIconInfo,
                /*aViewDataList=*/ viewDataList);
        localizableAppInfoList.AppendL(tmpLocAppInfo);
        CleanupStack::Pop(tmpLocAppInfo);
        CleanupStack::Pop(tmpCaptionAndIconInfo);
    }
    aEnv->ReleaseIntArrayElements(aLanguages, languages, 0);

    // Create application registration data objects.
    TApplicationCharacteristics appCharacteristics;
    appCharacteristics.iAttributes = TApaAppCapability::ENonNative;
    appCharacteristics.iEmbeddability = TApplicationCharacteristics::ENotEmbeddable;
    appCharacteristics.iSupportsNewFile = EFalse;
    appCharacteristics.iAppIsHidden = EFalse;
    appCharacteristics.iLaunchInBackground = EFalse;
    appCharacteristics.iGroupName = *groupName;
    CApplicationRegistrationData *appRegData =
        CApplicationRegistrationData::NewLC(
            /*aOwnedFileArray=*/ ownedFileArray,
            /*aServiceArray=*/ serviceArray,
            /*aLocalizableAppInfoList=*/ localizableAppInfoList,
            /*aAppPropertiesArray=*/ appPropertiesArray,
            /*aOpaqueDataArray=*/ opaqueDataArray,
            /*aAppUid=*/ appUid, /*aAppFile=*/ appName,
            /*aCharacteristics=*/ appCharacteristics,
            /*aDefaultScreenNumber=*/ 0);
    aScr->AddApplicationEntryL(aComponentId, *appRegData);
    CleanupStack::PopAndDestroy(appRegData);

    CleanupStack::PopAndDestroy(&captionsArray);
    CleanupStack::Pop(&localizableAppInfoList); // deleted in appRegData destructor
    CleanupStack::Pop(&opaqueDataArray); // deleted in appRegData destructor

    if (NULL != aIconFilename)
    {
        CleanupStack::PopAndDestroy(iconFilename);
    }
    CleanupStack::PopAndDestroy(groupName);
    CleanupStack::PopAndDestroy(appFilename);
    CleanupStack::PopAndDestroy(caption);
    __UHEAP_MARKEND;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _registerApplication
 * Signature: (IIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I[I[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1registerApplication
(JNIEnv *aEnv, jclass, jint aSessionHandle, jint aComponentId, jint aAppUid,
 jstring aAppName, jstring aAppFilename, jstring aGroupName,
 jstring aIconFilename, jint aNumberOfIcons,
 jintArray aLanguages, jobjectArray aAppNames)
{
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TRAPD(err, RegisterApplicationL(
              aEnv, pScr, aComponentId, aAppUid, aAppName, aAppFilename,
              aGroupName, aIconFilename, aNumberOfIcons,
              aLanguages, aAppNames));
    return err;
}

/**
 * See JNI method __1registerLocalizedComponentName.
 * This method makes calls that may leave.
 */
void RegisterLocalizedComponentNameL(
    JNIEnv *aEnv, RSoftwareComponentRegistry *aScr, jint aComponentId,
    jstring aName, jstring aVendor, jint aLanguage)
{
    HBufC *name = CreateHBufCFromJavaStringLC(aEnv, aName);
    aScr->SetComponentNameL(aComponentId, *name, (TLanguage)aLanguage);
    CleanupStack::PopAndDestroy(name);

    HBufC *vendor = CreateHBufCFromJavaStringLC(aEnv, aVendor);
    aScr->SetVendorNameL(aComponentId, *vendor, (TLanguage)aLanguage);
    CleanupStack::PopAndDestroy(vendor);
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _registerLocalizedComponentName
 * Signature: (IILjava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1registerLocalizedComponentName
(JNIEnv *aEnv, jclass, jint aSessionHandle, jint aComponentId, jstring aName, jstring aVendor, jint aLanguage)
{
    __UHEAP_MARK;
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TRAPD(err, RegisterLocalizedComponentNameL(
              aEnv, pScr, aComponentId, aName, aVendor, aLanguage));
    __UHEAP_MARKEND;
    return err;
}

/**
 * See JNI method __1setLocalizedComponentProperty.
 * This method makes calls that may leave.
 */
void SetLocalizedComponentPropertyL(
    JNIEnv *aEnv, RSoftwareComponentRegistry *aScr, jint aComponentId,
    jstring aName, jstring aValue, jint aLanguage)
{
    if (NULL != aName && NULL != aValue)
    {
        HBufC *name = CreateHBufCFromJavaStringLC(aEnv, aName);
        HBufC *value = CreateHBufCFromJavaStringLC(aEnv, aValue);
        if (KUnspecifiedLocale == aLanguage)
        {
            aScr->SetComponentPropertyL(aComponentId, *name, *value);
        }
        else
        {
            aScr->SetComponentPropertyL(
                aComponentId, *name, *value, (TLanguage)aLanguage);
        }
        CleanupStack::PopAndDestroy(value);
        CleanupStack::PopAndDestroy(name);
    }
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _setLocalizedComponentProperty
 * Signature: (IILjava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1setLocalizedComponentProperty
(JNIEnv *aEnv, jclass, jint aSessionHandle, jint aComponentId, jstring aName, jstring aValue, jint aLanguage)
{
    __UHEAP_MARK;
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TRAPD(err, SetLocalizedComponentPropertyL(
              aEnv, pScr, aComponentId, aName, aValue, aLanguage));
    __UHEAP_MARKEND;
    return err;
}

/**
 * See JNI method __1getComponentId.
 * This method makes calls that may leave.
 */
TComponentId GetComponentIdL(
    JNIEnv *aEnv, RSoftwareComponentRegistry *aScr, jstring aGlobalId)
{
    HBufC *globalId = CreateHBufCFromJavaStringLC(aEnv, aGlobalId);
    TComponentId componentId =
        aScr->GetComponentIdL(*globalId, Usif::KSoftwareTypeJava);
    CleanupStack::PopAndDestroy(globalId);
    return componentId;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getComponentId
 * Signature: (ILjava/lang/String;Lcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getComponentId
(JNIEnv *aEnv, jclass, jint aSessionHandle, jstring aGlobalId, jobject aComponentId)
{
    __UHEAP_MARK;
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TComponentId componentId = -1;
    TRAPD(err, componentId = GetComponentIdL(aEnv, pScr, aGlobalId));
    __UHEAP_MARKEND;
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aComponentId);
        jmethodID methodSetId = aEnv->GetMethodID(clazz, "setId", "(I)V");
        aEnv->CallVoidMethod(aComponentId, methodSetId, componentId);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getComponentIdForApp
 * Signature: (IILcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getComponentIdForApp
(JNIEnv *aEnv, jclass, jint aSessionHandle, jint aAppUid, jobject aComponentId)
{
    TInt err = KErrNone;
    __UHEAP_MARK;
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TUid appUid = TUid::Uid(aAppUid);
    TComponentId componentId = -1;
    TRAP(err, componentId = pScr->GetComponentIdForAppL(appUid));
    __UHEAP_MARKEND;
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aComponentId);
        jmethodID methodSetId = aEnv->GetMethodID(clazz, "setId", "(I)V");
        aEnv->CallVoidMethod(aComponentId, methodSetId, componentId);
    }
    return err;
}

/**
 * See JNI method __1getuid.
 * This method makes calls that may leave.
 */
TInt GetUidL(jint aCid)
{
    // Create a new tmporary instance of RSoftwareComponentRegistry.
    RSoftwareComponentRegistry *pScr = CreateScrL();
    CleanupClosePushL(*pScr);
    TInt uid = 0;
    CPropertyEntry *property = pScr->GetComponentPropertyL(aCid, KUid());
    uid = ((CIntPropertyEntry*)property)->IntValue();
    delete property;
    // Close and delete the temporary RSoftwareComponentRegistry.
    CleanupStack::PopAndDestroy(pScr);
    delete pScr; // For some reason PopAndDestroy does not delete this.
    return uid;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getUid
 * Signature: (IILcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getUid
(JNIEnv *aEnv, jclass, jint aCid, jobject aComponentId)
{
    __UHEAP_MARK;
    TInt uid = 0;
    TRAPD(err, uid = GetUidL(aCid));
    __UHEAP_MARKEND;
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aComponentId);
        jmethodID methodSetId = aEnv->GetMethodID(clazz, "setId", "(I)V");
        aEnv->CallVoidMethod(aComponentId, methodSetId, uid);
    }
    return err;
}

/**
 * See JNI method __1logComponent.
 * This method makes calls that may leave.
 */
void LogComponentL(JNIEnv *aEnv, RSoftwareComponentRegistry *aScr, jstring aGlobalId, TLanguage aLanguage)
{
    HBufC *globalId = CreateHBufCFromJavaStringLC(aEnv, aGlobalId);
    CComponentEntry *componentEntry =
        aScr->GetComponentL(*globalId, Usif::KSoftwareTypeJava, aLanguage);
    CleanupStack::PopAndDestroy(globalId);

    // Log component entry.
    TComponentId componentId = componentEntry->ComponentId();
    LOG_TDESC_L(EJavaInstaller, EInfo, "GlobalId: %S", componentEntry->GlobalId());
    LOG1(EJavaInstaller, EInfo, "ComponentId: %d", componentId);
    LOG_TDESC_L(EJavaInstaller, EInfo, "SoftwareType: %S", componentEntry->SoftwareType());
    LOG_TDESC_L(EJavaInstaller, EInfo, "Name: %S", componentEntry->Name());
    LOG_TDESC_L(EJavaInstaller, EInfo, "Vendor: %S", componentEntry->Vendor());
    LOG_TDESC_L(EJavaInstaller, EInfo, "Version: %S", componentEntry->Version());
    LOG1(EJavaInstaller, EInfo, "ComponentSize: %d", componentEntry->ComponentSize());
    LOG1(EJavaInstaller, EInfo, "ScomoState: %d", componentEntry->ScomoState());
    LOG1(EJavaInstaller, EInfo, "IsDrmProtected: %d", componentEntry->IsDrmProtected());
    LOG1(EJavaInstaller, EInfo, "IsHidden: %d", componentEntry->IsHidden());
    LOG1(EJavaInstaller, EInfo, "IsKnownRevoked: %d", componentEntry->IsKnownRevoked());
    LOG1(EJavaInstaller, EInfo, "IsOriginVerified: %d", componentEntry->IsOriginVerified());
    LOG1(EJavaInstaller, EInfo, "IsRemovable: %d", componentEntry->IsRemovable());
    TDriveList driveList = componentEntry->InstalledDrives();
    for (TInt i = EDriveA; i < driveList.Length(); i++)
    {
        if (driveList[i])
        {
            LOG1(EJavaInstaller, EInfo, "InstalledDrive: %d", i);
        }
    }
    delete componentEntry;

    // Log component properties.
    RPointerArray<CPropertyEntry> properties;
    aScr->GetComponentPropertiesL(componentId, properties, aLanguage);
    CleanupResetAndDestroyPushL(properties);
    for (TInt i = 0; i < properties.Count(); i++)
    {
        CPropertyEntry *propertyEntry = properties[i];
        switch (propertyEntry->PropertyType())
        {
        case CPropertyEntry::EBinaryProperty:
            LOG_TDESC_L(EJavaInstaller, EInfo, "BinaryProperty: %S",
                        propertyEntry->PropertyName());
            break;
        case CPropertyEntry::EIntProperty:
            LOG_TDESC_L(EJavaInstaller, EInfo, "IntProperty: %S",
                        propertyEntry->PropertyName());
            LOG2(EJavaInstaller, EInfo, "  = 0x%x (%d)",
                 ((CIntPropertyEntry*)propertyEntry)->IntValue(),
                 ((CIntPropertyEntry*)propertyEntry)->IntValue());
            break;
        case CPropertyEntry::ELocalizedProperty:
            LOG_TDESC_L(EJavaInstaller, EInfo, "LocalizedProperty: %S",
                        propertyEntry->PropertyName());
            LOG_TDESC_L(EJavaInstaller, EInfo, " = %S",
                        ((CLocalizablePropertyEntry*)propertyEntry)->StrValue());
            break;
        }
    }
    CleanupStack::PopAndDestroy(&properties);

    // Log uids of applications associated to component.
    RArray<TUid> appUids;
    CleanupClosePushL(appUids);
    aScr->GetAppUidsForComponentL(componentId, appUids);
    LOG1(EJavaInstaller, EInfo, "Number of AppUids found: %d", appUids.Count());
    for (TInt i = 0; i < appUids.Count(); i++)
    {
        LOG2(EJavaInstaller, EInfo, "AppUid [%x] (%d)",
             appUids[i].iUid, appUids[i].iUid);
    }
    CleanupStack::PopAndDestroy(&appUids);
}

/**
 * See JNI method __1logComponent.
 * This method makes calls that may leave.
 */
void LogComponentL(JNIEnv *aEnv, RSoftwareComponentRegistry *aScr, jstring aGlobalId)
{
    // Log the component with all supported languages.
    TComponentId componentId = GetComponentIdL(aEnv, aScr, aGlobalId);
    RArray<TLanguage> languages;
    CleanupClosePushL(languages);
    aScr->GetComponentSupportedLocalesListL(componentId, languages);
    LogComponentL(aEnv, aScr, aGlobalId, KNonLocalized);
    for (TInt i = 0; i < languages.Count(); i++)
    {
        if (KNonLocalized == i)
        {
            continue;
        }
        TLanguage language = languages[i];
        LOG1(EJavaInstaller, EInfo, "Component supports language %d", language);
        LogComponentL(aEnv, aScr, aGlobalId, language);
    }
    CleanupStack::PopAndDestroy(&languages);
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _logComponent
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1logComponent
(JNIEnv *aEnv, jclass, jint aSessionHandle, jstring aGlobalId)
{
    //__UHEAP_MARK;
    RSoftwareComponentRegistry *pScr =
        reinterpret_cast<RSoftwareComponentRegistry*>(aSessionHandle<<2);
    TRAPD(err, LogComponentL(aEnv, pScr, aGlobalId));
    //__UHEAP_MARKEND;
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getUsifMode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getUsifMode
(JNIEnv *, jclass)
{
    return 1;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getErrorCategory
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getErrorCategory
(JNIEnv *, jclass, jint aErrorId)
{
    int errorCategory = Usif::EUnexpectedError;
    switch (aErrorId)
    {
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_NO_MEM:
        errorCategory = Usif::ELowDiskSpace;
        break;
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_NO_NET:
        errorCategory = Usif::ENetworkUnavailable;
        break;
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_CORRUPT_PKG:
        errorCategory = Usif::ECorruptedPackage;
        break;
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_COMPAT_ERR:
        errorCategory = Usif::EApplicationNotCompatible;
        break;
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_AUTHORIZATION_ERR:
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_AUTHENTICATION_ERR:
        errorCategory = Usif::ESecurityError;
        break;
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_PUSH_REG_ERR:
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_UNEXPECTED_ERR:
    case com_nokia_mj_impl_utils_InstallerErrorMessage_UNINST_UNEXPECTED_ERR:
    case com_nokia_mj_impl_utils_InstallerErrorMessage_OTHER_UNEXPECTED_ERR:
        errorCategory = Usif::EUnexpectedError;
        break;
    case com_nokia_mj_impl_utils_InstallerErrorMessage_INST_CANCEL:
    case com_nokia_mj_impl_utils_InstallerErrorMessage_UNINST_CANCEL:
        errorCategory = Usif::EUserCancelled;
        break;
    case com_nokia_mj_impl_utils_InstallerErrorMessage_UNINST_NOT_ALLOWED:
        errorCategory = Usif::EUninstallationBlocked;
        break;
    }
    return errorCategory;
}

#else // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _notifyAppChange
 * Signature: ([II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1notifyAppChange
(JNIEnv *, jclass, jintArray, jint)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _launchAppView
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1launchAppView
(JNIEnv *, jclass)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _startSession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1startSession
(JNIEnv *, jclass, jboolean)
{
    return 1; // dummy session handle
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _commitSession
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1commitSession
(JNIEnv *, jclass, jint)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _rollbackSession
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1rollbackSession
(JNIEnv *, jclass, jint)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _closeSession
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1closeSession
(JNIEnv *, jclass, jint)
{
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _registerComponent
 * Signature: (ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;JZZZZLcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1registerComponent
(JNIEnv *, jclass, jint, jint, jstring, jstring, jstring, jstring,
 jobjectArray, jlong, jboolean, jboolean, jboolean, jboolean, jint,
 jstring, jstring, jstring, jstring, jobject)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _unregisterComponent
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1unregisterComponent
(JNIEnv *, jclass, jint, jint)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _registerApplication
 * Signature: (IIILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I[I[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1registerApplication
(JNIEnv *, jclass, jint, jint, jint, jstring, jstring, jstring, jstring, jint, jintArray, jobjectArray)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _registerLocalizedComponentName
 * Signature: (IILjava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1registerLocalizedComponentName
(JNIEnv *, jclass, jint, jint, jstring, jstring, jint)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _setLocalizedComponentProperty
 * Signature: (IILjava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1setLocalizedComponentProperty
(JNIEnv *, jclass, jint, jint, jstring, jstring, jint)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getComponentId
 * Signature: (ILjava/lang/String;Lcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getComponentId
(JNIEnv *, jclass, jint, jstring, jobject)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getComponentIdForApp
 * Signature: (IILcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getComponentIdForApp
(JNIEnv *, jclass, jint, jint, jobject)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getUid
 * Signature: (IILcom/nokia/mj/impl/installer/applicationregistrator/ComponentId;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getUid
(JNIEnv *, jclass, jint, jobject)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _logComponent
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1logComponent
(JNIEnv *, jclass, jint, jstring)
{
    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getUsifMode
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getUsifMode
(JNIEnv *, jclass)
{
    return 0;
}

/*
 * Class:     com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator
 * Method:    _getErrorCategory
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_SifRegistrator__1getErrorCategory
(JNIEnv *, jclass, jint)
{
    return 0;
}

#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
