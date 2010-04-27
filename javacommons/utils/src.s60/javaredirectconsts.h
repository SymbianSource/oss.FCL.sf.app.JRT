/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Constants used by redirect client and server
*
*/

#ifndef JAVAREDIRECTCONSTS_H
#define JAVAREDIRECTCONSTS_H

namespace java
{
namespace util
{

_LIT(KJavaRedirectServer,"JavaRedirectServer");

enum ERedirectTypes
{
    ESystemOut,
    ESystemErr,
    ELog
};

const TInt REDIRECT_VERSION_MAJOR = 1;
const TInt REDIRECT_VERSION_MINOR = 0;
const TInt REDIRECT_VERSION_BUILD = 0;

} // namespace util
} // namespace java

#endif // JAVAREDIRECTCONSTS_H
