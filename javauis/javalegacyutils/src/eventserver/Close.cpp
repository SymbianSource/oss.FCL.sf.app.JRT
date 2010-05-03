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

void CJavaEventSourceBase::DoFinalize(CJavaEventSourceBase* aThis)
//
// Server-side invokation function for finalization
//
{
    aThis->FinalizeSvr();
}

EXPORT_C void CJavaEventSourceBase::Close(JNIEnv& aJni)
//
// Remove a reference to the object, this is either from Dispose(),
// or due to event dispatch. When no longer referenced, the object is
// destroyed completely.
//
{
    if (User::LockedDec(iRef)==0)
    {
        __ASSERT_DEBUG(IsDisposed(),User::Invariant());     // must have called Dispose()
        // server side finalization
        if (iServer.Handle()!=0)
        {
            ExecuteV(&CJavaEventSourceBase::DoFinalize,this);
        }
        // JNI object finalization
        FinalizeJni(aJni);
        // remove our own weak reference
        if (iPeer)
            aJni.DeleteWeakGlobalRef(iPeer);
        // finally destroy the object
        delete this;
    }
}

void CJavaEventSourceBase::OnVmClose()
//
// Remove a reference to the object, this is either from Dispose(),
// or due to event dispatch. When no longer referenced, the object is
// destroyed completely.
//
{
    if (User::LockedDec(iRef)==0)
    {
        __ASSERT_DEBUG(IsDisposed(),User::Invariant());     // must have called Dispose()
        // server side finalization
        if (iServer.Handle()!=0)
        {
            //ExecuteV(&CJavaEventSourceBase::DoFinalize,this);
        }

        // finally destroy the object
        delete this;
    }
}
