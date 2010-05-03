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


#include "CJavaEventServer.h"

// class CJavaEventServer

void CJavaEventServer::PostEvent(CJavaEventBase* aEvent, TInt aPriority)
//
// Post the event object to the event dispatcher.
//
// If the dispatcher is ready, then send it directly, otherwise
// add it to the event queue
//
{
    ASSERT(aEvent!=0);
    ASSERT(aPriority >= 0 && aPriority <= CJavaEventBase::ELastPriority);

    NotifyJavaCall(reinterpret_cast<TInt>(aEvent), aPriority);

}

// class CJavaEventSourceBase

EXPORT_C TBool CJavaEventSourceBase::DoPostEvent(CJavaEventBase* aEvent, TInt aPriority)
//
// Post the event to the event dispatcher queue
//
// Can accept a NULL pointer, which assumes OOM has occurred
//
// If this event source has been disposed, the event is discarded
// and EFalse is returned to indicate that events should no longer
// be generated for this event source.
//
// Otherwise the event is passed to the event queue and ETrue is
// returned to indicate success.
//
{
    if (!aEvent)
        return ETrue;
    if (CheckEvent(aEvent) && Open())
    {
        aEvent->iObject=this;
        iServer->PostEvent(aEvent, aPriority);
        return ETrue;
    }
    if (aEvent->IsDisposable())
    {
        delete aEvent;
    }
    return EFalse;
}

EXPORT_C TBool CJavaEventSourceBase::CheckEvent(CJavaEventBase* /*aEvent*/)
{
    return ETrue;
}
