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
* Description:  Implementation of STSSignature
 *
*/


#include <hash.h>
#include "stssignature.h"
#include "logger.h"

namespace java
{
namespace satsa
{

STSSignature::STSSignature()
{
    mdctx = NULL;
    md = NULL;
    mIsUpdated = false;
}


STSSignature::~STSSignature()
{

}

int STSSignature::Update(JNIEnv* /*aJni*/, jbyteArray /*aData*/, jint /*aOffset*/,
                         jint /*aLength*/)
{
    LOG(ESATSA, EInfo, "STSRSACipher::Update");
    return 0;
}
} // namespace satsa
} // namespace java


