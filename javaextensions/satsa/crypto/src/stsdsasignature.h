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
* Description:  Provides the functionality of a DSA based cryptographic
 *                cipher for encryption and decryption.
 *
*/


#ifndef STSDSASIGNATURE_H
#define STSDSASIGNATURE_H

//  INCLUDES
#include <string>
#include <openssl/dsa.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <jni.h>
#include "stssignature.h"

using namespace std;
namespace java
{
namespace satsa
{



/**
 *  Provides RSA signature algorithm.
 */
class STSDSASignature: public STSSignature
{
public:
    // Constructors and destructor

    static STSDSASignature* Create(const std::wstring aPadding, int* errCode);

    virtual ~STSDSASignature();

public:
    // From STSSignature
    int InitVerify(JNIEnv* aJni, jstring aKeyAlgorithm, jstring aKeyFormat,
                   jbyteArray aKeyEncoded);

    int Verify(JNIEnv* aJni, jbyteArray aSignature);

    int Update(JNIEnv* aJni, jbyteArray aData, jint aOffset, jint aLength);

protected:


    STSDSASignature();

    int Construct(const std::wstring aPadding);

private:


    DSA* mDSA;
    EVP_PKEY *mpkey;
};
} // namespace satsa
} // namespace java
#endif // STSDSASIGNATURE_H
// End of File
