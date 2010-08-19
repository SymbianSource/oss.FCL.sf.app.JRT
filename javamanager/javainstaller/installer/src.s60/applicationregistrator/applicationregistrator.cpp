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
*                ApplicationRegistrator.
*
*/


#include <apaid.h>
#include <f32file.h>
#include <apgcli.h>
#include <utf.h>
#include <s32mem.h>
#include <bautils.h>
#include <w32std.h>
#include <apgtask.h>
#include <apacmdln.h>
#include <centralrepository.h>
#include <AknUtils.h>
#include <hal.h>

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
#include <SWInstLogTaskParam.h>
#include <SWInstTask.h>
#include <SWInstTaskManager.h>
#endif // !SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <apgicnfl.h>
#else
#include <apgicnflpartner.h>
#endif



// JNI header must be included before iconconverter.h. Latter includes some
// other header file which defines WIN32 macro, causing incorrect definitions
// to be taken from jniport.h when WINSCW compilation is made.
// This error situation can be seen from linker warnings which complain
// about not frozen exports for JNI methods.
#include "com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator.h"
#include "iconconverter.h"
#include "javacommonutils.h"
#include "logger.h"

// NAMESPACE DECLARATION
using namespace java;
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
using namespace SwiUI;
#endif // !SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

IMPORT_C HBufC* CreateHBufCFromJavaStringLC(JNIEnv* aEnv, jstring aString);

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
/**
 * MIDP Stub SIS file UID. The application type Uid for MIDlets in S60
 */
const TUid KMidletApplicationTypeUid = { 0x10210E26 };

/**
 * Symbian file path separator
 */
_LIT(KPathSeperator, "\\");

/**
 * Postfix for the fake application name generated only to make AppArc happy
 */
_LIT(KAppPostfix, ".fakeapp");


#ifdef RD_JAVA_S60_RELEASE_9_2
const TInt KAppIconCount = 2;
#else
const TInt KAppIconCount = 1;
#endif

// ------------------------

/**
 * Internal wrapper method that is used to make TRAPping the leaves in the
 * native Symbian code easier.
 * Used from JNI method ...1registerApplication()
 * @see Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1registerApplication
 */
jint registerApplicationL(JNIEnv *aEnv, jclass aClass, jint aSessionHandle,
                          jint aUid, HBufC16 *aGroupName, HBufC16 *aMIDletName, HBufC16 *aTargetDrive,
                          HBufC16 *aIconFileName, HBufC16 *aJarFileName, jboolean aHidden,
                          jboolean aBackground);

/**
 * Internal helper method for checking whether this code is executing in
 * a device that has correctly working version of
 * RApaLsSession::ForceCommitNonNativeApplicationsUpdatesL()
 *
 * @return ETrue if force commit works well
 */
TBool isForceCommitSupported();
#endif // !SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

/**
 * Internal helper method for checking whether Application Shell is already running
 * Used JNI method ...1startAppShellUi
 *
 * @return ETrue if AppShell is running
 */
TBool isAppShellUiRunning();

/**
 * Internal helper method for starting menu application
 *
 * @return KErrNone or Symbian error code
 */
TInt startAppShellApplicationL();

// --------------------------

