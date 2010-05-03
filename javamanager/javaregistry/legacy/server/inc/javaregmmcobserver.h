/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregmmcobserver definition
*
*/


#ifndef JAVAREGMMCOBSERVER_H
#define JAVAREGMMCOBSERVER_H

#include <f32file.h>


namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATIONS
class MMemoryCardObserver;

/**
 * CMmcObserver class registers to file server for receiving mmc status
 * change events and passes this events to the registered observer object.
 *
 * @since S60 v3.2
 */
class CMmcObserver : public CActive
{
public:

    /**
     * Creates new mmc observer object.
     *
     * @since S60 v3.2
     * @param aDrive drive number to watch
     * @param aObserver observer object
     * @param aFileServerSession file server session object
     */
    static CMmcObserver* NewL(TInt aDrive,
                              MMemoryCardObserver* aObserver,
                              RFs* aFileServerSession);

    /**
     * Creates new mmc observer object and leave it on cleanup stack
     *
     * @since S60 v3.2
     * @param aDrive drive number to watch
     * @param aObserver observer object
     * @param aFileServerSession file server session object
     */
    static CMmcObserver* NewLC(TInt aDrive,
                               MMemoryCardObserver* aObserver,
                               RFs* aFileServerSession);

    /**
     * Starts MMC event observing.
     *
     * @since S60 v3.2
     */
    void StartObserver();

    /**
     * Gets the id of the inserted mmc.
     *
     * @since S60 v3.2
     * @return the unique id of the inserted MMC or 0 if MMC not present
     */
    TUint MmcId();

public:     // from CActive

    /**
     * RunL
     */
    void RunL();

    /**
     * DoCancel
     */
    void DoCancel();

    /**
     * Destructor
     */
    ~CMmcObserver();

private:

    /**
     * ConstructL
     *
     * @since S60 v3.2
     * @param aObserver observer object
     * @param aFileServerSession file server session object
     */
    void ConstructL(MMemoryCardObserver* aObserver, RFs* aFileServerSession);

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aDrive drive number to watch
     */
    CMmcObserver(TInt aDrive);

private:    //data

    // observer object
    MMemoryCardObserver* iObserver;

    // file server session object
    RFs* iFsSession;

    // drive number we are watching
    TInt iDrive;

    // notification path
    TPath iNotificationPath;

};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGMMCOBSERVER_H

// End of File
