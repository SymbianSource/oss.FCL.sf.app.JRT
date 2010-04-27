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
* Description:  Base class for cipher algorithms which implements
 *                CSymmetricCipher.
 *
*/


#ifndef STSSYMMETRICCIPHER_H
#define STSSYMMETRICCIPHER_H

//  INCLUDES
#include <string>

#include <openssl/evp.h>
#include "stscipher.h"

using namespace std;

// CONSTANTS
class CSymmetricCipher;
class CBlockTransformation;
class CPadding;

namespace java
{
namespace satsa
{
// CLASS DECLARATION
/**
 *  Base class for cipher algorithms which implements CSymmetricCipher.
 *  For every algorithm key validity must be checked in the subclasses
 *  because initialization with wrong size key causes a panic.
 *
 */
class STSSymmetricCipher: public STSCipher
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~STSSymmetricCipher();

    static STSSymmetricCipher* Create(STSTransformation* aTransformation,
                                      int* errCode);

public:
    // Functions from STSCipher

    jint DoInit(JNIEnv* aJni, const TCipherMode aMode,
                const jstring aKeyAlgorithm, const jstring aKeyFormat,
                const jbyteArray aKeyEncoded, const jbyteArray aParams);

    jint DoFinal(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                 jint aInputLength, jbyteArray aOutput, jint aOutputOffset);

    jint Update(JNIEnv* aJni, jbyteArray aInput, jint aInputOffset,
                jint aInputLength, jbyteArray aOutput, jint aOutputOffset);

protected:

    /**
     * C++ default constructor.
     */
    STSSymmetricCipher();

    int Construct(STSTransformation* aTransformation);

private:

    // Provides symmetric cipher functionality.
    CSymmetricCipher* mCipher;

    // Bytes processed using Update method. Value is reset to 0 in Init
    // method.
    int mBytesProcessed;

    EVP_CIPHER_CTX *mCipherCtx;
    const EVP_CIPHER *mCipherType;

};
} // namespace satsa
} // namespace java
#endif // STSSYMMETRICCIPHER_H
// End of File
