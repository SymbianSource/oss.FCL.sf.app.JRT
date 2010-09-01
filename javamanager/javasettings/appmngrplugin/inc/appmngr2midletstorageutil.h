/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MidletAppInfo represents installed Java applications
*
*/


#ifndef APPMNGR2MIDLETSTORAGEUTIL_H
#define APPMNGR2MIDLETSTORAGEUTIL_H

#include <memory>
#include <e32cmn.h>

#include "javaosheaders.h"
#include "javastorage.h"


/**
 * MIDlet Suite Application Info
 *
 * MIDlet Suite Application Info handles uinstallation of installed MIDlet suite,
 * shows MIDlet suite details and MIDlet suite settings.
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.1
 */
OS_NONSHARABLE_CLASS(AppMngr2MidletStorageUtil)
{
public:

    // constructor and destructor
    AppMngr2MidletStorageUtil();
    ~AppMngr2MidletStorageUtil();

    /**
     * Populate suite information.
     *
     * @param aUid suite identifier.
     */
    void populateSuiteInformation(const TUid aUid);

    /**
     * Get given attribute value.
     *
     * @param aName attribute to return.
     * @return attribute value if found, null otherwise. Buffer ownership
     *         is transfered to caller.
     *
     */
    HBufC* attribute(const TDesC& aName);

private:

    /**
     * Check if connection exists. If not open it.
     */
    void checkConnection();

private:
    std::auto_ptr<java::storage::JavaStorage>mStorage;
    java::storage::JavaStorageApplicationEntry_t mPopulatedEntry;
    bool mIsOpen;
};

#endif // APPMNGR2MIDLETSTORAGEUTIL_H

