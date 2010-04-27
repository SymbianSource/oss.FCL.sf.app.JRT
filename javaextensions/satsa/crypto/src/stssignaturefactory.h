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
* Description:  Creates STSCipher objects according to algorithm.
 *
*/


#ifndef STSSIGNATUREFACTORY_H
#define STSSIGNATUREFACTORY_H

//  INCLUDES
#include <string>

#include <jni.h>

using namespace std;

namespace java
{
namespace satsa
{


class STSSignature;


/**
 *  Creates STSCipher objects according to algorithm.
 *
 *  Algorithm is specified in form of <digest>with<encryption>,
 *  which is used to form a name for a signature algorithm with a particular
 *  message digest (such as MD2 or MD5) and algorithm (such as RSA or DSA),
 *
 */
class STSSignatureFactory
{
public:
    /**
     * Creates new STSCipher instance.
     *
     * @param aAlgorithm <digest>with<encryption>
     * @return New STSSignature instance.
     */
    static STSSignature* CreateSignature(JNIEnv* aJni,
                                         const jstring aAlgorithm, int* errCode);

    /**
     * Checks if algorithm is supported with signatures.
     *
     * @param aAlgorithm Algorithm name.
     * @return true if algorithm is supported with signatures.
     */
    static jboolean
    IsSupportedAlgorithm(JNIEnv* aJni, const jstring aAlgorithm);

    /**
     *
     * @param aKeyAlgorithm Name of the algorithm associated with this key.
     * @param aKeyFormat The primary encoding format of the key.
     * @param aKeyEncoded The encoded key.
     */
    static jint IsValidKey(JNIEnv* aJni, const jstring aAlgorithm,
                           const jstring aKeyFormat, const jbyteArray aKeyEncoded);

};
} // namespace satsa
} // namespace java
#endif // STSSIGNATUREFACTORY_H
// End of File
