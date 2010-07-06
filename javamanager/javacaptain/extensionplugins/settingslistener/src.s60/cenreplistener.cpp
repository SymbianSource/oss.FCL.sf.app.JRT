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

#include "coreinterface.h"
#include "eventconsumerinterface.h"

#include "logger.h"
#include "cenreplistener.h"
#include "settingschangeeventsprovidermessages.h"

#include <e32base.h>
#include <centralrepository.h>

namespace java
{
namespace captain
{

    // General listener used as a base value indicating that whole repository is being
    // listened instead of individual key.
    CenRepListener::CenRepListener() : mKeyType(CenRepListener::GENERAL_LISTENER)
    {
        JELOG2(EJavaCaptain);
    }

    CenRepListener::~CenRepListener()
    {
        JELOG2(EJavaCaptain);
        Close();
    }

    void CenRepListener::Close()
    {
        JELOG2(EJavaCaptain);
        if (mNotifyHandler ) {
            mNotifyHandler->StopListening();
            delete mNotifyHandler;
            mNotifyHandler = 0;
        }
        if ( mCenRepSession ) {
            delete mCenRepSession;
            mCenRepSession = 0;
        }
    }

    CenRepListener* CenRepListener::NewLC(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
        CCenRepNotifyHandler::TCenRepKeyType aKeyType)
    {
        JELOG2(EJavaCaptain);
        CenRepListener* crListener = new (ELeave) CenRepListener();
        CleanupStack::PushL(crListener);
        crListener->ConstructL(aCore, aRepoId, aKeyId, aKeyType);
        return (crListener);
    }

    CenRepListener* CenRepListener::NewL(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
        CCenRepNotifyHandler::TCenRepKeyType aKeyType)
    {
        JELOG2(EJavaCaptain);
        CenRepListener* crListener = CenRepListener::NewLC(aCore, aRepoId, aKeyId, aKeyType);
        CleanupStack::Pop(crListener);
        return (crListener);
    }

    CenRepListener* CenRepListener::NewLC(CoreInterface* aCore, TUid aRepoId)
    {
        JELOG2(EJavaCaptain);
        CenRepListener* crListener = new (ELeave) CenRepListener();
        CleanupStack::PushL(crListener);
        crListener->ConstructL(aCore, aRepoId);
        return (crListener);
    }

    CenRepListener* CenRepListener::NewL(CoreInterface* aCore, TUid aRepoId)
    {
        JELOG2(EJavaCaptain);
        CenRepListener* crListener = CenRepListener::NewLC(aCore, aRepoId);
        CleanupStack::Pop(crListener);
        return (crListener);
    }

    void CenRepListener::ConstructL(CoreInterface* aCore, TUid aRepoId, TUint32 aKeyId,
        CCenRepNotifyHandler::TCenRepKeyType aKeyType)
    {
        JELOG2(EJavaCaptain);
        mCore = aCore;
        mRepoId = aRepoId;
        mKeyType = aKeyType;
        mCenRepSession = CRepository::NewL(mRepoId);
        mNotifyHandler = CCenRepNotifyHandler::NewL(*this, *mCenRepSession,
                                                    aKeyType, aKeyId);
        mNotifyHandler->StartListeningL();
    }

    void CenRepListener::ConstructL(CoreInterface* aCore, TUid aRepoId)
    {
        JELOG2(EJavaCaptain);
        mCore = aCore;
        mRepoId = aRepoId;
        mCenRepSession = CRepository::NewL(mRepoId);
        mNotifyHandler = CCenRepNotifyHandler::NewL(*this, *mCenRepSession);
        mNotifyHandler->StartListeningL();
    }

    void CenRepListener::HandleNotifyInt(TUint32 aKeyId, TInt /*aNewValue*/)
    {
        JELOG2(EJavaCaptain);
        WLOG1(EJavaCaptain, "Change in Unrecognised cenrep TInt key noticed! "
              "CR Key= %0x", aKeyId);
    }

    void CenRepListener::HandleNotifyReal( TUint32 aKeyId, TReal /*aNewValue*/ )
    {
        JELOG2(EJavaCaptain);
        WLOG1(EJavaCaptain, "Change in Unrecognised cenrep TReal key noticed! "
              "CR Key= %0x", aKeyId);
    }

    void CenRepListener::HandleNotifyBinary( TUint32 aKeyId, const TDesC8& /*aNewValue*/ )
    {
        JELOG2(EJavaCaptain);
        WLOG1(EJavaCaptain, "Change in Unrecognised cenrep Binary key noticed! "
              "CR Key= %0x", aKeyId);
    }

    void CenRepListener::HandleNotifyGeneric(TUint32 aKeyId)
    {
        JELOG2(EJavaCaptain);
        WLOG1(EJavaCaptain, "Change in Unrecognised cenrep key noticed! "
              "CR Key= %0x", aKeyId);
    }

    void CenRepListener::HandleNotifyString( TUint32 aKeyId, const TDesC16& /*aNewValue*/ )
    {
        JELOG2(EJavaCaptain);
        if ( KCRUidJavaRuntime == mRepoId && KJavaRuntimeMIDPClasspath == aKeyId ) {
            ILOG(EJavaCaptain, "CenRepListener::HandleNotifyString: change notified "
                 "in key KJavaRuntimeMIDPClasspath");
            dispatchEvent(SETTINGS_CHANGE_EVENT_PROVIDER,
                                        MIDP_CLASS_PATH_CHANGE);
        } else {
            WLOG1(EJavaCaptain, "Change in Unrecognised cenrep string key noticed! "
                  "CR Key= %0x", aKeyId);
        }
    }

    void CenRepListener::HandleNotifyError(TUint32 aKeyId, TInt error, CCenRepNotifyHandler* /* aHandler */)
    {
        JELOG2(EJavaCaptain);
        ELOG2(EJavaCaptain, "Error (code: %d) occured when listening changes on "
              "Cenrep key (Key ID: %0x). Listening stopped!", error, aKeyId);
        Close();
    }

    void CenRepListener::dispatchEvent(const std::string& aEvent,
                                       const SettingsChangeEventType_t& aType) const
    {
        JELOG2(EJavaCaptain);
        ILOG2(EJavaCaptain, "CenRepListener::dispatchEvent: dispatching event=%s "
              "type=%d", aEvent.c_str(), aType);
        CommsMessage eventMsg;
        setSettingsChangeEventMessageParams(eventMsg, aType);
        mCore->getEventDispatcher()->event(aEvent, eventMsg);
    }

} // namespace captain
} // namespace java