/**
 * Starts native application registration session.
 *
 * @return native session handle or Symbian error code (negative number)
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1startSession
(JNIEnv *, jclass)
{
    TInt err;
    RApaLsSession *pApaSession = new RApaLsSession;

    if (NULL == pApaSession)
    {
        // Could not create session
        return KErrGeneral;
    }

    err = pApaSession->Connect();
    if (KErrNone != err)
    {
        // Cannot connect to AppArc server
        return err;
    }

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    // Delete any pending (un)registrations (possible if
    // e.g. device rebooted before commit).
    // This call does nothing if there is no pending registrations.
    // Ignore errors.
    (void)pApaSession->RollbackNonNativeApplicationsUpdates();

    // Prepare for Java application registrations / unregistrations
    TRAP(err, pApaSession->PrepareNonNativeApplicationsUpdatesL());
    if (KErrNone != err)
    {
        // Close session and return error
        pApaSession->Close();
        return err;
    }
#endif // !SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

    // Return handle to session. Utilize the fact that in Symbian
    // all pointer addresses are MOD 4 so the last 2 bits are 0
    // and can be shifted out. This way the returned handle is
    // always positive whereas Symbian error codes are always negative.
    return reinterpret_cast<TUint>(pApaSession)>>2;
}


/**
 * Registers Java Application to S60 AppArc
 *
 * @param[in] aSessionHandle
 * @param[in] aUid The Uid of the application.
 * @param[in] aGroupName  Max group name len is 16 in AppArc
 * @param[in] aMIDletName
 * @param[in] aTargetDrive Must be "C:", "E:", ..., "J:"
 * @param[in] aIconFileName The name of the icon
 * @param[in] aJarFileName The full path name of the jar file
 * @param[in] aHidden
 * @param[in] aBackground
 * @return 0 if registration succeeded or Symbian error code
 */
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1registerApplication
(JNIEnv */*aEnv*/, jclass /*aClass*/, jint /*aSessionHandle*/, jint /*aUid*/, jstring /*aGroupName*/,
 jstring /*aMIDletName*/, jstring /*aTargetDrive*/, jstring /*aIconFileName*/,
 jstring /*aJarFileName*/, jboolean /*aHidden*/, jboolean /*aBackground*/)
{
    return KErrNone;
}
#else // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1registerApplication
(JNIEnv *aEnv, jclass aClass, jint aSessionHandle, jint aUid, jstring aGroupName,
 jstring aMIDletName, jstring aTargetDrive, jstring aIconFileName,
 jstring aJarFileName, jboolean aHidden, jboolean aBackground)
{
    TInt err(KErrNone);
    TBool fStayInLoop(EFalse);

    HBufC16 *unicodeIconFileName(NULL);
    HBufC16 *unicodeGroupName(NULL);
    HBufC16 *unicodeMIDletName(NULL);
    HBufC16 *unicodeTargetDrive(NULL);
    HBufC16 *unicodeJarFileName(NULL);

    // create loop that is executed only once to make error handling easier
    do
    {
        // Copy the unicode characters in jstrings to Symbian buffers so that
        // they can be used in S60 code
        const jchar *iconFileName = aEnv->GetStringChars(aIconFileName, 0);
        if (NULL == iconFileName)
        {
            ELOG(
                EJavaInstaller,
                "Cannot register application, getting icon file name as unicode failed in JNI");
            err = KErrArgument;
            break;
        }
        unicodeIconFileName = HBufC16::New(aEnv->GetStringLength(aIconFileName) + 1);
        if (NULL == unicodeIconFileName)
        {
            err = KErrNoMemory;
            break;
        }
        *unicodeIconFileName = iconFileName;
        aEnv->ReleaseStringChars(aIconFileName, iconFileName);

        const jchar *groupName = aEnv->GetStringChars(aGroupName, 0);
        if (NULL == groupName)
        {
            ELOG(
                EJavaInstaller,
                "Cannot register application, getting group name as unicode failed in JNI");
            err = KErrArgument;
            break;
        }
        unicodeGroupName = HBufC16::New(aEnv->GetStringLength(aGroupName) + 1);
        if (NULL == unicodeGroupName)
        {
            err = KErrNoMemory;
            break;
        }
        *unicodeGroupName = groupName;
        aEnv->ReleaseStringChars(aGroupName, groupName);

        const jchar *midletName = aEnv->GetStringChars(aMIDletName, 0);
        if (NULL == midletName)
        {
            ELOG(
                EJavaInstaller,
                "Cannot register application, getting midlet name as unicode failed in JNI");
            err = KErrArgument;
            break;
        }
        unicodeMIDletName = HBufC16::New(aEnv->GetStringLength(aMIDletName) + 1);
        if (NULL == unicodeMIDletName)
        {
            err = KErrNoMemory;
            break;
        }
        *unicodeMIDletName = midletName;
        aEnv->ReleaseStringChars(aMIDletName, midletName);

        const jchar *targetDrive = aEnv->GetStringChars(aTargetDrive, 0);
        if (NULL == targetDrive)
        {
            ELOG(
                EJavaInstaller,
                "Cannot register application, getting target drive as unicode failed in JNI");
            err = KErrArgument;
            break;
        }
        unicodeTargetDrive = HBufC16::New(aEnv->GetStringLength(aTargetDrive) + 1);
        if (NULL == unicodeTargetDrive)
        {
            err = KErrNoMemory;
            break;
        }
        *unicodeTargetDrive = targetDrive;
        aEnv->ReleaseStringChars(aTargetDrive, targetDrive);

        const jchar *jarFileName = aEnv->GetStringChars(aJarFileName, 0);
        if (NULL == jarFileName)
        {
            ELOG(
                EJavaInstaller,
                "Cannot register application, getting jar file name as unicode failed in JNI");
            err = KErrArgument;
            break;
        }
        unicodeJarFileName = HBufC16::New(aEnv->GetStringLength(aJarFileName) + 1);
        if (NULL == unicodeTargetDrive)
        {
            err = KErrNoMemory;
            break;
        }
        *unicodeJarFileName = jarFileName;
        aEnv->ReleaseStringChars(aJarFileName, jarFileName);
    }
    while (fStayInLoop);

    if (KErrNone != err)
    {
        delete unicodeIconFileName;
        delete unicodeGroupName;
        delete unicodeMIDletName;
        delete unicodeTargetDrive;
        delete unicodeJarFileName;

        return err;
    }

    // Call the actual register method inside TRAP harness
    TInt ret = KErrNone;
    TRAP(err,
         ret = registerApplicationL(
                   aEnv, aClass, aSessionHandle, aUid, unicodeGroupName,
                   unicodeMIDletName, unicodeTargetDrive, unicodeIconFileName,
                   unicodeJarFileName, aHidden, aBackground));

    LOG1WSTR(
        EJavaInstaller,
        EInfo,
        "Registering application, midlet name (%s)",
        (wchar_t *)(unicodeMIDletName->Des().PtrZ()));
    LOG1WSTR(
        EJavaInstaller,
        EInfo,
        "Registering application, icon file name (%s)",
        (wchar_t *)(unicodeIconFileName->Des().PtrZ()));

    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "Cannot register application, err code is %d ",
              err);
    }
    if (KErrNone != ret)
    {
        // error even though the code did not leave
        err = ret;

        ELOG1(EJavaInstaller,
              "Cannot register application, non-leaving error %d",
              ret);
    }

    // Free all Symbian buffers containing copies of jstring data
    delete unicodeIconFileName;
    delete unicodeGroupName;
    delete unicodeMIDletName;
    delete unicodeTargetDrive;
    delete unicodeJarFileName;

    return err;
}


