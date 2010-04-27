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
*                FileRoots.
*
*/


#include <e32property.h>
#include <f32file.h>
#include <apgcli.h>
#include <sysutil.h>
#include <javauids.h>

#include "com_nokia_mj_impl_installer_utils_FileRoots.h"
#include "javacommonutils.h"
#include "javaoslayer.h"
#include "logger.h"


/**
 * In S60 5.0 and TB 9.1 OMJ is (usually) post installed to C: drive.
 */
const TInt KDefaultResourceDriveNumber = 2;

/**
 * UID of Java Installer
 */
const TUid KTUidJavaInstallerUid = { KJavaInstallerUid };


/*
 * Class:     com_nokia_mj_impl_installer_utils_FileRoots
 * Method:    _getResourceDrive
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_FileRoots__1getResourceDrive
(JNIEnv *, jclass)
{
    TInt err;
    RApaLsSession apaSession;

    err = apaSession.Connect();
    if (KErrNone != err)
    {
        ELOG(EJavaInstaller, "FileRoots__1getResourceDrive: Cannot connect to AppArc server");
        return KDefaultResourceDriveNumber;
    }

    TApaAppInfo info;
    err = apaSession.GetAppInfo(info, KTUidJavaInstallerUid);
    if (err != KErrNone)
    {
        ELOG1(EJavaInstaller,
              "FileRoots__1getResourceDrive: Cannot get full path name of installer, err %d ",
              err);
        apaSession.Close();
        return KDefaultResourceDriveNumber;
    }

    // Take the drive letter from path and convert it to drive number
    TInt driveNum = KDefaultResourceDriveNumber;
    err = RFs::CharToDrive(info.iFullName[0], driveNum);
    if (KErrNone != err)
    {
        driveNum = KDefaultResourceDriveNumber;
    }

    apaSession.Close();

    return driveNum;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_FileRoots
 * Method:    _getMidpRoot
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_nokia_mj_impl_installer_utils_FileRoots__1getMidpRoot
(JNIEnv *aEnv, jclass)
{
    std::string path;
    java::util::JavaOsLayer::getMidpRoot(path);

    // Convert path to jstring and return it
    return aEnv->NewStringUTF((const char*)(path.c_str()));
}
