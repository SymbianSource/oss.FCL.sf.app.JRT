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
* Description:  Handles native landmark database events
 *
*/


#ifndef CLAPILMDATABASEEVENTNOTIFIER_H
#define CLAPILMDATABASEEVENTNOTIFIER_H

// INTERNAL INCLUDES
#include    "mlapilmdatabaseeventnotifier.h"

// EXTERNAL INCLUDES
#include    <e32std.h>
#include    <e32base.h>
#include    <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class MLAPILmDatabaseObserver;

/**
 * Handles native landmark database events
 *
 * The class monitors for the specified landmark database and notifies its
 * observers when a certain database event occurs. Currently, the notifier notifies
 * only when an event which is related to landmarks or categories occurs
 *
 * @lib N/A
 * @since S60 3.2
 */
NONSHARABLE_CLASS(CLAPILmDatabaseEventNotifier) : public CActive,
        public MLAPILmDatabaseEventNotifier
{
public: // Constructors and destructor

    /**
     * Two-phased constructor. Creates an instance from this class
     * The caller takes the ownership of the returned instance which
     * is left to the cleanup stack
     *
     * @param aDatabase The database which events this active object listens
     * @return New instance from this class
     */
    static CLAPILmDatabaseEventNotifier* NewLC(CPosLandmarkDatabase& aDatabase);

    /**
     * Destructor.
     */
    virtual ~CLAPILmDatabaseEventNotifier();

public: // From MLAPILmDatabaseEventNotifier

    /**
     * Adds an event observer to this notifier
     *
     * @param aObserver An event observer. This class does NOT take
     *        the ownership of the value
     */
    void AddObserverL(MLAPILmDatabaseObserver* aObserver);

    /**
     * Removes an event observer from this notifier. Nothing happens if
     * there is no such observer in the observer array
     *
     * @param aObserver An event observer. This class does NOT take
     *        the ownership of the value
     */
    void RemoveObserver(MLAPILmDatabaseObserver* aObserver);

    /**
     * Returns ETrue if event listening is supported. EFalse if the
     * Landmarks API does not support event listening
     */
    TBool IsEventListeningSupported();

protected: // From CActive

    /**
     * Called when a request has completed. Note that RunError is not
     * implemented because RunL is currently leave-safe
     */
    void RunL();

    /**
     * Called by the base class when this active object is canceled
     */
    void DoCancel();

protected: // Constructors

    /**
     * C++ constructor
     */
    CLAPILmDatabaseEventNotifier(CPosLandmarkDatabase& aDatabase);

private: // Data

    // The database which events this object listens. Used
    CPosLandmarkDatabase& iDatabase;
    // Array of database event observers. Used
    RPointerArray< MLAPILmDatabaseObserver> iEventObservers;
    // Event objects which holds the event and item id when
    // a request has completed.
    TPosLmEvent iEvent;

};

#endif // CLAPILMDATABASEEVENTNOTIFIER_H
// End of file
