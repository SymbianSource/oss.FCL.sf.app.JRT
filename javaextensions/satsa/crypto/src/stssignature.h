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
* Description:  Provides the functionality of a digital signature algorithm
 *
*/


#ifndef STSSIGNATURE_H
#define STSSIGNATURE_H

//  INCLUDES

#include <jni.h>
#include <openssl/evp.h>
#include "javajniutils.h"

namespace java
{
namespace satsa
{

/**
 *  STSSignature provides the functionality of a digital signature algorithm.
 *
 *  Digital signatures are used for authentication and
 *  integrity assurance of digital data.
 *  There are three phases to the use of a Signature object
 *  for verifying a signature:
 *  1. Initialization, with a public key, which initializes the signature for
 *     verification
 *  2. Updating Depending on the type of initialization,
 *     this will update the bytes to be verified.
 *  3. Verifying a signature on all updated bytes.
 *
 */
class STSSignature
{
public:

    static STSSignature* Create();

    virtual ~STSSignature();

public:
    /**
     * Initializes this object for verification. If this method is called
     * again with a different argument, it negates the effect of this call.
     *
     * @param aKey The public key of the identity whose signature is going
     *        to be verified.
     */
    virtual int InitVerify(JNIEnv* aJni, jstring aKeyAlgorithm,
                           jstring aKeyFormat, jbyteArray aKeyEncoded) = 0;

    /**
     * Updates the data to be verified.
     *
     * @param aData Data for update.
     */
    virtual int Update(JNIEnv* aJni, jbyteArray aData, jint aOffset,
                       jint aLength);

    /**
     * Verifies the passed-in signature.
     * A call to this method resets this signature object to the state it
     * was in when previously initialized for verification via a call to
     * InitVerify. That is, the object is reset and available to verify
     * another signature from the identity whose public key was specified
     * in the call to InitVerify.
     *
     * @param aSignature The signature to be verified.
     * @return ETrue if the signature was verified, EFalse if not.
     */
    virtual int Verify(JNIEnv* aJni, jbyteArray aSignature) = 0;

protected:

    STSSignature();

protected:

    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    bool mIsUpdated;
};
} // namespace satsa
} // namespace java
#endif // STSSIGNATURE_H
// End of File
