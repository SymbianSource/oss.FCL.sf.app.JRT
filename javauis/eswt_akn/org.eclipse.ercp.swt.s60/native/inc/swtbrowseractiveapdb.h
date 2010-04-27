/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTBROWSERACTIVEAPDB_H
#define SWTBROWSERACTIVEAPDB_H


#include <e32base.h>

class CActiveApDb;
class CApListItemList;
class CSwtBrowserApDbObserver;

/**
 * CSwtBrowserActiveApDb
 * @lib eswtapifacade.dll
 */

class CSwtBrowserActiveApDb
        : public CBase
{
public:     // construction

    /**
     * Two-phased constructor. Leaves on failure.
     *
     * @return The constructed model
     */
    IMPORT_C static CSwtBrowserActiveApDb* NewL();

    /**
     * Destructor.
     */
    virtual ~CSwtBrowserActiveApDb();


public:


    /**
     * Get Access Point Engine object.
     *
     * @return Access Point Engine object
     */
    IMPORT_C CActiveApDb& ApDb() const;

    /**
     * Get CommsDb object.
     *
     * @return CommsDb object
     */
    IMPORT_C CCommsDatabase& CommsDb() const;

    // From MActiveApDbObserver

    /**
     * From MActiveApDbObserver
     * Handle database event (refresh cached AP list on change).
     *
     * @param aEvent Database-related event.
     * @return void.
     */
    void HandleApDbEventL(const TInt aEvent);

    /**
     * Get Access Point list.
     *
     * @return Access Point list
     */
    IMPORT_C TUint32 FirstAccessPointUidL();



protected:  // construction

    /**
     * Constructor.
     */
    CSwtBrowserActiveApDb();

    /**
     * Second phase constructor. Leaves on failure.
     */
    void ConstructL();

private:

    /**
     * Get all access points (implementation detail). If database is
     * inaccessible, old data remains and update becomes pending.
     *
     * @return void.
     */
    void GetAccessPointsL();

    /**
     * Get all access points (implementation detail).
     *
     * @param aList Append access points to this list
     * @return void.
     */
    void DoGetAccessPointsL(CApListItemList& aList) const;


private:    // data


    /**
     * ETrue if database has changed, but could not get new data yet.
     */
    TBool iUpdatePending;

    /**
     *Etrue if referesh needs to be done on next access to cached data
     *Added as a result of performance tuning
     */
    TBool iDelayedRefresh;


    /**
     * Access Point database.
     * Own
     */
    CActiveApDb* iDb;

    /**
     * Cached AP list. Since querying the AP Engine is expensive (slow),
     * we maintain a cached list of AP-s for quick access.
     * Own
     */
    CApListItemList* iCachedApList;

    /**
     * Pointer to active ap observer,
     * Own
     */
    CSwtBrowserApDbObserver* iObserver;

};

#endif // SWTBROWSERACTIVEAPDB_H
