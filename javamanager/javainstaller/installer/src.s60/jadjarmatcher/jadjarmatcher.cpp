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
* Description:  The JNI code for Java Installer component
*                JadJarMatcher.
*
*/


#include <e32std.h>
#include <f32file.h>

#include "com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher.h"
#include "jadjarmatcherscanfrominbox.h"
#include "javacommonutils.h"
#include "logger.h"

// NAMESPACE DECLARATION
using namespace java;

/**
 * See JNI methods __1findJadFilesFromInbox, __1findJarFilesFromInbox.
 * This is just internal wrapper method for handling Leaves.
 */
jint FindFilesFromInboxL
(JNIEnv *aEnv, jclass, jstring aStartFilename, jobject aFiles, jobject aHandles, jobject aTimestamps, TBool aJarScan)
{
    LOG1(EJavaInstaller, EInfo, "FindFilesFromInboxL: aJarScan %d", aJarScan);

    RArray<RFile> resultArray;
    CJadJarMatcherScanFromInbox* scanInbox =
        new(ELeave) CJadJarMatcherScanFromInbox();
    CleanupStack::PushL(scanInbox);

    // Copy the characters in jstrings to Symbian descriptors so that
    // they can be used in S60 code.
    const jchar* startFilename = aEnv->GetStringChars(aStartFilename, 0);
    if (NULL == startFilename)
    {
        ELOG(EJavaInstaller,
             "Cannot scan Inbox, getting filename failed in JNI");
        return KErrArgument;
    }
    HBufC16* unicodeStartFilename =
        HBufC16::New(aEnv->GetStringLength(aStartFilename) + 1);
    if (NULL == unicodeStartFilename)
    {
        ELOG(EJavaInstaller,
             "Cannot scan inbox, filename allocation failed in JNI");
        return KErrNoMemory;
    }
    CleanupStack::PushL(unicodeStartFilename);
    *unicodeStartFilename = startFilename;
    aEnv->ReleaseStringChars(aStartFilename, startFilename);

    // Start Inbox scan.
    TRAPD(err, scanInbox->ScanFromInboxL(
              unicodeStartFilename, resultArray, aJarScan));
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "FindFilesFromInboxL failed with code %d", err);
    }
    else
    {
        LOG1(EJavaInstaller, EInfo,
             "FindFilesFromInboxL returns %d matches", resultArray.Count());
        jclass classFiles = aEnv->GetObjectClass(aFiles);
        jmethodID methodFilesAddElement = aEnv->GetMethodID(
                                              classFiles, "addElement", "(Ljava/lang/Object;)V");
        jclass classHandles = aEnv->GetObjectClass(aHandles);
        jmethodID methodHandlesAddElement = aEnv->GetMethodID(
                                                classHandles, "addElement", "(Ljava/lang/Object;)V");
        jclass classTimestamps = aEnv->GetObjectClass(aTimestamps);
        jmethodID methodTimestampsAddElement = aEnv->GetMethodID(
                                                   classTimestamps, "addElement", "(Ljava/lang/Object;)V");
        jclass integerClass = aEnv->FindClass("java/lang/Integer");
        jmethodID integerConstructor = aEnv->GetMethodID(
                                           integerClass, "<init>", "(I)V");
        jclass longClass = aEnv->FindClass("java/lang/Long");
        jmethodID longConstructor = aEnv->GetMethodID(
                                        longClass, "<init>", "(J)V");
        for (TInt i = 0; i < resultArray.Count(); i++)
        {
            // Add results to Java side Vector (aFiles parameter).
            TFileName fileName;
            resultArray[i].FullName(fileName);
            jstring jFileName = aEnv->NewString(
                                    (const unsigned short*)(fileName.Ptr()), fileName.Length());
            aEnv->CallVoidMethod(aFiles, methodFilesAddElement, jFileName);
            // Add results to Java side Vector (aHandles parameter).
            // Caller must close the file handles returned in resultArray.
            jobject handle = aEnv->NewObject(
                                 integerClass, integerConstructor, &resultArray[i]);
            aEnv->CallVoidMethod(aHandles, methodHandlesAddElement, handle);
            // Add results to Java side Vector (aTimestamps parameter).
            TInt64 time = 0;
            TTime ttime;
            TInt timeErr = resultArray[i].Modified(ttime);
            if (KErrNone == timeErr)
            {
                time = ttime.Int64();
            }
            else
            {
                ELOG1(EJavaInstaller,
                      "FindFilesFromInboxL failed to get file timestamp, err=%d",
                      timeErr);
            }
            jobject timestamp = aEnv->NewObject(longClass, longConstructor, time);
            aEnv->CallVoidMethod(aTimestamps, methodTimestampsAddElement, timestamp);
        }
    }

    CleanupStack::PopAndDestroy(unicodeStartFilename);
    CleanupStack::PopAndDestroy(scanInbox);
    return err;
}

