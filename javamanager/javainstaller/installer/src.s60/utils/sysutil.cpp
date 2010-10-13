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
*                SysUtil.
*
*/


#include <centralrepository.h>
#include <e32property.h>
#include <f32file.h>
#include <hal.h>
#include <hal_data.h>
#include <sysutil.h>
#include <driveinfo.h>

#include "com_nokia_mj_impl_installer_utils_SysUtil.h"
#include "logger.h"

#ifndef KPSUidJavaLatestInstallation
#define KPSUidJavaLatestInstallation 0x10282567
#endif

/**
 * Helper method for creating HBufC from jstring.
 */
HBufC* CreateHBufCFromJavaStringLC(JNIEnv* aEnv, jstring aString)
{
    HBufC16* str = HBufC16::NewL(aEnv->GetStringLength(aString));
    CleanupStack::PushL(str);
    const TUint16* temp = aEnv->GetStringChars(aString, 0);
    str->Des() = temp;
    aEnv->ReleaseStringChars(aString, temp);
    return str;
}

/**
 * See JNI method __1getRepositoryValue.
 * This is just internal wrapper method for handling Leaves.
 */
jint GetRepositoryValueL
(JNIEnv *aEnv, jclass, jint aRepository, jlong aKey, jobject aIntValue)
{
    TUid repositoryUid = { aRepository };
    TUint key(aKey);
    TInt val(0);
    CRepository* repository = CRepository::NewLC(repositoryUid);
    TInt err = repository->Get(key, val);
    CleanupStack::PopAndDestroy(repository);
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aIntValue);
        jmethodID methodID = aEnv->GetMethodID(clazz, "setValue", "(I)V");
        if (0 == methodID)
        {
            ELOG(EJavaInstaller,
                 "SysUtil__1getRepositoryValue: Cannot find setValue method");
            err = KErrGeneral;
        }
        else
        {
            aEnv->CallVoidMethod(aIntValue, methodID, val);
        }
    }
    return err;
}

/**
 * See JNI method __1getRepositoryStringValue.
 * This is just internal wrapper method for handling Leaves.
 */
jint GetRepositoryStringValueL
(JNIEnv *aEnv, jclass, jint aRepository, jlong aKey, jobjectArray aStringArray)
{
    TUid repositoryUid = { aRepository };
    CRepository* repository = CRepository::NewLC(repositoryUid);
    RBuf buf;
    buf.Create(NCentralRepositoryConstants::KMaxUnicodeStringLength);
    CleanupClosePushL(buf);
    TInt err = repository->Get(aKey, buf) ;
    if (KErrNone == err)
    {
        jstring val = aEnv->NewString((const unsigned short*)(buf.Ptr()), buf.Length());
        if (NULL == val)
        {
            ELOG(EJavaInstaller,
                 "SysUtil__1getRepositoryStringValue: String could not be constructed");
            err = KErrGeneral;
        }
        else
        {
            aEnv->SetObjectArrayElement(aStringArray, 0, val);
        }
    }
    CleanupStack::PopAndDestroy(&buf) ;
    CleanupStack::PopAndDestroy(repository);
    return err;
}

/**
 * See JNI method __1setRepositoryValue.
 * This is just internal wrapper method for handling Leaves.
 */
jint SetRepositoryValueL
(JNIEnv *, jclass, jint aRepository, jlong aKey, jint aValue)
{
    TUid repositoryUid = { aRepository };
    TUint key(aKey);
    TInt val(aValue);
    CRepository* repository = CRepository::NewLC(repositoryUid);
    TInt err = repository->Set(key, val);
    CleanupStack::PopAndDestroy(repository);
    return err;
}

/**
 * See JNI method __1setRepositoryStringValue.
 * This is just internal wrapper method for handling Leaves.
 */
