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
* Description:  Implementation of STSCipher
 *
*/



#include "stscipher.h"
#include "logger.h"

namespace java
{
namespace satsa
{


STSCipher::STSCipher():mTransformation(0), mIV(0), miv_length(0), mKey(0),
        mKey_length(0)
{

}


STSCipher::~STSCipher()
{
    delete mTransformation;
    mTransformation = 0;
    delete mIV;
    mIV = 0;
    delete mKey;
    mKey = 0;
}

// Sets the transformation for this cipher object.
void STSCipher::SetTransformation(STSTransformation* aTransformation)
{
    LOG(ESATSA, EInfo, "STSCipher::SetTransformation+");
    // Delete existing transformation object.
    delete mTransformation;

    // Take the ownership.
    mTransformation = aTransformation;
    LOG(ESATSA, EInfo, "STSCipher::SetTransformation--");
}

// Initializes the cipher.
jint STSCipher::Init(JNIEnv* aJni, const TCipherMode aMode,
                     const jstring aKeyAlgorithm, const jstring aKeyFormat,
                     const jbyteArray aKeyEncoded)
{
    LOG(ESATSA, EInfo, "STSCipher::Init+");
    // Function return value
    jint retVal = 0;

    // Call the ciphers specific initialize function.
    retVal = DoInit(aJni, aMode, aKeyAlgorithm, aKeyFormat, aKeyEncoded, NULL);

    if (retVal == 0)
    {
        // Set the mode of the initialized cipher
        mMode = aMode;
    } // end if

    // return the return value
    LOG(ESATSA, EInfo, "STSCipher::Init--");
    return retVal;
}

//Initialize the cipher
jint STSCipher::Init(JNIEnv* aJni, const TCipherMode aMode,
                     const jstring aKeyAlgorithm, const jstring aKeyFormat,
                     const jbyteArray aKeyEncoded, const jbyteArray aParams)
{
    LOG(ESATSA, EInfo, "STSCipher::Init with Params+");
    // Function return value
    jint retVal = 0;

    // Call the ciphers specific initialize function.
    retVal = DoInit(aJni, aMode, aKeyAlgorithm, aKeyFormat, aKeyEncoded,
                    aParams);

    if (retVal == 0)
    {
        // Set the mode of the initialized cipher
        mMode = aMode;
    } // end if

    // return the return value
    LOG(ESATSA, EInfo, "STSCipher::Init with Params--");
    return retVal;
}

// Function to retreive the IV of the cipher.
const unsigned char* STSCipher::IV(int* length) const
{
    LOG(ESATSA, EInfo, "STSCipher::IV+");
    // return IV if it is set or generated, otherwise return NULL.
    if (length)
    {
        *length = miv_length;
    }
    LOG(ESATSA, EInfo, "STSCipher::IV--");
    return mIV;

}
} // namespace satsa
} // namespace java