/**
 * See JNI methods __1findJadFilesFromInbox, __1findJarFilesFromInbox.
 * This is just internal wrapper method for handling Leaves.
 */
jint CopyFileL(JNIEnv *aEnv, jclass, jint aHandle, jstring aFilename)
{
    __UHEAP_MARK;
    RFile* file = reinterpret_cast<RFile*>(aHandle);

    // Copy the characters in jstrings to Symbian descriptors so that
    // they can be used in S60 code.
    const jchar* filename = aEnv->GetStringChars(aFilename, 0);
    if (NULL == filename)
    {
        ELOG(EJavaInstaller,
             "JadJarMatcher: Cannot copy file, getting filename failed in JNI");
        return KErrArgument;
    }
    HBufC16* targetFilename =
        HBufC16::New(aEnv->GetStringLength(aFilename) + 1);
    if (NULL == targetFilename)
    {
        ELOG(EJavaInstaller,
             "JadJarMatcher: Cannot copy file, filename allocation failed in JNI");
        return KErrNoMemory;
    }
    CleanupStack::PushL(targetFilename);
    *targetFilename = filename;
    aEnv->ReleaseStringChars(aFilename, filename);

    //TFileName sourceFilename;
    //file->FullName( sourceFilename );
    //LOG1WSTR( EJavaInstaller, EInfo, "Copying file %s",
    //          (wchar_t *)(sourceFilename.PtrZ()));
    //LOG1WSTR( EJavaInstaller, EInfo, "to %s",
    //          (wchar_t *)(targetFilename->Des().PtrZ()));

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    CFileMan* fileMan = CFileMan::NewL(fs);
    CleanupStack::PushL(fileMan);
    TInt err = fileMan->Copy(*file, *targetFilename);
    CleanupStack::PopAndDestroy(fileMan);
    CleanupStack::PopAndDestroy(&fs);

    CleanupStack::PopAndDestroy(targetFilename);
    __UHEAP_MARKEND;
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher
 * Method:    _findJadFilesFromInbox
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher__1findJadFilesFromInbox
(JNIEnv *aEnv, jclass aClass, jstring aJar, jobject aFiles, jobject aHandles, jobject aTimestamps)
{
    TInt result = KErrNone;
    TRAPD(err, result = FindFilesFromInboxL(aEnv, aClass, aJar, aFiles, aHandles, aTimestamps, EFalse));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher
 * Method:    _findJarFilesFromInbox
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher__1findJarFilesFromInbox
(JNIEnv *aEnv, jclass aClass, jstring aJad, jobject aFiles, jobject aHandles, jobject aTimestamps)
{
    TInt result = KErrNone;
    TRAPD(err, result = FindFilesFromInboxL(aEnv, aClass, aJad, aFiles, aHandles, aTimestamps, ETrue));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher
 * Method:    _copyFile
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher__1copyFile
(JNIEnv *aEnv, jclass aClass, jint aHandle, jstring aFilename)
{
    TInt result = KErrNone;
    TRAPD(err, result = CopyFileL(aEnv, aClass, aHandle, aFilename));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher
 * Method:    _closeFileHandles
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_jadjarmatcher_JadJarMatcher__1closeFileHandles
(JNIEnv *aEnv, jclass /*aClass*/, jintArray aHandles)
{
    TInt count = aEnv->GetArrayLength(aHandles);
    jint* handles = aEnv->GetIntArrayElements(aHandles, NULL);
    for (TInt i = 0; i < count; i++)
    {
        RFile* file = reinterpret_cast<RFile*>(handles[i]);
        file->Close();
    }
    aEnv->ReleaseIntArrayElements(aHandles, handles, 0);
    LOG1(EJavaInstaller, EInfo, "JadJarMatcher: Closed %d Inbox files", count);
    return KErrNone;
}
