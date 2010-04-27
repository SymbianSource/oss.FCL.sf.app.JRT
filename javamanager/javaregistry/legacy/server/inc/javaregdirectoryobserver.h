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
* Description:  javaregdirectoryobserver definition
*
*/


#ifndef JAVAREGDIRECTORYOBSERVER_H
#define JAVAREGDIRECTORYOBSERVER_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include "mjavaregcacheupdater.h"

namespace Java
{
namespace Manager
{
namespace Registry
{


/**
 * This class monitors the changes in the directory given as parameter,
 * and notifies the store to update it's cache accordingly.
 *
 * @since S60 v3.2
 */
// CLASS DECLARATION
NONSHARABLE_CLASS(CJavaRegDirectoryObserver) :
        public CActive
{

public:

    /**
     * Creates an object of this class.
     *
     * @since S60 v3.2
     * @param aCacheUpdater The object to notify about the changes.
     * @param aFs File server session, to subscribe to changes.
     * @param aDrive The drive, wich the path in the registry represents.
     * @param aPath The path to monitor.
     */
    static CJavaRegDirectoryObserver* NewL(
        MJavaRegCacheUpdater& aCacheUpdater,
        RFs& aFs,
        TInt aDrive,
        const TDesC& aPath);

    /**
     * Creates an object of this class.
     *
     * @since S60 v3.2
     * @param aCacheUpdater The object to notify about the changes.
     * @param aFs File server session, to subscribe to changes.
     * @param aDrive The drive, wich the path in the registry represents.
     * @param aPath The path to monitor.
     */
    static CJavaRegDirectoryObserver* NewLC(
        MJavaRegCacheUpdater& aCacheUpdater,
        RFs& aFs,
        TInt aDrive,
        const TDesC& aPath);

    /**
     * Destructor
     * @since S60 v3.2
     */
    ~CJavaRegDirectoryObserver();

    /**
     * Refreshes the iEntryList member.
     *
     * @since S60 v3.2
     */
    void RefreshEntryListL();

    /**
     * Returns the iPath member.
     *
     * @since S60 v3.2
     * @return The iPath member.
     */
    const TDesC& Path() const;

    /**
     * Sets the iPath member to the given parameter value.
     *
     * @since S60 v3.2
     * @param aPath The path to monitor.
     */
    void SetPathL(const TDesC& aPath);

    /**
     * Returns the iDrive member.
     *
     * @since S60 v3.2
     * @return The iDrive member..
     */
    TInt Drive() const;

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
     * RunError
     */
    TInt RunError(TInt aError);

private:

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aCacheUpdater The object to notify about the changes.
     * @param aFs File server session, to subscribe to changes.
     * @param aDrive The drive, wich the path in the registry represents.
     */
    CJavaRegDirectoryObserver(
        MJavaRegCacheUpdater& aCacheUpdater,
        RFs& aFs,
        TInt aDrive);

    /**
     * 2nd phase constructor.
     *
     * @since S60 v3.2
     * @param aPath The path to monitor.
     */
    void ConstructL(const TDesC& aPath);

private:

    // The object to notify about the changes.
    MJavaRegCacheUpdater& iCacheUpdater;

    // File server session, to subscribe to changes.
    RFs& iFs;

    // The drive, wich the path in the registry represents.
    TInt iDrive;

    // The path to monitor.
    TPath iPath;

    // The file-entrylist of the iPath directory.
    CDir* iEntryList;

};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGDIRECTORYOBSERVER_H

// End of File
