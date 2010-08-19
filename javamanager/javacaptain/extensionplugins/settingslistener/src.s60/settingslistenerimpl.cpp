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
}

SettingsListenerImpl::~SettingsListenerImpl()
{
    cleanUp();
}

void SettingsListenerImpl::startListening()
{
    // All "listeners" must be created&started here.

    // Currently there is just this one CenRep key to be listened
    TInt error = KErrNone;
    CenRepListener* crMidpClassPathChangeListener =
        createCenRepKeyListener(KCRUidJavaRuntime, KJavaRuntimeMIDPClasspath,
                                CCenRepNotifyHandler::EStringKey, error);
    if (crMidpClassPathChangeListener)
    {
        mCenRepListeners.push_back(crMidpClassPathChangeListener);
    }
    else
    {
        ELOG4(EJavaCaptain, "Error (code: %d) occured when starting "
              "CenRepListener (repo=%0x, key=%0x, keytype=%d)",
              error, KCRUidJavaRuntime.iUid, KJavaRuntimeMIDPClasspath,
              CCenRepNotifyHandler::EStringKey);
    }
}

void SettingsListenerImpl::stopListening()
{
    cleanUp();
}

CenRepListener* SettingsListenerImpl::createCenRepKeyListener(TUid aRepoId, TUint32 aKeyId,
        CCenRepNotifyHandler::TCenRepKeyType aKeyType, TInt& aErrorCode)
{
    CenRepListener* listener = NULL;
    TRAP(aErrorCode, listener = CenRepListener::NewL(mCore, aRepoId, aKeyId, aKeyType));
    return listener; // Possible error propagated as a function parameter
}

// Clean up owned resources
void SettingsListenerImpl::cleanUp()
{
    for (crListeners_t::iterator listenersIter = mCenRepListeners.begin();
            listenersIter != mCenRepListeners.end();  ++listenersIter)
    {
        delete *listenersIter;
        *listenersIter = 0;
    }
    mCenRepListeners.clear();
}

} // namespace captain
} // namespace java

