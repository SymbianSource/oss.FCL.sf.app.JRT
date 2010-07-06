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
* Description:  Platform specific component which manages all the listeners
*                     listening changes in system configuration. Currently only Central
*                     Repository listening is supported.
*
*/

#ifndef SETTINGSLISTENERIMPL_H
#define SETTINGSLISTENERIMPL_H

#include "javaosheaders.h"

#include "settingslistenerinterface.h"

#include <vector>
#include <memory>
#include <cenrepnotifyhandler.h>

namespace java
{
namespace captain
{

class CoreInterface;
class CenRepListener;

OS_NONSHARABLE_CLASS(SettingsListenerImpl) :  public SettingsListenerInterface
{
public:
    SettingsListenerImpl(CoreInterface* aCore);
    virtual ~SettingsListenerImpl();

    // SystemSettingsChangeListenerInterface
    virtual void StartListening();
    virtual void StopListening();

private:
    void cleanUp();
    int createCenRepKeyListener(TUid aRepoId, TUint32 aKeyId,
                            CCenRepNotifyHandler::TCenRepKeyType aKeyType,
                            std::auto_ptr<CenRepListener>& aListener);
    int createCenRepListener(TUid aRepoId, CenRepListener*& aListener);

    CoreInterface* mCore;  // resource not owned
    typedef std::vector<CenRepListener*> crListeners_t;
    crListeners_t mCenRepListeners; // listeners are owned resources.

};

} // namespace captain
} // namespace java

#endif // SETTINGSLISTENERIMPL_H

