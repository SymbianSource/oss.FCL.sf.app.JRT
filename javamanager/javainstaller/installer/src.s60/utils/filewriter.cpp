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
* Description:  The JNI code for Java Installer class FileWriter.
*
*/

#include "com_nokia_mj_impl_installer_utils_FileWriter.h"
#include "javacommonutils.h"
#include "javasymbianoslayer.h" // for CleanupResetAndDestroyPushL
#include "logger.h"
#include "s60commonutils.h"

#include <caf/caf.h>
#include <caf/supplier.h>
#ifdef RD_JAVA_S60_RELEASE_5_0
#include <middleware/Oma2Agent.h>
#else
#include <caf/cafplatform.h>
#include <mw/Oma2Agent.h>
#endif // RD_JAVA_S60_RELEASE_5_0

// NAMESPACE DECLARATION
using namespace java;
using namespace java::util;
using namespace ContentAccess;

IMPORT_C HBufC* CreateHBufCFromJavaStringLC(JNIEnv* aEnv, jstring aString);

/**
 * See JNI method __1create.
 * This is an internal wrapper method for handling Leaves.
 */
jint CreateL(
    JNIEnv *aEnv, jclass, jstring aMimeType,
    jstring aOutputPath, jstring aSuggestedFilename)
{
    // Convert java strings to Symbian descriptors.
    HBufC *mimeType = CreateHBufCFromJavaStringLC(aEnv, aMimeType);
    HBufC *outputPath = CreateHBufCFromJavaStringLC(aEnv, aOutputPath);
    HBufC *suggestedFilename = CreateHBufCFromJavaStringLC(aEnv, aSuggestedFilename);

    // Create the content access agent.
    CSupplier* supplier = CSupplier::NewLC();
    CMetaDataArray* metaData = CMetaDataArray::NewLC();

    // Tell the agent which MIME type to use for the encrypted data.
    _LIT(KContentType, "Content-type");
    metaData->AddL(KContentType, *mimeType);
    supplier->SetOutputDirectoryL(*outputPath);

    // The KOmaImportContentType is a OMA DRM agent specific MIME type which
    // indicates that plain content is to be encrypted
    CImportFile* file = supplier->ImportFileL(
                            KOmaImportContentType, *metaData, *suggestedFilename);

    CleanupStack::PopAndDestroy(metaData);
    CleanupStack::PopAndDestroy(supplier);
    CleanupStack::PopAndDestroy(suggestedFilename);
    CleanupStack::PopAndDestroy(outputPath);
    CleanupStack::PopAndDestroy(mimeType);

    // Return handle to CImportFile. Utilize the fact that in Symbian
    // all pointer addresses are MOD 4 so the last 2 bits are 0
    // and can be shifted out. This way the returned handle is
    // always positive whereas Symbian error codes are always negative.
    return reinterpret_cast<TUint>(file)>>2;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_FileWriter
 * Method:    _create
 * Signature: (Ljava/lang/String;Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_FileWriter__1create
(JNIEnv *aEnv, jclass aClass, jstring aMimeType, jstring aOutputPath, jstring aSuggestedFilename)
{
    TInt result = KErrNone;
    TRAPD(err, result = CreateL(
                            aEnv, aClass, aMimeType, aOutputPath, aSuggestedFilename));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_FileWriter
 * Method:    _write
 * Signature: (I[BII)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_FileWriter__1write
(JNIEnv *aEnv, jclass, jint aHandle, jbyteArray aBytes, jint aOffset, jint aLength)
{
    // Convert handle to pointer.
    CImportFile *file = reinterpret_cast<CImportFile*>(aHandle<<2);

    const TInt KWriteBufferSize = 2048;
    TBuf8<KWriteBufferSize> writeBuffer;
    TInt err = KErrNone;
    TInt index = aOffset;
    TInt limit = aOffset + aLength;
    TInt length = KWriteBufferSize;
    while (index < limit && KErrNone == err)
    {
        if (limit - index < KWriteBufferSize)
        {
            length = limit - index;
        }
        // Convert bytes from jbyteArray to TDesC8.
        (void)S60CommonUtils::CopyToNative(
            *aEnv, aBytes, index, length, writeBuffer);
        // Write data.
        err = file->WriteData(writeBuffer);
        index += length;
    }

    return err;
}

/**
 * See JNI method __1close.
 * This is an internal wrapper method for handling Leaves.
 */
jint CloseL(JNIEnv *aEnv, jclass aClass, jint aHandle, jobject aOutputFilename)
{
    // Convert handle to pointer.
    CImportFile *file = reinterpret_cast<CImportFile*>(aHandle<<2);

    // Complete writing and get output filename from the agent.
    TInt err = file->WriteDataComplete();
    // Initialize output filename to aOutputFilename parameter.
    if (KErrNone == err)
    {
        // Get class handle to StringBuffer
        aClass = aEnv->GetObjectClass(aOutputFilename);
        // Get method ID to StringBuffer StringBuffer.append(String)
        jmethodID methodID = aEnv->GetMethodID(
                                 aClass, "append", "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
        if (0 == methodID)
        {
            ELOG(EJavaInstaller,
                 "FileWriter__1close: StringBuffer.append not found");
            err = KErrGeneral;
        }
        else
        {
            CSupplierOutputFile &output = file->OutputFileL(0);
            TPtrC outputFilename = output.FileName();
            // Create new Java String.
            jstring jOutputFilename =
                aEnv->NewString(outputFilename.Ptr(), outputFilename.Length());
            aEnv->CallObjectMethod(aOutputFilename, methodID, jOutputFilename);
        }
    }

    // Delete the CImportFile instance which is no longer used.
    delete file;
    return err;
}

/*
 * Class:     com_nokia_mj_impl_installer_utils_FileWriter
 * Method:    _close
 * Signature: (ILjava/lang/StringBuffer;)I
 */
JNIEXPORT jint JNICALL Java_com_nokia_mj_impl_installer_utils_FileWriter__1close
(JNIEnv *aEnv, jclass aClass, jint aHandle, jobject aOutputFilename)
{
    TInt result = KErrNone;
    TRAPD(err, result = CloseL(aEnv, aClass, aHandle, aOutputFilename));
    if (KErrNone == err)
    {
        err = result;
    }
    return err;
}
