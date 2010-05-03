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

EXPORT_C CJavaEventSourceBase::TConstructorBase::TConstructorBase(CJavaEventSourceBase* aObject,JNIEnv& aJni)
        :iObject(aObject),iJni(aJni)
{
    // This codescanner error is supressed "illegally" away. The reason is that:
    //   - The method is constructor, so it cannot be "L" method.
    //   - The possible leave is TRAPped correctly in mevents.h file in CJavaEventSource::New
    //     method.
    CleanupStack::PushL(TCleanupItem(&TConstructorBase::Disposer,this)); // codescanner::leave
}

void CJavaEventSourceBase::TConstructorBase::Disposer(TAny* aThis)
{
    TConstructorBase& t=*static_cast<TConstructorBase*>(aThis);
    t.iObject->Dispose(t.iJni);
}
