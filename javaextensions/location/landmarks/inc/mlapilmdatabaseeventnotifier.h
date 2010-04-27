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
* Description:  Interface for listening native database events
 *
*/


#ifndef MLAPILMDATABASEEVENTNOTIFIER_H
#define MLAPILMDATABASEEVENTNOTIFIER_H

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <e32def.h>

// FORWARD DECLARATIONS
class MLAPILmDatabaseObserver;

/**
 * Interface for listening native database events
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(MLAPILmDatabaseEventNotifier)
{
public:

    /**
     * Destructor. This allows deletion through this interface
     */
    virtual ~MLAPILmDatabaseEventNotifier()
    {}

public: // New functions

    /**
     * Adds an event observer to this notifier
     *
     * @param aObserver An event observer. This class does NOT take
     *        the ownership of the value
     */
    virtual void AddObserverL(MLAPILmDatabaseObserver* aObserver) = 0;

    /**
     * Removes an event observer from this notifier. Nothing happens if
     * there is no such observer in the observer array
     *
     * @param aObserver An event observer. This class does NOT take
     *        the ownership of the value
     */
    virtual void RemoveObserver(MLAPILmDatabaseObserver* aObserver) = 0;

    /**
     * Returns ETrue if event listening is supported. EFalse if the
     * Landmarks API does not support event listening
     */
    virtual TBool IsEventListeningSupported() = 0;

}
;

#endif // MLAPILMDATABASEEVENTNOTIFIER_H
// End of file
