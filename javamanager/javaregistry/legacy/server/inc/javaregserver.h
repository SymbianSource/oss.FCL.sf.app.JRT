/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregserver definition
*
*/


#ifndef JAVAREGSERVER_H
#define JAVAREGSERVER_H

#include <e32base.h>

#include "javaregdef.h"
#include "javaregburstates.h"

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATIONS
class CJavaRegServerSession;
class CJavaRegStore;
class CJavaPropertyArray;

/**
 * Java Registry Server class.
 *
 * @since S60 v3.2
 */
class CJavaRegServer :
        public CPolicyServer
{

public:

    /**
     * Creates new java registry server object.
     */
    static CJavaRegServer* NewL();

    /**
     * Destructor
     */
    virtual ~CJavaRegServer();

    /**
     * Sends panic to client.
     *
     * @since S60 v3.2
     * @param aMessage client message
     * @param aPanic panic code
     */
    void PanicClient(const RMessage2& aMessage, TInt aPanic) const;

    /**
     * Checks if an entry with specified uid exist or not
     * in the registry.
     *
     * @since S60 v3.2
     * @param aUid uid of the entry to be checked
     * @return true if the entry exists and false if not
     */
    TBool EntryExistsL(TUid aUid);

    /**
     * Returns the stored uids for the given drive.
     *
     * @since S60 v3.2
     * @param[out] aUids stored uids are returned in this parameter
     * @param aDrive drive whose entries must be returned
     */
    void GetEntryUidsL(RArray<TUid>& aUids, TInt aDrive);

    /**
     * Gets entry for the given uid.
     * If no entry found for the given uid aProperties is null else
     * it is allocated a new CJavaPropertyArray object containing the read
     * entry.
     *
     * @since S60 v3.2
     * @param aUid uid of the required entry
     * @param[out] aProperties entry are read in this parameter
     */
    void GetEntryL(
        TUid aUid,
        CJavaPropertyArray*& aProperties, TBool aAllEntries = EFalse);

    /**
     * Returns with a set of uids whose entries contains the properties (and
     * has the same values) given in the aProperties parameter.
     *
     * @since S60 v3.2
     * @param aProperties the set of properties which must exists
     *          (and must have the same values) in the looked entries
     * @param[out] aFoundUids uids of the found entries are returned
     *          in this parameter
     */
    void FindEntriesL(CJavaPropertyArray& aProperties,
                      RArray<TUid>& aFoundUids);

    /**
     * Returns with a set of uids whose entries contains the properties (and
     * has the same values) given in the aProperties parameter. This version
     * returns all uids including whose represent midlets in not presented
     * removeable media
     *
     * @since S60 v5.0
     * @param aProperties the set of properties which must exists
     *          (and must have the same values) in the looked entries
     * @param[out] aFoundUids uids of the found entries are returned
     *          in this parameter
     */
    void FindAllEntriesL(CJavaPropertyArray& aProperties,
                         RArray<TUid>& aFoundUids);

    /**
     * Sets backup state.
     *
     * @since S60 v3.2
     * @param aState backup state to be set
     */
    void SetBackupState(TBackupState aState);

    /**
     * Check backup state.
     * In case of server state is backing up or restoring this method leaves
     * with the KErrServerBusy.
     *
     * @since S60 v3.2
     */
    void LeaveIfBackingUpOrRestoringL();

    /**
     * Check backup state.
     * In case of server state is restoring this method leaves
     * with the KErrServerBusy.
     *
     * @since S60 v3.2
     */
    void LeaveIfRestoringL();

private:

    /**
     * Constructor
     */
    CJavaRegServer();

    /**
     * ConstructL
     */
    void ConstructL();

private:    // From CPolicyServer

    /**
     * Creates a new server session object.
     */
    CSession2*  NewSessionL(const TVersion&  aVersion,
                            const RMessage2&  aMessage) const;

    /**
     * Performs a custom security check for the setter methods.
     *
     * @since S60 v3.2
     * @param aMsg The message to check.
     * @param aAction A reference to the action to take if the security
     *                check fails.
     * @param aMissing A reference to the list of security attributes
     *                 missing from the checked process.
     */
    virtual TCustomResult CustomSecurityCheckL(
        const RMessage2&  aMsg,
        TInt&  aAction,
        TSecurityInfo&  aMissing);

private:    // data

    // state of backup
    TBackupState iBackupState;

    // java registry store object
    CJavaRegStore* iStore;

};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGSERVER_H

