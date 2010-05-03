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
* Description:  Class presenting transformation for cipher operations.
 *
*/


#ifndef STSTRANSFORMATION_H
#define STSTRANSFORMATION_H

//  INCLUDES
#include <string>

#include <jni.h>
#include "stsconstants.h"

using namespace std;
namespace java
{
namespace satsa
{
/**
 *  Class presenting transformation for cipher operations.
 *
 *  Transformation is presented with string <algorithm>/<mode>/<padding>
 *  or <algorithm>
 */
class STSTransformation
{
public:
    // Constructors and destructor
    /**
     * Destructor.
     */
    virtual ~STSTransformation();

    static STSTransformation* Create(JNIEnv* aJni, jstring aTransformation,
                                     int* errCode);
protected:
    /**
     * Constructor
     */
    STSTransformation();

public:
    /**
     * @return Transformation's algorithm.
     */
    const std::wstring Algorithm();

    /**
     * @return Transformation's mode.
     */
    const std::wstring Mode();

    /**
     * @return Transformation's padding.
     */
    const std::wstring Padding();

private:
    /**
     * Parses transformation string.
     *
     * @param aTransformation Contains transformation
     *        <algorithm>/<mode>/<padding> or <algorithm>.
     */
    int Construct(JNIEnv* aJni, jstring aTransformation);

private:

    // Owned algorithm
    std::wstring mAlgorithm;

    // Owned mode
    std::wstring mMode;

    // Owned padding
    std::wstring mPadding;

};
} // namespace satsa
} // namespace java
#endif // STSTRANSFORMATION_H
// End of File