/**
 * See JNI method __1registerApplication.
 * This method makes all calls that may leave (the actual registering)
 */
jint registerApplicationL(
    JNIEnv *, jclass, jint aSessionHandle, jint aUid, HBufC16 *aGroupName,
    HBufC16 *aMIDletName, HBufC16 *aTargetDrive, HBufC16 *aIconFileName,
    HBufC16 * /*aJarFileName*/, jboolean aHidden, jboolean aBackground)
{
    RFs         fileServ;
    RFile       appArcIcon;
    TUid        midletUid;
    TInt        err;
    TInt        numberOfIcons = KAppIconCount; // number of icons stored in MBM
    // Also default mbm has one icon.

    midletUid.iUid = aUid;

    // AppArc requires open RFile handle
    err = fileServ.Connect();
    if (KErrNone != err)
    {
        // Will not register application without icon
        ELOG1(EJavaInstaller,
              "Cannot open RFs connection, err code is %d ", err);
        return err;
    }
    CleanupClosePushL(fileServ);

    // The session must be shared so that AppArc can
    // use the same icon file handle
    err = fileServ.ShareProtected();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "Cannot share file server session, err code is %d ", err);
        User::Leave(err);
    }

    // Default icon has been copied from ROM, so the icon may have ReadOnly
    // flag on. Must remove that flag.
    fileServ.SetAtt(*aIconFileName, 0, KEntryAttReadOnly);

    // open the icon
    err = appArcIcon.Open(
              fileServ,
              *aIconFileName,
              (EFileShareReadersOrWriters | EFileWrite));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "registerApplicationL: Cannot open icon, err code is %d ", err);
        User::Leave(err);
    }

    // Now icon is open
    CleanupClosePushL(appArcIcon);

    // Generate the executable name name using the same
    // algorithm as used earlier in S60 platform
    // in case some external S60 application
    // needs to parse the Uid from the executable name.
    TFileName appName;
    appName.Copy(KPathSeperator);
    appName.AppendNum(midletUid.iUid);
    appName.Append(KAppPostfix);

    // Create writer needed for actual registration
    CApaRegistrationResourceFileWriter *writer = NULL;
    writer = CApaRegistrationResourceFileWriter::NewL(
                 midletUid,
                 appName,
                 TApaAppCapability::ENonNative);
    CleanupStack::PushL(writer);

    writer->SetGroupNameL(*aGroupName);
    if (aBackground)
    {
        // By default this is false, so the value needs to be set
        // only if it is true
        writer->SetLaunchInBackgroundL(ETrue);
    }
    if (aHidden)
    {
        // By default this is false, so the value needs to be set
        // only if it is true
        writer->SetAppIsHiddenL(ETrue);
    }

    // write application Uid to opaque data   (needed by MIDlet launcher)
    TBuf8<4>         opaqueData;     // opaque data will contain one signed 32-bit int
    RDesWriteStream  writeStream(opaqueData);
    writeStream.WriteInt32L(midletUid.iUid);
    writeStream.CommitL();
    writer->SetOpaqueDataL(opaqueData);

    CApaLocalisableResourceFileWriter*  lWriter = NULL;
    lWriter =
        CApaLocalisableResourceFileWriter::NewL(
            KNullDesC,  // short caption
            *aMIDletName,  // caption
            numberOfIcons,
            KNullDesC);
    CleanupStack::PushL(lWriter);

    TDriveUnit targetDrive(*aTargetDrive);

    // Convert session handle to pointer.
    RApaLsSession *pApaSession =
        reinterpret_cast<RApaLsSession*>(aSessionHandle<<2);

    // Register application to AppArc
    pApaSession->RegisterNonNativeApplicationL(
        KMidletApplicationTypeUid,
        targetDrive,
        *writer,
        lWriter,
        &appArcIcon);

    CleanupStack::PopAndDestroy(lWriter);
    CleanupStack::PopAndDestroy(writer);
    // close icon file handle
    CleanupStack::PopAndDestroy(&appArcIcon);

    // close file server connection
    CleanupStack::PopAndDestroy(&fileServ);

    return err;
}
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK


/**
 * Unregisters Java Application from S60 AppArc
 *
 * @param[in] aSessionHandle
 * @param[in] aUid The Uid of the application to be unregistered..
 * @return 0 if unregistration succeeded or Symbian error code
 */
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1unregisterApplication
(JNIEnv *, jclass, jint /*aSessionHandle*/, jint /*aUid*/)
{
    return KErrNone;
}
#else // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1unregisterApplication
(JNIEnv *, jclass, jint aSessionHandle, jint aUid)
{
    // Convert session handle to pointer.
    RApaLsSession *pApaSession =
        reinterpret_cast<RApaLsSession*>(aSessionHandle<<2);

    TUid appUid;
    appUid.iUid = aUid;
    TRAPD(err, pApaSession->DeregisterNonNativeApplicationL(appUid));
    return err;
}
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK


/**
 * Commits native application registration session.
 * If commit succeeds the native session is closed.
 *
 * @param[in] aSessionHandle
 * @param[in] aSynchronous if true, makes synchronous commit
 * @return 0 or Symbian error code (negative number)
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1commitSession
(JNIEnv *, jclass, jint aSessionHandle, jboolean aSynchronous)
{
    (void)aSynchronous; // suppress compilation warning about unused argument
    // Convert session handle to pointer.
    RApaLsSession *pApaSession =
        reinterpret_cast<RApaLsSession*>(aSessionHandle<<2);

    TInt err = KErrNone;

#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    if (aSynchronous)
    {
        // Make synchronous commit
        TRAP(err, pApaSession->CommitNonNativeApplicationsUpdatesL());
        if (KErrNone == err)
        {
            pApaSession->Close();
            delete pApaSession;
        }
        return err;
    }

    // Otherwise check whether the device supports asynchronous commit
    // (has a working version of new AppArc method ForceCommitNonNativeApplicationsUpdatesL)
    // Latest S60 5.0 builds contain fix for Symbian
    // defect PDEF129466 (PDEF129467 for Symbian OS 9.5).
    if (!isForceCommitSupported())
    {
        // Use old API
        TRAP(err, pApaSession->CommitNonNativeApplicationsUpdatesL());
    }
    else
    {
#ifdef __WINS__
        // Use always this synchronous commit when running in emulator
        // to make writing autotest cases easier.
        TRAP(err, pApaSession->CommitNonNativeApplicationsUpdatesL());
#else // __WINS__
        // asynchronous commit
        TRAP(err, pApaSession->ForceCommitNonNativeApplicationsUpdatesL());
#endif // __WINS__
    }
#endif // !SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

    if (KErrNone == err)
    {
        pApaSession->Close();
        delete pApaSession;
    }
    return err;
}


/**
 * Rolls back and closes native application registration session.
 *
 * @param[in] aSessionHandle
 * @return 0 or Symbian error code (negative number)
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1rollbackSession
(JNIEnv *, jclass, jint aSessionHandle)
{
    // Convert session handle to pointer.
    RApaLsSession *pApaSession =
        reinterpret_cast<RApaLsSession*>(aSessionHandle<<2);

    TInt err = KErrNone;
#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    err = pApaSession->RollbackNonNativeApplicationsUpdates();
#endif // !SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
    pApaSession->Close();
    delete pApaSession;

    return err;
}


/**
 * Tells whether the uid is in use in AppArc.
 * Calls AppArc RApaLsSession::GetAppType
 *
 * @param[in] aSessionHandle
 * @param[in] aUid
 * @return 0 if the uid is not in use,
 *  1 if the uid is in use
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1uidInUse
(JNIEnv *, jclass, jint aSessionHandle, jint aUid)
{
    // Convert session handle to pointer.
    RApaLsSession *pApaSession =
        reinterpret_cast<RApaLsSession*>(aSessionHandle<<2);

    TUid typeUid;
    TUid appUid;
    appUid.iUid = aUid;
    TInt err = pApaSession->GetAppType(typeUid, appUid);
    if (KErrNone == err)
    {
        // AppArc knows the app type -> the uid is in use
        return 1;
    }
    else
    {
        return 0;
    }
}


/**
 * Returns the logical group name of the application.
 * Calls AppArc RApaLsSession::GetAppCapability
 *
 * @param[in] aSessionHandle
 * @param[in] aUid
 * @param[out] aGroupName should be "" when called, will contain group name
 *  when function returns if call was successfull.
 *  Note that max group name len is 16.
 * @return Symbian error code (negative number) if fails, otherwise 0
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1getGroupName
(JNIEnv *aEnv, jclass aClass, jint aSessionHandle, jint aUid, jobject aGroupName)
{
    if (0 == aGroupName)
    {
        // Cannot return logical group name because StringBuffer param is null
        return KErrArgument;
    }

    // Convert session handle to pointer.
    RApaLsSession *pApaSession =
        reinterpret_cast<RApaLsSession*>(aSessionHandle<<2);

    TInt    err;
    TUid    midletUid;
    TApaAppCapabilityBuf cap;

    midletUid.iUid = aUid;

    err = pApaSession->GetAppCapability(cap, midletUid);
    if (KErrNone != err)
    {
        return err;
    }

    // The name is available in Unicode format from cap, convert it to UTF8
    // Then lenght of the buffer has '+1' to reserve space for zero terminator
    // needed to convert string to Java string
    TBuf8<KApaMaxAppGroupName+1> nameBuf;   // max group name len is 16
    err = CnvUtfConverter::ConvertFromUnicodeToUtf8(nameBuf, cap().iGroupName);
    if (KErrNone != err)
    {
        return err;
    }

    // Get class handle to StringBuffer
    aClass = aEnv->GetObjectClass(aGroupName);

    // Get method ID to StringBuffer StringBuffer.append(String)
    jmethodID methodID = aEnv->GetMethodID(
                             aClass,
                             "append",
                             "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
    if (0 == methodID)
    {
        return KErrGeneral;
    }

    // Create new Java String
    jstring logicalName = aEnv->NewStringUTF((const char*)(nameBuf.PtrZ()));

    // Append it to aGroupName parameter
    aEnv->CallObjectMethod(aGroupName, methodID, logicalName);

    return 0;
}


/**
 * Closes native application registration session.
 *
 * @param[in] aSessionHandle the session to be closed
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1closeSession
(JNIEnv *, jclass, jint aSessionHandle)
{
    // Convert session handle to pointer.
    RApaLsSession *pApaSession =
        reinterpret_cast<RApaLsSession*>(aSessionHandle<<2);

    pApaSession->Close();
    delete pApaSession;
}


/**
 * Converts icon to S60 specific format. SVG icons are converted to MIF,
 * other icons to MBM.
 *
 * @param[in] aInputIconFilename file name of the original icon inside .jar
 * @param[in] aOutputIconFilename file name for converted (output) icon
 * @param[in] aJarFilename jar file name. In S60 the icon is always
 *   inside the .jar file
 * @param[out] aIconSuffix the correct suffix of the icon is returned through
 *    this parameter, will contain '.mbm' or '.mif' when function returns
 * @return true if the conversion succeeds
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1convertIcon
(JNIEnv *aEnv, jclass aClass, jstring aInputIconFilename,
 jstring aOutputIconFilename, jstring aJarFilename, jobject aIconSuffix)
{
    jboolean conversionSucceeded = JNI_TRUE;
    RFs      fileServ;

    // IconConverter requires open RFile handle
    TInt err = fileServ.Connect();
    if (KErrNone != err)
    {
        // Cannot convert icons without file server connection
        ELOG1(EJavaInstaller,
              "_1convertIcon: Cannot open RFs connection, err code is %d ", err);
        return err;
    }

    // Copy the unicode characters in jstrings to Symbian buffers so that
    // they can be used in S60 code
    const jchar *iconFileName = aEnv->GetStringChars(aInputIconFilename, 0);
    if (NULL == iconFileName)
    {
        ELOG(
            EJavaInstaller,
            "Cannot convert icon, getting icon file name as unicode failed in JNI");
        fileServ.Close();
        return JNI_FALSE;
    }
    HBufC16 *unicodeIconFileName = HBufC16::New(aEnv->GetStringLength(aInputIconFilename) + 1);
    if (NULL == unicodeIconFileName)
    {
        fileServ.Close();
        return JNI_FALSE;
    }
    *unicodeIconFileName = iconFileName;
    aEnv->ReleaseStringChars(aInputIconFilename, iconFileName);

    const jchar *jarFileName = aEnv->GetStringChars(aJarFilename, 0);
    if (NULL == jarFileName)
    {
        ELOG(
            EJavaInstaller,
            "Cannot convert icon, getting jar file name as unicode failed in JNI");
        delete unicodeIconFileName;
        fileServ.Close();
        return JNI_FALSE;
    }
    HBufC16 *unicodeJarFileName = HBufC16::New(aEnv->GetStringLength(aJarFilename) + 1);
    if (NULL == unicodeJarFileName)
    {
        delete unicodeIconFileName;
        fileServ.Close();
        return JNI_FALSE;
    }
    *unicodeJarFileName = jarFileName;
    aEnv->ReleaseStringChars(aJarFilename, jarFileName);

    const jchar *outputFileName = aEnv->GetStringChars(aOutputIconFilename, 0);
    if (NULL == outputFileName)
    {
        ELOG(
            EJavaInstaller,
            "Cannot convert icon, getting output file name as unicode failed in JNI");
        delete unicodeIconFileName;
        delete unicodeJarFileName;
        fileServ.Close();
        return JNI_FALSE;
    }
    HBufC16 *unicodeOutputFileName = HBufC16::New(aEnv->GetStringLength(aOutputIconFilename) + 1);
    if (NULL == unicodeOutputFileName)
    {
        delete unicodeIconFileName;
        delete unicodeJarFileName;
        fileServ.Close();
        return JNI_FALSE;
    }
    *unicodeOutputFileName = outputFileName;
    aEnv->ReleaseStringChars(aOutputIconFilename, outputFileName);

    CIconConverter *pConv = NULL;
    TRAP(err, pConv= CIconConverter::NewL(fileServ));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller, "Cannot create icon converter, error %d ", err);
        delete unicodeIconFileName;
        delete unicodeJarFileName;
        delete unicodeOutputFileName;
        fileServ.Close();
        return JNI_FALSE;
    }

    // read icon from .jar file, convert it to MIF/MBM and store to file
    TBool isMbmIcon = EFalse;
    err = pConv->Convert(*unicodeJarFileName, *unicodeIconFileName,
                         *unicodeOutputFileName, &isMbmIcon);
    delete pConv;
    if (KErrNone != err)
    {
        WLOG1(EJavaInstaller,
              "Cannot convert icon, err code is %d.",
              err);
        conversionSucceeded = JNI_FALSE;
    }

    // Free all Symbian buffers containing copies of jstring data
    delete unicodeIconFileName;
    delete unicodeJarFileName;
    delete unicodeOutputFileName;
    fileServ.Close();

    // set the value of aIconSuffix to '.mbm' or '.mif'
    if (0 == aIconSuffix)
    {
        // Cannot return icon suffix name because StringBuffer param is null
        WLOG(EJavaInstaller,
             "_1convertIcon cannot return the correct icon suffix because param is null");
        return JNI_FALSE;
    }

    // Get class handle to StringBuffer
    aClass = aEnv->GetObjectClass(aIconSuffix);

    // Get method ID to StringBuffer StringBuffer.append(String)
    jmethodID methodID = aEnv->GetMethodID(
                             aClass,
                             "append",
                             "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
    if (0 == methodID)
    {
        WLOG(EJavaInstaller, "_1convertIcon: JNI: getting append method failed.");
        return JNI_FALSE;
    }

    // Create new Java String, Multibitmap file name suffix is '.mbm',
    // suffix for mif wrapped svg is '.mif'
    jstring suffix;
    if (isMbmIcon)
    {
        suffix = aEnv->NewStringUTF(".mbm");
    }
    else
    {
        suffix = aEnv->NewStringUTF(".mif");
    }

    // Append it to aIconSuffix parameter
    aEnv->CallObjectMethod(aIconSuffix, methodID, suffix);

    return conversionSucceeded;
}


/**
 * Returns true if the current device is touch screen
 * device without physical LSK and RSK
 *
 * @return true if virtual on screen keypad is needed
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1isOnDeviceKeypadNeeded
(JNIEnv *, jclass)
{
    // Return TRUE -> OnScreenKeypad attribute will be given
    // default value if the attribute has not been defined in
    // .jad file or manifest.
    // The runtime code will check whether the device has touch
    // screen using AknLayoutUtils::PenEnabled() and show the
    // keypad only if needed.
    return JNI_TRUE;
}


/**
 * Sends a process to process notification to the parent process
 * of Java Installer telling that Installer is about to start
 * displaying UI dialogs.
 */
