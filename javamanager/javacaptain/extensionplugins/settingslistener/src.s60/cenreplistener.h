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
* Description:  Class is used for listening changes in various Symbian Central
*                     repository keys.
*
*/

#ifndef CENREPLISTENER_H
#define CENREPLISTENER_H

#include <e32std.h>
#include <cenrepnotifyhandler.h>

#include "javaosheaders.h"
#include "settingschangeeventsprovidermessages.h"
#include "javaruntimeprivatecrkeys.h"

namespace java
{
namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(CenRepListener) :
        public CBase, public MCenRepNotifyHandlerCallback
{
public:
    enum TListenerType {GENERAL_LISTENER = -1};

    static CenRepListener* NewL(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
                                CCenRepNotifyHandler::TCenRepKeyType aKeyType);

    virtual ~CenRepListener();

    // MCenRepNotifyHandlerCallback (only partial implementation needed here for now)
    // These handler methods must know what to do in each case, i.e
    // how to handle the change in each particular key.
    virtual void HandleNotifyString(TUint32 aKeyId, const TDesC16& aNewValue);
    virtual void HandleNotifyError(TUint32 aKeyId, TInt aError,
                                   CCenRepNotifyHandler* aHandler);
protected:
    CenRepListener();

    void dispatchEvent(const std::string& aEvent,
                       const SettingsChangeEventType_t& aType) const;

private:
    void ConstructL(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
                    CCenRepNotifyHandler::TCenRepKeyType aKeyType);
    void close();
    TInt mKeyType;
    CoreInterface* mCore;
    TUid mRepoId;
    CCenRepNotifyHandler* mNotifyHandler;
    CRepository* mCenRepSession;
};

} // namespace captain
} // namespace java

#endif // CENREPLISTENER_H

