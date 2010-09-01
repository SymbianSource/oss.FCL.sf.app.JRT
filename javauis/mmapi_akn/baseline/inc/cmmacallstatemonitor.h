/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Publish and Subscribe key watcher
*
*/


#ifndef CMMACALLSTATEMONITOR_H
#define CMMACALLSTATEMONITOR_H

#include <e32property.h>

class MMMACallStateObserver;

/**
 *  Publish and subscribe key watcher.
 *
 *  @lib 
 */
class CMMACallStateMonitor : public CActive
    {
public:

    /*
    * Two Phased Constructor.
    *
    * @since 
    * @param aUid PubSub UID to watch
    * @param aKey specific key to watch
    * @return Construced object
    */
    static CMMACallStateMonitor* NewL( TUid aUid, TInt aKey,
                                            MMMACallStateObserver* aObserver );

    /*
    * virtual destructor.
    *
    * @since 
    */
    virtual ~CMMACallStateMonitor();

public:     // New Functions

    /*
    * Return the value of the key.
    *
    * @since 
    * @param aValue
    * @return KErrNone, if successful otherwise system error
    */
    TInt GetValue( TInt& aValue );
    
protected:  // From Base class

    /*
    * From CActive.
    *
    * @since 
    */
    void RunL();
    
    /*
    * From CActive.
    *
    * @since 
    */
    void DoCancel();
    
private:

    /*
    * Default Constructor.
    *
    * @since 
    * @param aUid PubSub key to watch
    * @param aKey specific key to watch
    */
    CMMACallStateMonitor( TUid aUid,
                      TInt aKey,
                      MMMACallStateObserver* aObserver );

    /*
    * Second phased constructor.
    *
    * @since 
    */
    void ConstructL();

private: // data
    
    RProperty             iProperty;
    MMMACallStateObserver*    iObserver;  // not owned
    TUid                  iUid;       // Uid that we are watching
    TInt                  iKey;       // Key that we are watching
    };

#endif // C_MMAPSKEYWATCHER_H