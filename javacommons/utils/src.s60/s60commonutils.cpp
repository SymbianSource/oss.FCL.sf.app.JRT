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
* Description:  ?Description
*
*/


#include <e32def.h>
#include <e32cmn.h>
#include <e32std.h>

#include "s60commonutils.h"
#include "jni.h"
#include <f32file.h>
#include <bautils.h>                    // BaflUtils
#include <driveinfo.h>                  // DriveInfo
#include <data_caging_path_literals.hrh> // KDC_* constant strings

using namespace java::util;

enum TJavaArrayPanic
{
    EBadOffsetIntoJavaArray,
    EWritingOverEndOfJavaArray,
    EBadOffsetIntoJavaArrayForRead,
    EReadingOverEndOfJavaArray,
};



/**
 * Copies the java buffer to native descriptor.
 * @param aJni The JNI environment.
 * @param aJavaBuffer The Java buffer to copy data from.
 * @param aOffset Start of data in Java buffer to copy. This is assumed to be valid.
 * @param aLength Amount of data to copy. This is assumed to be valid.
 * @param aNativeBuffer Target for data. This is assumed to be long enough.
 * @returns An error code.
 */
OS_EXPORT TInt S60CommonUtils::CopyToNative(JNIEnv& aJni, jbyteArray aJavaBuffer,
        TInt aOffset, TInt aLength, TDes8& aNativeBuffer)
{
    __ASSERT_DEBUG(aOffset <= aJni.GetArrayLength(aJavaBuffer),
                   User::Panic(_L("S60CommonUtils"), EBadOffsetIntoJavaArrayForRead));
    __ASSERT_DEBUG(aLength <= aJni.GetArrayLength(aJavaBuffer) - aOffset,
                   User::Panic(_L("S60CommonUtils"), EReadingOverEndOfJavaArray));

    aNativeBuffer.SetLength(aLength);
    TUint8* nativeBufPtr = const_cast<TUint8*>(aNativeBuffer.Ptr());
    jbyte* jNativeBufPtr = reinterpret_cast<jbyte*>(nativeBufPtr);
    aJni.GetByteArrayRegion(aJavaBuffer, aOffset, aLength, jNativeBufPtr);
    return KErrNone;
}



/**
 * Copies data from the native to the Java array.
 * @return The number of bytes copied.
 */
OS_EXPORT TInt S60CommonUtils::CopyToJava(JNIEnv& aJni, const TDesC8& aNativeBuffer,
        jbyteArray aJavaBuffer, TInt aOffset, TInt aLength)
{
    __ASSERT_DEBUG(aOffset <= aJni.GetArrayLength(aJavaBuffer),
                   User::Panic(_L("S60CommonUtils"), EBadOffsetIntoJavaArray));
    __ASSERT_DEBUG(aLength <= aJni.GetArrayLength(aJavaBuffer) - aOffset,
                   User::Panic(_L("S60CommonUtils"), EWritingOverEndOfJavaArray));

    TInt nativeBufLength = aNativeBuffer.Length();
    TInt length = (nativeBufLength < aLength) ? nativeBufLength : aLength;
    TUint8* nativeBufPtr = const_cast<TUint8*>(aNativeBuffer.Ptr());
    jbyte* jNativeBufPtr = reinterpret_cast<jbyte*>(nativeBufPtr);
    aJni.SetByteArrayRegion(aJavaBuffer, aOffset, length, jNativeBufPtr);
    return length;
}

OS_EXPORT jstring S60CommonUtils::NativeToJavaString(JNIEnv& aJni, const TDesC16& aNativeString)
{
    const jchar* ptr = aNativeString.Ptr();
    const jsize len = aNativeString.Length();
    jstring javaString = aJni.NewString(ptr, len);
    return javaString;
}

/**
 * Creates a Java array of strings from a native array of descriptors allocated
 * on the heap.
 * @param aJni The JNI environment.
 * @param aNativeArray The array of descriptors.
 * @return The newly created Java array of String objects, or NULL on error.
 */