JNIEXPORT void JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1notifyLauncherThatUiIsReady
(JNIEnv *, jclass)
{
    RProcess currentProcess;
    currentProcess.Rendezvous(EJavaInstaller);

    return;
}


/**
 * Returns true if the code is executed in Symbian 9.2
 * emulator environment.
 *
 * @return if Symbian 9.2 emulator environment
 */
JNIEXPORT jboolean JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1runningIn92Emulator
(JNIEnv *, jclass)
{
#if !defined(RD_JAVA_S60_RELEASE_5_0_IAD) && defined(__WINS__)
    return JNI_TRUE;
#else
    return JNI_FALSE;
#endif
}


#ifndef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
/**
 * Internal helper method for checking whether this code is executing in
 * a device that has correctly working version of
 * RApaLsSession::ForceCommitNonNativeApplicationsUpdatesL()
 *
 * The device family based on Nokia XpressMusic5800 did not have
 * working version of force commit
 *
 * @return ETrue if current device does not belong to Nokia XpressMusic5800
 *  device family
 */
TBool isForceCommitSupported()
{
    TInt err;
    TInt value;

    err = HAL::Get(HAL::EModel, value);
    if (KErrNone == err)
    {
        LOG1(EJavaInstaller, EInfo, "Device model is %d", value);
    }
    else
    {
        WLOG1(EJavaInstaller, "Cannot get device model id, error %d", err);
        // It is safest to assume that force commit is not supported
        return EFalse;
    }

    // Check against all current (28. Aug 2009) members of the device family.
    // Note that several closely related devices share the same model id.
    if ((0x2000DA56 != value) &&
            (0x2001DE9D != value) &&
            (0x2000DA60 != value) &&
            (0x20023765 != value))
    {
        // The device does not belong to Nokia 5800 XpressMusic device family,
        // so it supports foce commit
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}
#endif // !SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

/**
 * See JNI method __1logComponent.
 * This method makes calls that may leave.
 */
#ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
void AddInstallLogEntryL(
    JNIEnv */*aEnv*/, jint /*aAction*/, jint /*aUid*/,
    jstring /*aName*/, jstring /*aVendor*/,
    jint /*aMajorVersion*/, jint /*aMinorVersion*/, jint /*aMicroVersion*/)
{
}
#else
void AddInstallLogEntryL(
    JNIEnv *aEnv, jint aAction, jint aUid, jstring aName, jstring aVendor,
    jint aMajorVersion, jint aMinorVersion, jint aMicroVersion)
{
    TUid uid = TUid::Uid(aUid);
    HBufC *name = CreateHBufCFromJavaStringLC(aEnv, aName);
    HBufC *vendor = CreateHBufCFromJavaStringLC(aEnv, aVendor);
    TVersion version(aMajorVersion, aMinorVersion, aMicroVersion);

    // Create log task object.
    CTask* task = CTask::NewL(KLogTaskImplUid, EFalse);
    CleanupStack::PushL(task);

    // Initalize log task parameters.
    TLogTaskParam params;
    params.iName.Copy(name->Left(KMaxLogNameLength));
    params.iVendor.Copy(vendor->Left(KMaxLogVendorLength));
    params.iVersion = version;
    params.iUid = uid;
    // Time must be universal time.
    TTime time;
    time.UniversalTime();
    params.iTime = time;
    params.iAction = (TLogTaskAction)aAction;
    params.iIsStartup = EFalse; // Startup list was not modified.

    TLogTaskParamPckg pckg(params);
    task->SetParameterL(pckg, 0);

    // Create log task manager.
    CTaskManager* taskManager = CTaskManager::NewL();
    CleanupStack::PushL(taskManager);

    // Add the log task to the task list.
    taskManager->AddTaskL(task);
    taskManager->ExecutePendingTasksL();

    // Cleanup.
    CleanupStack::PopAndDestroy(taskManager);
    CleanupStack::Pop(task);

    CleanupStack::PopAndDestroy(vendor);
    CleanupStack::PopAndDestroy(name);
}
#endif // SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK

/**
 * Adds an entry to platform installation log.
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_applicationregistrator_ApplicationRegistrator__1addInstallLogEntry
(JNIEnv *aEnv, jclass, jint aAction, jint aUid, jstring aName, jstring aVendor,
 jint aMajorVersion, jint aMinorVersion, jint aMicroVersion)
{
    TRAPD(err, AddInstallLogEntryL(
              aEnv, aAction, aUid, aName, aVendor,
              aMajorVersion, aMinorVersion, aMicroVersion));
    return err;
}
