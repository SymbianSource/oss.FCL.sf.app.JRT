/*
* Copyright (c) 1999-2001 Nokia Corporation and/or its subsidiary(-ies).
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


#include <mevents.h>
#include <j2me/jdebug.h>

TBool CJavaEventSourceBase::Open()
//
// Add an extra reference to the object if it is not disposed
// Return true if we have gained a good reference, false if
// the object is awaiting destruction
//
{
    if (!IsDisposed())
    {
        // Another thread may call Dispose() between the test for
        // being disposed and the reference increment
        // Thus we must check to see if the reference count before
        // the increment was -1, in which case we must release the object
        if (User::LockedInc(iRef)>=0)
            return ETrue;
        User::LockedDec(iRef);
    }
    return EFalse;
}