OS_EXPORT jobjectArray S60CommonUtils::NativeToJavaStringArray(JNIEnv& aJni,
        const RPointerArray<HBufC>& aNativeArray)
{
    jclass stringClass = aJni.FindClass("java/lang/String");
    if (stringClass == NULL)
    {
        return NULL;
    }

    TInt count = aNativeArray.Count();
    jobjectArray jObjArray = aJni.NewObjectArray(count, stringClass, NULL);
    if (jObjArray == NULL)
    {
        return NULL;
    }

    for (int i = 0; i< count; i++)
    {
         if( !aNativeArray[ i ] )
         {
             // Skip null elements since already initialized
             continue;
         }
    	
        //TPtr16 temp =  aNativeArray[i]->Des();
        jstring javaString = S60CommonUtils::NativeToJavaString(aJni, *aNativeArray[i]);
        if (javaString == NULL)
        {
            aJni.DeleteLocalRef(jObjArray);
            return NULL;
        }

        aJni.SetObjectArrayElement(jObjArray, i, javaString);
        aJni.DeleteLocalRef(javaString);
    }

    return jObjArray;
}

/* Converts wchar_t to descriptor */
OS_EXPORT HBufC* S60CommonUtils::wstringToDes(const wchar_t* cStr)
{
    HBufC* descStr = NULL;
    int length = wcslen(cStr);
    if (length > 0)
    {
        descStr = HBufC::New(length + 1);
        if (descStr == NULL)
        {
            return descStr;
        }
        TPtr ptr = descStr->Des();
        TPtr ptr16((TUint16*)cStr,length);
        ptr16.SetLength(length);
        ptr.Copy(ptr16);
        ptr.ZeroTerminate();
    }
    return descStr;
}

OS_EXPORT TTime S60CommonUtils::JavaTimeToTTime(jlong aJavaTime)
{
    // Convert jlong to a TInt64
    TInt64 milliSeconds = *reinterpret_cast<TInt64*>(&aJavaTime);
    // Create a TTime object that represents the Java Date 'epoch' time of 00:00, 1 Jan 1970
    TInt64 javaEpocTimeNum = MAKE_TINT64(JavaUpperEpocTime, JavaLowerEpocTime);
    TTime time(javaEpocTimeNum);
    TTimeIntervalMicroSeconds timeInterval(milliSeconds * 1000);
    return time + timeInterval;
}

OS_EXPORT jlong S60CommonUtils::TTimeToJavaTime(TTime aEpocTime)
{
    // Create a TTime object that represents the Java Date 'epoch' time of 00:00, 1 Jan 1970
    TInt64 javaEpocTimeNum = MAKE_TINT64(JavaUpperEpocTime, JavaLowerEpocTime);
    TTime javaEpochTime(javaEpocTimeNum);
    // Find difference in microseconds between 'epoch' and EPOC date and adjust to milliseconds
    TTimeIntervalMicroSeconds microInterval = aEpocTime.MicroSecondsFrom(javaEpochTime);
    TInt64 timeInterval = microInterval.Int64();
    timeInterval = timeInterval/1000;
    jlong javaTimeInterval = *reinterpret_cast<jlong*>(&timeInterval);
    return javaTimeInterval;
}


