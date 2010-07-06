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

#ifndef CENREP_LISTENER_H
#define CENREP_LISTENER_H

#include "javaosheaders.h"
#include "settingschangeeventsprovidermessages.h"

#include <e32std.h>
#include <cenrepnotifyhandler.h>

namespace java
{
namespace captain
{

/**
 * Java Runtime environment Central Repository UID
 */
const TUid    KCRUidJavaRuntime             = { 0x10282DFD };

/**
 * Indicates classpath for MIDP runtimes. This key value is generated
 * from the values defined by extension libraries.
 */
const TUint32 KJavaRuntimeMIDPClasspath     = 0x00000001;


class CoreInterface;

OS_NONSHARABLE_CLASS(CenRepListener) :
public CBase, public MCenRepNotifyHandlerCallback
{
public:
    enum TListenerType {GENERAL_LISTENER = -1};

    static CenRepListener* NewLC(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
                                 CCenRepNotifyHandler::TCenRepKeyType aKeyType);
    static CenRepListener* NewL(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
                                  CCenRepNotifyHandler::TCenRepKeyType aKeyType);
    static CenRepListener* NewLC(CoreInterface* aCore, TUid aRepoId);
    static CenRepListener* NewL(CoreInterface* aCore, TUid aRepoId);

    virtual ~CenRepListener();

    // MCenRepNotifyHandlerCallback
    // These handler methods must know what to do in each case, i.e
    // how to handle the change in each particular key.
    virtual void HandleNotifyString( TUint32 aKeyId, const TDesC16& aNewValue );
    virtual void HandleNotifyInt(TUint32 aKeyId, TInt aNewValue);
    virtual void HandleNotifyReal( TUint32 aId, TReal aNewValue );
    virtual void HandleNotifyBinary( TUint32 aId, const TDesC8& aNewValue );
    virtual void HandleNotifyGeneric(TUint32 aKeyId);
    virtual void HandleNotifyError(TUint32 aKeyId, TInt error,
                                   CCenRepNotifyHandler* aHandler);
protected:
    CenRepListener();

    void dispatchEvent(const std::string& aEvent,
                       const SettingsChangeEventType_t& aType) const;

private:
    void ConstructL(CoreInterface* aCore, TUid aRepoId);
    void ConstructL(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
                    CCenRepNotifyHandler::TCenRepKeyType aKeyType);
    void Close();
    TInt mKeyType;
    CoreInterface* mCore;
    TUid mRepoId;
    CCenRepNotifyHandler* mNotifyHandler;
    CRepository* mCenRepSession;
};

} // namespace captain
} // namespace java

#endif // CENREP_LISTENER_H

