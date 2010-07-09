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

#include <vector>
#include <memory>
#include <cenrepnotifyhandler.h>

#include "javaosheaders.h"
#include "settingslistenerinterface.h"

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
    virtual void startListening();
    virtual void stopListening();

private:
    void cleanUp();
    CenRepListener* createCenRepKeyListener(TUid aRepoId, TUint32 aKeyId,
                                            CCenRepNotifyHandler::TCenRepKeyType aKeyType,
                                            TInt& aErrorCode);

    CoreInterface* mCore;  // resource not owned
    typedef std::vector<CenRepListener*> crListeners_t;
    crListeners_t mCenRepListeners; // listeners are owned resources.

};

} // namespace captain
} // namespace java

#endif // SETTINGSLISTENERIMPL_H