//------------------------------------------------------------------------------
// public static ConvertWiderToNarrowL(const TDesC& aSource,TDesC8& aDestination)
//
//------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::ConvertWiderToNarrowLC(const TDesC& aSource,
        TDesC8*& aDestination)
{
    TBool value = (!&aSource) || (aSource.Length() < 1);
    if (value)
    {
        // we are poping in NON CleanupStack version.
        CleanupStack::PushL(aDestination);
        return 0;
    }

    HBufC8* buffer = HBufC8::NewLC(aSource.Length());
    TPtr8 ptr = buffer->Des();
    ptr.Copy(aSource);
    aDestination = buffer;

    return aDestination->Length();
}
//------------------------------------------------------------------------------
// public static ConvertWiderToNarrowL(const TDesC& aSource,TDesC8& aDestination)
// DO NOT LEAVE IT ON THE CLEANUP STACK
//------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::ConvertWiderToNarrowL(const TDesC& aSource,
        TDesC8*& aDestination)
{
    ConvertWiderToNarrowLC(aSource, aDestination);
    CleanupStack::Pop(aDestination); //aDestination

    if (aDestination)
        return aDestination->Length();
    else
        return 0;
}
//------------------------------------------------------------------------------
// public static ConvertNarrowToWiderL(const TDesC& aSource,TDesC8& aDestination)
// Leave the TDesC on cleanup stack
//------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::ConvertNarrowToWiderLC(const TDesC8& aSource,
        TDesC*& aDestination)
{
    TBool value = (!&aSource) || (aSource.Length() < 1);
    if (value)
    {
        // we are poping in NON CleanupStack version.
        CleanupStack::PushL(aDestination);
        return 0;
    }
    HBufC16* buffer = HBufC16::NewLC(aSource.Length());
    TPtr16 ptr = buffer->Des();
    ptr.Copy(aSource);
    aDestination = buffer;

    return aDestination->Length();
}
//------------------------------------------------------------------------------
// public static ConvertNarrowToWiderL(const TDesC& aSource,TDesC8& aDestination)
// DO NOT LEAVE IT ON THE CLEANUP STACK
//------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::ConvertNarrowToWiderL(const TDesC8& aSource,
        TDesC*& aDestination)
{
    ConvertNarrowToWiderLC(aSource, aDestination);
    CleanupStack::Pop(aDestination); //aDestination

    if (aDestination)
        return aDestination->Length();
    else
        return 0;
}
//------------------------------------------------------------------------------
// public static ConvertNarrowToWiderL(const TDesC& aSource,TDesC8& aDestination)
// Leave the TDesC on cleanup stack
//------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::CopyNarrowLC(const TDesC8& aSource,
        TDesC8*& aDestination)
{
    TBool value = (!&aSource) || (aSource.Length() < 1);
    if (value)
    {
        // we are poping in NON CleanupStack version.
        CleanupStack::PushL(aDestination);
        return 0;
    }
    HBufC8* buffer = HBufC8::NewLC(aSource.Length());
    TPtr8 ptr = buffer->Des();
    ptr.Copy(aSource);
    aDestination = buffer;

    return aDestination->Length();
}
//-------------------------------------------------------------------------------
// public static ConvertNarrowToWiderL(const TDesC& aSource,TDesC8& aDestination)
// DO NOT LEAVE IT ON THE CLEANUP STACK
//-------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::CopyNarrowL(const TDesC8& aSource,
        TDesC8*& aDestination)
{
    CopyNarrowLC(aSource, aDestination);
    CleanupStack::Pop(aDestination); //aDestination
    if (aDestination)
        return aDestination->Length();
    else
        return 0;
}
//------------------------------------------------------------------------------
// public static ConvertNarrowToWiderL(const TDesC& aSource,TDesC8& aDestination)
// Leave the TDesC on cleanup stack
//------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::CopyWiderLC(const TDesC16& aSource,
        TDesC16*& aDestination)
{
    TBool value = (!&aSource) || (aSource.Length() < 1);
    if (value)
    {
        // we are poping in NON CleanupStack version.
        CleanupStack::PushL(aDestination);
        return 0;
    }
    HBufC16* buffer = HBufC16::NewLC(aSource.Length());
    TPtr16 ptr = buffer->Des();
    ptr.Copy(aSource);
    aDestination = buffer;

    return aDestination->Length();
}
//------------------------------------------------------------------------------
// public static ConvertNarrowToWiderL(const TDesC& aSource,TDesC8& aDestination)
// DO NOT LEAVE IT ON THE CLEANUP STACK
//------------------------------------------------------------------------------
OS_EXPORT TInt S60CommonUtils::CopyWiderL(const TDesC16& aSource,
        TDesC16*& aDestination)
{
    CopyWiderLC(aSource, aDestination);
    CleanupStack::Pop(aDestination); //aDestination
    if (aDestination)
        return aDestination->Length();
    else
        return 0;
}

