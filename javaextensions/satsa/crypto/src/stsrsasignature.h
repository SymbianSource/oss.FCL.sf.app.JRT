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
* Description:  Provide the functionality of a RSA digital signature algorithm
 *
*/


#ifndef STSRSASIGNATURE_H
#define STSRSASIGNATURE_H

//  INCLUDES
#include <string>

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <jni.h>
#include "stssignature.h"

using namespace std;

// CONSTANTS

// CLASS DECLARATION
class CRSAPublicKey;
class CX509SubjectPublicKeyInfo;
class CAlgorithmIdentifier;

namespace java
{
namespace satsa
{
/**
 *  Provides RSA signature algorithm.
 *
 */
class STSRSASignature: public STSSignature
{
public:

    static STSRSASignature* Create(const std::wstring aPadding, int* errCode);

    virtual ~STSRSASignature();

public:
    // From STSSignature

    int InitVerify(JNIEnv* aJni, jstring aKeyAlgorithm, jstring aKeyFormat,
                   jbyteArray aKeyEncoded);

    int Verify(JNIEnv* aJni, jbyteArray aSignature);

    int Update(JNIEnv* aJni, jbyteArray aData, jint aOffset, jint aLength);

protected:

    STSRSASignature();

    int Construct(const std::wstring aPadding);

private:
    CRSAPublicKey* mRSAKey;
    CAlgorithmIdentifier* mHashID;
    RSA *mRSA;
    EVP_PKEY *mpkey;
};
} // namespace satsa
} // namespace java
#endif // STSRSASIGNATURE_H
// End of File
