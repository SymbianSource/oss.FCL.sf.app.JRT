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
* Description:
*
*/


//  INCLUDE FILES

#include "javax_microedition_pki_UserCredentialManager.h"
#include "cstscredentialmanager.h"
#include "satsajnitools.h"
#include "s60commonutils.h"
#include "fs_methodcall.h"



LOCAL_C void CallCreateCredentialManagerAddToEventSourceL(
    TInt* aManagerHandle)
{
    CSTSCredentialManager* manager = CSTSCredentialManager::NewLC();
    TInt managerHandle = reinterpret_cast<TInt>(manager) ;
    CleanupStack::Pop(manager);
    *aManagerHandle = managerHandle;
}

JNIEXPORT jint JNICALL
Java_javax_microedition_pki_UserCredentialManager__1createUCManager(
    JNIEnv* /* aJniEnv */,
    jobject /* aPeer */)
{


    TInt managerHandle = KErrGeneral;

    TInt error = KErrNone;
    TRAP(error,
         CallCreateCredentialManagerAddToEventSourceL(
             &managerHandle));

    if (error != KErrNone)
    {
        return error;
    }

    return managerHandle;
}

JNIEXPORT jint
JNICALL Java_javax_microedition_pki_UserCredentialManager__1addCredential(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aManagerHandle,
    jstring aCertDisplayName, jbyteArray aPkiPath)
{


    CSTSCredentialManager* manager =
        reinterpret_cast<CSTSCredentialManager *>(aManagerHandle);

    const RJString certDisplayName(*aJniEnv, aCertDisplayName);
    const TInt pkiPathLength = aJniEnv->GetArrayLength(aPkiPath);
    jbyte* pkiPath = aJniEnv->GetByteArrayElements(aPkiPath, NULL);
    if (!pkiPath)
    {
        return KErrNoMemory;
    }
    TPtrC8 pkiDes(reinterpret_cast< const TUint8 * >(pkiPath),
                  pkiPathLength);

    TBool completed = EFalse;
    TInt arg1 = reinterpret_cast<TInt>(&certDisplayName);
    TInt arg2 = reinterpret_cast<TInt>(&pkiDes);

    TInt error = KErrNone;
    TRAP(error,CallMethodL(completed,manager,
                           &CSTSCredentialManager::AddCredentialL,
                           arg1,
                           arg2,manager));

    aJniEnv->ReleaseByteArrayElements(aPkiPath, pkiPath, 0);

    if (error == KErrNone)
    {
        return !completed;
    }
    return error;
}

JNIEXPORT jint
JNICALL Java_javax_microedition_pki_UserCredentialManager__1removeCredential(
    JNIEnv* aJniEnv,
    jobject /* aPeer */,
    jint aManagerHandle,
    jstring aCertDisplayName, jbyteArray aIssuerAndSerialNumber,
    jstring aSecurityElementID, jstring aSecurityElementPrompt)
{

    CSTSCredentialManager* manager =
        reinterpret_cast<CSTSCredentialManager *>(aManagerHandle);

    const RJString certDisplayName(*aJniEnv, aCertDisplayName);
    const TInt issuerLength =
        aJniEnv->GetArrayLength(aIssuerAndSerialNumber);
    jbyte* issuerAndSerialNumber =
        aJniEnv->GetByteArrayElements(aIssuerAndSerialNumber, NULL);
    if (!issuerAndSerialNumber)
    {
        return KErrNoMemory;
    }

    TPtrC8 issuerDes(
        reinterpret_cast< const TUint8 * >(issuerAndSerialNumber),
        issuerLength);

    const RJString securityElementID(*aJniEnv, aSecurityElementID);
    const RJString securityElementPrompt(*aJniEnv, aSecurityElementPrompt);

    TBool completed = EFalse;
    TInt arg1 = reinterpret_cast<TInt>(&certDisplayName);
    TInt arg2 = reinterpret_cast<TInt>(&issuerDes);
    TInt arg3 = reinterpret_cast<TInt>(&securityElementID);
    TInt arg4 = reinterpret_cast<TInt>(&securityElementPrompt);
    TInt arg5 = reinterpret_cast<TInt>(&completed);

    TInt error = KErrNone;
    TRAP(error,
         CallMethodL(manager,&CSTSCredentialManager::StaticRemoveCredentialL,
                     arg1,
                     arg2,
                     arg3,
                     arg4,
                     arg5,manager));

    aJniEnv->ReleaseByteArrayElements(aIssuerAndSerialNumber,
                                      issuerAndSerialNumber, 0);


    if (error == KErrNone)
    {
        return completed ? 0 : 1;
    }
    return error;
}


JNIEXPORT jbyteArray
JNICALL Java_javax_microedition_pki_UserCredentialManager__1generateCSR(
    JNIEnv* aJniEnv,
    jobject /*aPeer*/,
    jint aManagerHandle, jstring aNameInfo,
    jstring aAlgorithm, jint aKeyLen, jint aKeyUsage,
    jstring aSecurityElementID, jstring aSecurityElementPrompt,
    jintArray aError)
{
    CSTSCredentialManager* manager =
        reinterpret_cast<CSTSCredentialManager *>(aManagerHandle);

    const RJString nameInfo(*aJniEnv, aNameInfo);
    const RJString algorithm(*aJniEnv, aAlgorithm);
    const RJString securityElementID(*aJniEnv, aSecurityElementID);
    const RJString securityElementPrompt(*aJniEnv, aSecurityElementPrompt);

    TDesC8* retVal = NULL;

    CSTSCredentialManager::TGenerateCSRParams params =
    {
        static_cast< const TDesC* >(&nameInfo),
        static_cast< const TDesC* >(&algorithm),
        aKeyLen, aKeyUsage,
        static_cast< const TDesC* >(&securityElementID),
        static_cast< const TDesC* >(&securityElementPrompt)
    };

    TInt arg1 = reinterpret_cast<TInt>(&params);
    TInt arg2 = reinterpret_cast<TInt>(&retVal);

    TInt error = KErrNone;
    TRAP(error,
         CallMethodL(manager,&CSTSCredentialManager::StaticGenerateCSRL,
                     arg1, arg2,
                     manager));


    STSSetJavaErrorCode(aJniEnv, aError, error);

    if (error != KErrNone)
    {
        return NULL;
    }

    const TInt numBytes = retVal->Length();
    jbyteArray javaValue = aJniEnv->NewByteArray(numBytes);

    if (!javaValue)
    {
        STSSetJavaErrorCode(aJniEnv, aError, KErrNoMemory);
        return NULL;
    }

    if (numBytes > 0)
    {
        aJniEnv->SetByteArrayRegion(
            javaValue, 0, numBytes,
            JAVA_PTR(retVal->Ptr()));
    }
    return javaValue;
}
