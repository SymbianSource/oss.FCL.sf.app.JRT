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
* Description:  Creates STSCipher implementation according to
 *                transformation string.
 *
*/


#ifndef STSCIPHERFACTORY_H
#define STSCIPHERFACTORY_H

//  INCLUDES
#include <jni.h>

namespace java
{
namespace satsa
{

// FORWARD DECLARATIONS
class STSCipher;

// CLASS DECLARATION
/**
 *  Creates chipher implementaions.
 *
 */
class STSCipherFactory
{
public:
    /**
     * Creates STSCipher implementation according to transformation string.
     *
     * @param aTransformation Tansformation which created
     *                        cipher must provide
     */
    static STSCipher* CreateCipher(JNIEnv* aJni, jstring aTransformation,
                                   int* errCode);

};
} // namespace satsa
} // namespace java
#endif // STSCIPHERFACTORY_H
// End of File
