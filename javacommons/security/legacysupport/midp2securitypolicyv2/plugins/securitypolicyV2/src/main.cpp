/*
* Copyright (c) 2003 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/




#include <f32file.h>
#include "SecurityPolicy.h"
#include "XPFParser.h"

EXPORT_C MIDP::MSecurityPolicyV2* GetSecurityPolicyL(TUint32 aFlags, const TDesC& /*aPolicyName*/)
{
    if (aFlags > 0x0200)
    {
        User::Leave(KErrArgument);
    }
    return MIDP::CSecurityPolicy::NewL();
}