jint SetRepositoryStringValueL
(JNIEnv *aEnv, jclass, jint aRepository, jlong aKey, jstring aValue)
{
    TUid repositoryUid = { aRepository };
    TUint key(aKey);

    // Copy the characters in jstrings to Symbian descriptors so that
    // they can be used in S60 code.
    const jchar* value = aEnv->GetStringChars(aValue, 0);
    if (NULL == value)
    {
        ELOG(EJavaInstaller,
             "SysUtil__1setRepositoryStringValue: getting value failed in JNI");
        return KErrArgument;
    }
    HBufC16* valueDes = HBufC16::New(aEnv->GetStringLength(aValue) + 1);
    if (NULL == valueDes)
    {
        ELOG(EJavaInstaller,
             "SysUtil__1setRepositoryStringValue: value allocation failed in JNI");
        return KErrNoMemory;
    }
    CleanupStack::PushL(valueDes);
    *valueDes = value;
    aEnv->ReleaseStringChars(aValue, value);

    // Update repository value.
    CRepository* repository = CRepository::NewLC(repositoryUid);
    TInt err = repository->Set(key, *valueDes);

    CleanupStack::PopAndDestroy(repository);
    CleanupStack::PopAndDestroy(valueDes);

    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _defineProperty
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1defineProperty
(JNIEnv *, jclass, jint aCategory, jlong aKey)
{
    _LIT_SECURITY_POLICY_PASS(KReadPolicy);
    // update allowed by processes with write system data capability
    _LIT_SECURITY_POLICY_C1(KWritePolicy, ECapabilityWriteDeviceData);

    TUid categoryUid = { aCategory };
    TInt err = RProperty::Define(categoryUid, aKey, RProperty::EInt,
                                 KReadPolicy, KWritePolicy);
    if (KErrAlreadyExists == err)
    {
        err = KErrNone; // Ignore KErrAlreadyExists status.
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _deleteProperty
 * Signature: (IJ)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1deleteProperty
(JNIEnv *, jclass, jint aCategory, jlong aKey)
{
    TUid categoryUid = { aCategory };
    TInt err = RProperty::Delete(categoryUid, aKey);
    if (KErrNotFound == err)
    {
        err = KErrNone; // Ignore KErrNotFound status.
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getPropertyValue
 * Signature: (IJLcom/nokia/mj/impl/installer/utils/SysUtil/IntValue;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getPropertyValue
(JNIEnv *aEnv, jclass, jint aCategory, jlong aKey, jobject aIntValue)
{
    TUid categoryUid = { aCategory };
    TUint key(aKey);
    TInt val(0);
    TInt err = RProperty::Get(categoryUid, key, val);
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aIntValue);
        jmethodID methodID = aEnv->GetMethodID(clazz, "setValue", "(I)V");
        if (0 == methodID)
        {
            ELOG(EJavaInstaller,
                 "SysUtil__1getPropertyValue: Cannot find setValue method");
            return KErrGeneral;
        }
        aEnv->CallVoidMethod(aIntValue, methodID, val);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _setPropertyValue
 * Signature: (IJI)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1setPropertyValue
(JNIEnv *, jclass, jint aCategory, jlong aKey, jint aValue)
{
    TUid categoryUid = { aCategory };
    TUint key(aKey);
    TInt val(aValue);
    TInt err = RProperty::Set(categoryUid, key, val);
    if ((KErrPermissionDenied == err) && (KPSUidJavaLatestInstallation == key))
    {
        // Just log this error to allow running installer when OMJ itself is
        // installed to the device, the native installer is still running and
        // existing S60 midlets must be converted to OMJ
        WLOG(EJavaInstaller,
             "SysUtil__1setPropertyValue: Cannot set value because the old S60 "
             "installer still owns the PS key");
        err = KErrNone;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getRepositoryValue
 * Signature: (IJLcom/nokia/mj/impl/installer/utils/SysUtil/IntValue;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getRepositoryValue
(JNIEnv *aEnv, jclass aClass, jint aRepository, jlong aKey, jobject aIntValue)
{
    TInt result = KErrNone;
    TRAPD(err, result = GetRepositoryValueL(aEnv, aClass, aRepository, aKey, aIntValue));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getRepositoryStringValue
 * Signature: (IJ[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getRepositoryStringValue
(JNIEnv *aEnv, jclass aClass, jint aRepository, jlong aKey, jobjectArray aStringArray)
{
    TInt result = KErrNone;
    TRAPD(err, result = GetRepositoryStringValueL(aEnv, aClass, aRepository, aKey, aStringArray));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _setRepositoryValue
 * Signature: (IJI)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1setRepositoryValue
(JNIEnv *aEnv, jclass aClass, jint aRepository, jlong aKey, jint aValue)
{
    TInt result = KErrNone;
    TRAPD(err, result = SetRepositoryValueL(aEnv, aClass, aRepository, aKey, aValue));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _setRepositoryStringValue
 * Signature: (IJ[Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1setRepositoryStringValue
(JNIEnv *aEnv, jclass aClass, jint aRepository, jlong aKey, jstring aValue)
{
    TInt result = KErrNone;
    TRAPD(err, result = SetRepositoryStringValueL(aEnv, aClass, aRepository, aKey, aValue));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getDriveUniqId
 * Signature: (ILcom/nokia/mj/impl/installer/utils/SysUtil/IntValue;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getDriveUniqId
(JNIEnv *aEnv, jclass, jint aDrive, jobject aIntValue)
{
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        return err;
    }
    TVolumeInfo volumeInfo;
    err = fs.Volume(volumeInfo, aDrive);
    fs.Close();
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aIntValue);
        jmethodID methodID = aEnv->GetMethodID(clazz, "setValue", "(I)V");
        if (0 == methodID)
        {
            ELOG(EJavaInstaller,
                 "SysUtil__1getDriveUniqId: Cannot find setValue method");
            return KErrGeneral;
        }
        aEnv->CallVoidMethod(aIntValue, methodID, volumeInfo.iUniqueID);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _isDrivePresent
 * Signature: (ILcom/nokia/mj/impl/installer/utils/SysUtil/BooleanValue;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1isDrivePresent
(JNIEnv *aEnv, jclass, jint aDrive, jobject aBooleanValue)
{
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        return err;
    }
    TDriveInfo driveInfo;
    err = fs.Drive(driveInfo, aDrive);
    fs.Close();

    TBool drivePresent = ETrue;
    if (KErrNone == err)
    {
        drivePresent = (driveInfo.iType != EMediaNotPresent);
    }
    else if (KErrNotReady == err || KErrBadName == err)
    {
        drivePresent = EFalse;
        err = KErrNone;
    }

    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aBooleanValue);
        jmethodID methodID = aEnv->GetMethodID(clazz, "setValue", "(Z)V");
        if (0 == methodID)
        {
            ELOG(EJavaInstaller,
                 "SysUtil__1isDrivePresent: Cannot find setValue method");
            return KErrGeneral;
        }
        aEnv->CallVoidMethod(aBooleanValue, methodID, drivePresent);
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _isDriveReadOnly
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1isDriveReadOnly
(JNIEnv *, jclass, jint aDrive)
{
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        return err;
    }

    TUint driveStatus = 0;
    err = DriveInfo::GetDriveStatus(fs, aDrive, driveStatus);
    fs.Close();
    if (err < 0)
    {
        return err;
    }

    if (driveStatus & DriveInfo::EDriveReadOnly)
    {
        return 1;
    }
    else if (driveStatus & DriveInfo::EDriveRom)
    {
        return 1;
    }

    return 0;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _isDiskSpaceBelowCriticalLevel
 * Signature: (ILcom/nokia/mj/impl/installer/utils/SysUtil/BooleanValue;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1isDiskSpaceBelowCriticalLevel
(JNIEnv *aEnv, jclass, jint aBytesToWrite, jint aDrive, jobject aBooleanValue)
{
    TBool result = false;
    TRAPD(err, result = SysUtil::DiskSpaceBelowCriticalLevelL(NULL, aBytesToWrite, aDrive));
    if (KErrNone == err)
    {
        jclass clazz = aEnv->GetObjectClass(aBooleanValue);
        jmethodID methodID = aEnv->GetMethodID(clazz, "setValue", "(Z)V");
        if (0 == methodID)
        {
            ELOG(EJavaInstaller,
                 "SysUtil__1isDiskSpaceBelowCriticalLevel: Cannot find setValue method");
            return KErrGeneral;
        }
        aEnv->CallVoidMethod(aBooleanValue, methodID, result);
    }
    return err;
}

_LIT(KProcessName, "*");
/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getProcessState
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getProcessState
(JNIEnv *, jclass, jint aUid)
{
    TInt result = KErrNone;
    RProcess proc;
    TFindProcess procFinder(KProcessName); // search for "*"
    TFullName procName;
    while (procFinder.Next(procName) == KErrNone)
    {
        if (proc.Open(procFinder) != KErrNone)
        {
            continue;
        }
        if (proc.SecureId() == aUid)
        {
            if (proc.ExitType() == EExitPending)
            {
                LOG1(EJavaInstaller, EInfo,
                     "SysUtil__1getProcessState: Found process [%x]", aUid);
                result = 1; // PROC_STATE_ALIVE
                proc.Close();
                break;
            }
        }
        proc.Close();
    }
    return result;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getDefaultPhoneMemory
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getDefaultPhoneMemory
(JNIEnv *, jclass)
{
    TInt drive;
    TInt err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultPhoneMemory, drive);
    if (err < 0)
    {
        // In case of error return Symbian error code
        return err;
    }
    else
    {
        // else return the number of the drive
        return drive;
    }
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getDefaultMassStorage
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getDefaultMassStorage
(JNIEnv *, jclass)
{
    TInt drive;
    TInt err = DriveInfo::GetDefaultDrive(DriveInfo::EDefaultMassStorage, drive);
    if (err < 0)
    {
        // In case of error return Symbian error code
        return err;
    }
    else
    {
        // else return the number of the drive
        return drive;
    }
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getUserVisibleDrives
 * Signature: (Ljava/util/Vector;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getUserVisibleDrives
(JNIEnv *aEnv, jclass aClass, jobject aVisibleDrives)
{
    // Getting the list of user visible drives requires file server connection and
    // unfortunately we must create new one
    RFs fs;
    TInt err = fs.Connect();
    if (KErrNone != err)
    {
        ELOG1(EJavaInstaller,
              "SysUtil__1getUserVisibleDrives: Cannot connect to file server, err: %d", err);
        return err;
    }

    // The list of user visible drives
    DriveInfo::TDriveArray drives;
    err = DriveInfo::GetUserVisibleDrives(fs, drives);
    if (KErrNone != err)
    {
        fs.Close();
        return err;
    }

    TInt nCount = drives.Count();
    if (nCount == 0)
    {
        // Strange, no user visible drives
        WLOG(EJavaInstaller,
             "SysUtil__1getUserVisibleDrives: No user visible drives in system.");
        fs.Close();
        return KErrNone;
    }

    // Get status of each drive
    TUint driveStatuses[KMaxDrives];
    TInt64 driveFreeSpaces[KMaxDrives];
    TVolumeInfo volumeInfo;
    for (TInt nInd = 0; nInd < nCount; nInd++)
    {
        err = DriveInfo::GetDriveStatus(fs, drives[nInd], driveStatuses[nInd]);
        if (KErrNone != err)
        {
            WLOG2(EJavaInstaller,
                  "SysUtil__1getUserVisibleDrives: cannot get drive %d status, err %d",
                  drives[nInd], err);
            // If cannot get status of a drive, that drive will not be
            // returned to the caller
            driveStatuses[nInd] = 0;
        }
        err = fs.Volume(volumeInfo, drives[nInd]);
        if (KErrNone != err)
        {
            WLOG2(EJavaInstaller,
                  "SysUtil__1getUserVisibleDrives: cannot get drive %d size, err %d",
                  drives[nInd], err);
            driveFreeSpaces[nInd] = 0;
        }
        else
        {
            driveFreeSpaces[nInd] = volumeInfo.iFree;
        }
    }
    fs.Close();

    // Now values in TDriveArray must be returned to Java side inside the vector
    // parameter aVisibleDrives.
    // The vector contains com.nokia.mj.impl.installer.utils.DriveInfo objects

    // Get class handle to Vector
    aClass = aEnv->GetObjectClass(aVisibleDrives);

    // Get method ID to void addElement(Object)
    jmethodID methodID = aEnv->GetMethodID(
                             aClass,
                             "addElement",
                             "(Ljava/lang/Object;)V");
    if (0 == methodID)
    {
        ELOG(EJavaInstaller,
             "SysUtil__1getUserVisibleDrives: JNI cannot get Vector addElement() method");
        return KErrGeneral;
    }

    // Get reference to DriveInfo class
    jclass driveInfoClass = aEnv->FindClass("com/nokia/mj/impl/installer/utils/DriveInfo");
    if (NULL == driveInfoClass)
    {
        ELOG(EJavaInstaller,
             "SysUtil__1getUserVisibleDrives: JNI cannot get DriveInfo class");
        return KErrGeneral;
    }
    // Get reference to DriveInfo constructor (it takes two int parameters)
    jmethodID driveInfoConstructor = aEnv->GetMethodID(
                                         driveInfoClass,
                                         "<init>",
                                         "(IIJ)V");
    if (0 == driveInfoConstructor)
    {
        ELOG(EJavaInstaller,
             "SysUtil__1getUserVisibleDrives: JNI cannot get DriveInfo constructor");
        return KErrGeneral;
    }

    // Add drives to vector parameter
    for (TInt nInd = 0; nInd < nCount; nInd++)
    {
        // If the removable drive is not currently mounted
        // (e.g. MMC is not in the device), do NOT return the drive
        if (!(driveStatuses[nInd] & DriveInfo::EDrivePresent))
        {
            continue;
        }

        // If the drive substed, remote, corrupt, in exclusive use or read only,
        // do NOT return the drive
        if (driveStatuses[nInd] &
                (DriveInfo::EDriveSubsted | DriveInfo::EDriveRemote |
                 DriveInfo::EDriveCorrupt | DriveInfo::EDriveInUse |
                 DriveInfo::EDriveReadOnly))
        {
            continue;
        }

        // Create new DriveInfo object that contains the drive number
        // and the status of one of the user visible drives
        jobject driveInfoValue =
            aEnv->NewObject(driveInfoClass, driveInfoConstructor,
                            drives[nInd], driveStatuses[nInd],
                            driveFreeSpaces[nInd]);

        if (NULL == driveInfoValue)
        {
            ELOG(EJavaInstaller,
                 "SysUtil__1getUserVisibleDrives: JNI cannot construct DriveInfo object");
            return KErrGeneral;
        }

        // Append it to aVisibleDrives parameter
        aEnv->CallVoidMethod(aVisibleDrives, methodID, driveInfoValue);
    }

    return KErrNone;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getScreenWidth
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getScreenWidth
(JNIEnv *, jclass)
{
    TInt value = 0;
    TInt err = HAL::Get(HALData::EDisplayXPixels, value);
    if (KErrNone != err)
    {
        return err;
    }
    return value;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_SysUtil
 * Method:    _getScreenHeight
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_SysUtil__1getScreenHeight
(JNIEnv *, jclass)
{
    TInt value = 0;
    TInt err = HAL::Get(HALData::EDisplayYPixels, value);
    if (KErrNone != err)
    {
        return err;
    }
    return value;
}
