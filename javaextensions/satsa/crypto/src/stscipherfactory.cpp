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
* Description:  Implementation of STSCipherFactory.
 *
*/


// INCLUDE FILES
#include "stscipherfactory.h"
#include "stssymmetriccipher.h"
#include "ststransformation.h"
#include "stsrsacipher.h"
#include "logger.h"

namespace java
{
namespace satsa
{

//factory method which creates the cipher based on the transformation
STSCipher* STSCipherFactory::CreateCipher(JNIEnv* aJni,
        jstring aTransformation, int* errCode)
{
    LOG(ESATSA, EInfo, "STSCipherFactory::CreateCipher+");
    OpenSSL_add_all_algorithms();

    // Create the transformation object
    // Transformation is a form of algorithm/mode/padding or
    // algorithm
    STSTransformation* transformation = STSTransformation::Create(aJni,
                                        aTransformation, errCode);

    if (transformation == NULL)
    {
        // transformation was not created successfully and errCode is set approriately
        ELOG(ESATSA, "transformation was not created successfully");
        return NULL;
    }

    // Transformation succeeded, create the cipher object.
    STSCipher* cipher = NULL;

    if ((transformation->Algorithm() == STSAlgorithm3DES)
            || (transformation->Algorithm() == STSAlgorithmAES)
            || (transformation->Algorithm() == STSAlgorithmRC2)
            || (transformation->Algorithm() == STSAlgorithmDES))
    {
        // Create the native symmetric cipher
        LOG(ESATSA, EInfo, "STSCipherFactory::CreateCipher: Create the native symmetric cipher");
        cipher = STSSymmetricCipher::Create(transformation, errCode);
    }
    else if (transformation->Algorithm() == STSAlgorithmRSA)
    {
        // Create the native RSA cipher.
        LOG(ESATSA, EInfo, "STSCipherFactory::CreateCipher: Create the native RSA cipher");
        cipher = STSRSACipher::Create(errCode);
    }
    else
    {
        // Unsupported algorithm, return NULL.
        ELOG(ESATSA, "STSCipherFactory::CreateCipher: Unsupported algorithm, return NULL.");
        *errCode = KSTSErrNoSuchAlgorithm;
        return NULL;
    }

    if (cipher != NULL)
    {
        // Cipher created successfully.
        // Set the transformation that is expected in the cipher object that was created above
        LOG(ESATSA, EInfo, "STSCipherFactory::CreateCipher: Cipher created successfully.");
        cipher->SetTransformation(transformation);
    }

    LOG(ESATSA, EInfo, "STSCipherFactory::CreateCipher: return cipher");
    // return the cipher object.
    return cipher;
}
} // namespace satsa
} // namespace java


