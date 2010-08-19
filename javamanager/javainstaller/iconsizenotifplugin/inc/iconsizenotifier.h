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


#ifndef ICONSIZENOTIFIER_H
#define ICONSIZENOTIFIER_H


#include <eiknotapi.h> // MEikSrvNotifierBase2


const TInt KIdIconSizeNotifPlugin = 0x101FD68B;
const TUid KUidIconSizeNotifPlugin = {KIdIconSizeNotifPlugin};


class CIconSizeNotifier : public CBase, public MEikSrvNotifierBase2
{
public:
    static CIconSizeNotifier* NewLC();
    ~CIconSizeNotifier();

private:
    CIconSizeNotifier();
    void ConstructL();

// From MEikSrvNotifierBase2
public:
    void Release();
    TNotifierInfo RegisterL();
    TNotifierInfo Info() const;
    void StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
    TPtrC8 StartL(const TDesC8& aBuffer);
    void Cancel();
    TPtrC8 UpdateL(const TDesC8& aBuffer);
    void UpdateL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
    TInt NotifierCapabilites();
private:
    TNotifierInfo iInfo;
};

#endif // ICONSIZENOTIFIER_H
