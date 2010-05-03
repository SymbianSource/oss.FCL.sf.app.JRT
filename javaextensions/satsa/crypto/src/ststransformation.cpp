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
* Description:  Implementation of STSTransformation
 *
*/


#include "ststransformation.h"
#include "javajniutils.h"
#include "stsconstants.h"
#include "logger.h"

namespace java
{
namespace satsa
{

//Static function used to create the transformation object
STSTransformation* STSTransformation::Create(JNIEnv* aJni,
        jstring aTransformation, int* errCode)
{

    // Create the object
    STSTransformation* self = new STSTransformation();

    if (self != NULL)
    {
        // to parse and decode the transformation string
        *errCode = self->Construct(aJni, aTransformation);
        if (*errCode != 0)
        {
            ELOG(ESATSA, "STSTransformation::Create:construct failed");
            // An Error occured in parsing the transformation string
            return NULL;
        }
        else
        {
            // there was no error in parsing the transformation string
            return self;
        }
    }
    else
    {
        ELOG(ESATSA, "STSTransformation::Create:object creation failed");
        *errCode = KSTSErrNoMemory;
        return NULL;
    }
}

STSTransformation::STSTransformation()
{

}

STSTransformation::~STSTransformation()
{

}

int STSTransformation::Construct(JNIEnv* aJni, jstring aTransformation)
{
    LOG(ESATSA, EInfo, "STSTransformation::Construct+");
    std::wstring algorithm;
    std::wstring mode;
    std::wstring padding;

    //convert the jstring to native string
    std::wstring transformation;
    try
    {
        transformation = java::util::JniUtils::jstringToWstring(aJni,
                         aTransformation);
    }
    catch (...)
    {
        ELOG(ESATSA, "caught exception. Return error code");
        return (KSTSErrNoSuchAlgorithm);
    }

    int index = transformation.find(STSTransformationDelim);
    if (index != wstring::npos)
    {
        // Must be in a form of algorithm/mode/padding
        // algorithm
        algorithm.assign(transformation, 0, index);
        // mode/padding
        std::wstring modeAndPadding(transformation, index + 1, wstring::npos);

        index = modeAndPadding.find(STSTransformationDelim);
        if (index == wstring::npos)
        {
            ELOG(ESATSA, "mode or padding is missing");
            // There isn't delimeter, mode or padding is missing
            return (KSTSErrNoSuchAlgorithm);
        }

        // mode
        mode.assign(modeAndPadding, 0, index);
        if (mode.length() < 1)
        {
            ELOG(ESATSA, "mode was not found");
            // Mode wasn't found
            return (KSTSErrNoSuchAlgorithm);
        }

        // padding
        padding.assign(modeAndPadding, index + 1, wstring::npos);
        if (padding.length() < 1)
        {
            ELOG(ESATSA, "padding was not found");
            // Padding wasn't found
            return (KSTSErrNoSuchPadding);
        }

    }
    else
    {
        // If there is no delimeter in the string it
        // must contain only algorithm.
        algorithm.assign(transformation);

        // Mode and padding aren't defined
        // Use default ones
        mode.assign(STSModeECB);
        padding.assign(STSPaddingPKCS5);

    }

    // parsing over, assign the parsed tokens
    mAlgorithm = algorithm;
    mMode = mode;
    mPadding = padding;

    return 0;
}

const std::wstring STSTransformation::Algorithm()
{
    LOG(ESATSA, EInfo, "STSTransformation::Algorithm+");
    // Construction is failed if mAlgorithm is not valid
    return mAlgorithm;
}

const std::wstring STSTransformation::Mode()
{
    LOG(ESATSA, EInfo, "STSTransformation::Mode+");
    // Construction is failed if mMode is not valid
    return mMode;
}

const std::wstring STSTransformation::Padding()
{
    LOG(ESATSA, EInfo, "STSTransformation::Padding+");
    // Construction is failed if mPadding is not valid
    return mPadding;
}
} // namespace satsa
} // namespace java

