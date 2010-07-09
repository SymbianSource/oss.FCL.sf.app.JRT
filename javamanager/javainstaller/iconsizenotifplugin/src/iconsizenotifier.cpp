/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32cmn.h>
#include <eikenv.h>
#include <bautils.h>
#include <ecom/implementationproxy.h>

#include "iconsizenotifier.h"
#include "iconsizeutils.h"

const TUid KUidScreenOutput = { 0x10009D48 };

CIconSizeNotifier* CIconSizeNotifier::NewLC()
{
    CIconSizeNotifier* self = new(ELeave) CIconSizeNotifier();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CIconSizeNotifier::~CIconSizeNotifier()
{
}

CIconSizeNotifier::CIconSizeNotifier()
{
}

void CIconSizeNotifier::ConstructL()
{
}

void CIconSizeNotifier::Release()
{
    delete this;
}

CIconSizeNotifier::TNotifierInfo CIconSizeNotifier::RegisterL()
{
    iInfo.iUid = KUidIconSizeNotifPlugin;
    iInfo.iChannel = KUidScreenOutput;
    iInfo.iPriority = ENotifierPriorityHigh;
    return iInfo;
}

CIconSizeNotifier::TNotifierInfo CIconSizeNotifier::Info() const
{
    return iInfo;
}

void CIconSizeNotifier::StartL(const TDesC8& /*aBuffer*/, TInt aReplySlot, const RMessagePtr2& aMessage)
{
    TPckgBuf<TIconSizes> res(IconSizeUtils::GetIconSizes());
    aMessage.WriteL(aReplySlot, res);
    aMessage.Complete(KErrNone);
}

TPtrC8 CIconSizeNotifier::StartL(const TDesC8& /*aBuffer*/)
{
    return KNullDesC8();
}

void CIconSizeNotifier::Cancel()
{
}

TPtrC8 CIconSizeNotifier::UpdateL(const TDesC8& /*aBuffer*/)
{
    return KNullDesC8();
}

void CIconSizeNotifier::UpdateL(const TDesC8& /*aBuffer*/, TInt /*aReplySlot*/, const RMessagePtr2& aMessage)
{
    aMessage.Complete(KErrNotSupported);
}

void CreateNotifiersL(CArrayPtrFlat<MEikSrvNotifierBase2>& aNotifiers)
{
    CIconSizeNotifier* notifier1 = CIconSizeNotifier::NewLC();
    aNotifiers.AppendL(notifier1);
    CleanupStack::Pop(notifier1);
}

CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
{
    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers = new CArrayPtrFlat<MEikSrvNotifierBase2>(1);
    if (notifiers)
    {
        TRAPD(err, CreateNotifiersL(*notifiers));
        if (err)
        {
            TInt count = notifiers->Count();
            while (count--)
            {
                (*notifiers)[count]->Release();
            }
            delete notifiers;
            notifiers = NULL;
        }
    }
    return notifiers;
}

TInt CIconSizeNotifier::NotifierCapabilites()
{
    return ENoSpecialCapabilities;
}

// ECom plugin entry point
const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY(0x101FD68A, NotifierArray)
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