OS_EXPORT TFileName S60CommonUtils::ResourceLanguageFileNameL(const TDesC& aResourceFileName)
{
    // get the name of the running process
    TFileName* dllFileName = new(ELeave) TFileName;
    CleanupStack::PushL(dllFileName);
    Dll::FileName(*dllFileName);

    // create the full path without drive letter
    TFileName* tmp = new(ELeave) TFileName;
    CleanupStack::PushL(tmp);
    tmp->Append(KDC_RESOURCE_FILES_DIR);
    tmp->Append(_L("java\\"));
    tmp->Append(aResourceFileName);

    // add the drive letter of the DLL's file name
    TParse p;
    p.Set(*tmp, dllFileName, NULL);
    CleanupStack::PopAndDestroy(tmp);
    CleanupStack::PopAndDestroy(dllFileName);

    // check if the resource exists
    TFileName resourceFullName = p.FullName();
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    BaflUtils::NearestLanguageFile(fs, resourceFullName);
    if (!BaflUtils::FileExists(fs, resourceFullName))
    {
        // try the ROM drive
        TChar romDriveLetter;
        User::LeaveIfError(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRom, romDriveLetter));
        // replace resourceFullName's drive letter with romDriveLetter
        resourceFullName[0] = romDriveLetter;
        BaflUtils::NearestLanguageFile(fs, resourceFullName);
    }
    CleanupStack::PopAndDestroy(&fs);

    // return
    return resourceFullName;
}


OS_EXPORT TFileName S60CommonUtils::VerifiedFileNameL(const TDesC& aFileName)
{
    _LIT(KColonChar, ":");

    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Does the file name specify drive ?
    // If the second char is ':' assume that the first one is drive
    TBool fileNameContainsDrive(EFalse);
    TChar originalDriveLetter(0);
    TUint attr(0);
    TInt  err(KErrNone);
    TFileName fullName = aFileName;
    if (aFileName.Find(KColonChar) == 1)
    {
        fileNameContainsDrive = ETrue;
        // Is the file in current drive?
        err = fs.Att(aFileName, attr);
        if (KErrNone == err)
        {
            CleanupStack::PopAndDestroy(&fs);
            return fullName;
        }

        // Must search from other drives. Remember the current drive;
        originalDriveLetter = aFileName[0];
        originalDriveLetter.UpperCase();
    }

    // First check system drive
    TChar driveLetter;
    User::LeaveIfError(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultSystem, driveLetter));
    // Check system drive if it has not yet been checked
    if (originalDriveLetter != driveLetter)
    {
        if (fileNameContainsDrive)
        {
            fullName[0] = driveLetter;
        }
        else
        {
            fullName.Zero();
            fullName.Append(driveLetter);
            fullName.Append(KColonChar);
            fullName.Append(aFileName);
        }

        err = fs.Att(fullName, attr);
        if (KErrNone == err)
        {
            CleanupStack::PopAndDestroy(&fs);
            return fullName;
        }
    }

    // Next try to find file from ROM if the ROM drive has not yet been checked
    User::LeaveIfError(DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRom, driveLetter));
    if (originalDriveLetter != driveLetter)
    {
        if (fileNameContainsDrive)
        {
            fullName[0] = driveLetter;
        }
        else
        {
            fullName.Zero();
            fullName.Append(driveLetter);
            fullName.Append(KColonChar);
            fullName.Append(aFileName);
        }

        err = fs.Att(fullName, attr);
    }
    User::LeaveIfError(err);

    CleanupStack::PopAndDestroy(&fs);
    return fullName;
}

