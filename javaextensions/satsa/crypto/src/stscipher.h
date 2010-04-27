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
* Description:  Provides the functionality of a cryptographic cipher for
 *                encryption and decryption.
 *
*/


#ifndef STSCIPHER_H
#define STSCIPHER_H

//  INCLUDES
#include <stdlib.h>
#include <jni.h>
#include <openssl/evp.h>
#include "ststransformation.h"

namespace java
{
namespace satsa
{
// CLASS DECLARATION

/**
 *  STSCipher provides the functionality of a cryptographic cipher for
 *  encryption and decryption.
 */
class STSCipher
{
public:
    /**
     * Used to initialize cipher to encrypt or decrypt mode.
     */
    enum TCipherMode
    {
        EEncryptMode = 1, EDecryptMode = 2
    };

    virtual ~STSCipher();


    /**
     * Initializes this chipher. Creates needed transformation objects and
     * checks key validity.
     *
     * @param aMode EEncryptMode or EDecryptMode
     * @param aKey Key material used in the operations.
     */
    virtual jint Init(JNIEnv* aJni, const TCipherMode aMode,
                      const jstring aKeyAlgorithm, const jstring aKeyFormat,
                      const jbyteArray aKeyEncoded);

    /**
     * Initializes this cipher. Creates needed transformation objects and
     * checks key validity.
     *
     * @param aMode EEncryptMode or EDecryptMode
     * @param aKey Key material used in the operations
     * @param aParams Parameters for the algorithm
     */
    virtual jint Init(JNIEnv* aJni, const TCipherMode aMode,
                      const jstring aKeyAlgorithm, const jstring aKeyFormat,
                      const jbyteArray aKeyEncoded, const jbyteArray aParams);

    /**
     * Encrypts or decrypts data in a single-part operation,
     * or finishes a multiple-part operation.
     *
     * @param aInput Input buffer for the chipher.
     * @param aOutput Output buffer for the chipher.
     */
    virtual jint DoFinal(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                         jint aInputLength, jbyteArray aOutput, jint aOutputOffset) = 0;

    /**
     * Continues a multiple-part encryption or decryption operation.
     *
     * @param aInput Input buffer for the chipher.
     * @param aOutput Output buffer for the chipher.
     */
    virtual jint Update(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                        jint aInputLength, jbyteArray aOutput, jint aOutputOffset) = 0;

    /**
     * Return initialization vector or empty descriptor if IV is not set.
     *
     * @return Initialization vector.
     */
    virtual const unsigned char* IV(int* length) const;

    /**
     * Sets transformation to the cipher. Ownership is transferred.
     *
     * @param aTransformation Transformation used in encryption
     *        and decryption. Ownership is transferred.
     */
    void SetTransformation(STSTransformation* aTransformation);

protected:
    // Pure virtual function which will be implemented by the individual
    // cipher classes.
    virtual jint DoInit(JNIEnv* aJni, const TCipherMode aMode,
                        const jstring aKeyAlgorithm, const jstring aKeyFormat,
                        const jbyteArray aKeyEncoded, const jbyteArray aParams) = 0;

    STSCipher();

protected:

    // Transformation used in encryption and decryption.
    STSTransformation* mTransformation;

    // initialization vector
    unsigned char* mIV;
    unsigned int miv_length;

    // Key for the cipher
    unsigned char* mKey;
    unsigned int mKey_length;

    // Mode set in Init method.
    TCipherMode mMode;

};
} // namespace satsa
} // namespace java
#endif // STSCIPHER_H
// End of File
