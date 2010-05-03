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
* Description:  This class provides runtime support for push plugins.
 *
*/


#ifndef MIDPRUNTIMEPUSHSUPPORT_H
#define MIDPRUNTIMEPUSHSUPPORT_H

#include "javaosheaders.h"

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

OS_NONSHARABLE_CLASS(MidpStarterPushSupport)
{
public:

    //API
    virtual void
    startMidletRequestFromPush(const std::wstring& pushAdditionalInfo) = 0;
    virtual void closeRuntimeRequestFromPush() = 0;

protected:
    //Destructor
    virtual ~MidpStarterPushSupport() {}

};
} //end namespace runtime
} //end namespace java

#endif // MIDPRUNTIMEPUSHSUPPORT_H

