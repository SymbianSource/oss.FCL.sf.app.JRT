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

#include "settingslistenerimpl.h"
#include "cenreplistener.h"
#include "logger.h"

namespace java
{
namespace captain
{

    SettingsListenerImpl::SettingsListenerImpl(CoreInterface* aCore) :
        mCore(aCore)
    {
        JELOG2(EJavaCaptain);

    }

    SettingsListenerImpl::~SettingsListenerImpl()
    {
        JELOG2(EJavaCaptain);
        cleanUp();
    }

    void SettingsListenerImpl::StartListening()
    {
        JELOG2(EJavaCaptain);
        // All "listeners" must be created&started here.

        // Currently there is just this one CenRep key to be listened
        std::auto_ptr<CenRepListener> cRMidpClassPathChangeListener;
        int error = createCenRepKeyListener(KCRUidJavaRuntime, KJavaRuntimeMIDPClasspath,
                                         CCenRepNotifyHandler::EStringKey,
                                         cRMidpClassPathChangeListener);
        if ( KErrNone == error ) {
            mCenRepListeners.push_back(cRMidpClassPathChangeListener.release());
        } else {
            ELOG4(EJavaCaptain, "Error (code: %d) occured when starting "
                  "CenRepListener (repo=%0x, key=%0x, keytype=%d)",
                  error, KCRUidJavaRuntime.iUid, KJavaRuntimeMIDPClasspath,
                  CCenRepNotifyHandler::EStringKey);
        }
    }

    void SettingsListenerImpl::StopListening()
    {
        JELOG2(EJavaCaptain);
        cleanUp();
    }

    int SettingsListenerImpl::createCenRepKeyListener(TUid aRepoId, TUint32 aKeyId,
                                                  CCenRepNotifyHandler::TCenRepKeyType aKeyType,
                                                std::auto_ptr<CenRepListener>& aListener)
    {
        JELOG2(EJavaCaptain);
        TRAPD( error, aListener.reset(CenRepListener::NewL(mCore, aRepoId, aKeyId, aKeyType)) );
        return error;
    }

    int SettingsListenerImpl::createCenRepListener(TUid aRepoId, CenRepListener*& aListener)
    {
        JELOG2(EJavaCaptain);
        TRAPD( error, aListener = CenRepListener::NewL(mCore, aRepoId));
        return error;
    }

    // Clean up owned resources
    void SettingsListenerImpl::cleanUp()
    {
        JELOG2(EJavaCaptain);

        for  ( crListeners_t::iterator listenersIter = mCenRepListeners.begin();
               listenersIter != mCenRepListeners.end();  ++listenersIter )
        {
            delete *listenersIter;
            *listenersIter = 0;
        }
        mCenRepListeners.clear();
    }

} // namespace captain
} // namespace java

