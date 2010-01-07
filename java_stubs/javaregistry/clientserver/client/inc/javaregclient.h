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
* Description:  javaregclient definition
*
*/


#ifndef __JAVAREGCLIENT_H
#define __JAVAREGCLIENT_H

#include "javaregdef.h"
#include "javaregburstates.h"

namespace Java
{
namespace Manager
{
namespace Registry
{
// FORWARD DECLARATIONS
class CJavaPropertyArray;

/**
 * RJavaRegSession class makes it possible to access java registry server
 * services like adding, getting and removing entries.
 * The class implements the client side of java registry
 * client-server communication.
 *
 * @since S60 v3.2
 */
class RJavaRegSession : public RSessionBase
{

public:

    /**
     * Constructor
     *
     * @since S60 v3.2
     */
    RJavaRegSession();

    /**
     * Connects to registry server and sets integrity service's
     * transaction id.
     *
     * @since S60 v3.2
     * @param aTransactionId transaction id of integrity service
     * @param aLegacy legacy flag, must be set to true if
     *          legacy api wants to use server services
     * @return KErrNone if connection was successful or an error code
     */
    TInt Connect(TInt64 aTransactionId, TBool aLegacy = EFalse);

    /**
     * Connects to registry server and sets if integrity service
     * is required or not to be created in registry.
     *
     * @since S60 v3.2
     * @param aUseIntegrity true if registry should create an
     *          interior integrity session
     * @param aLegacy if true legacy api uses server services
     * @return KErrNone if connection was successful or an error code
     */
    TInt Connect(TBool aUseIntegrity, TBool aLegacy = EFalse);

    /**
     * Checks if an entry with specified uid exist or not
     * in the registry.
     *
     * @since S60 v3.2
     * @param aUid uid of the entry to be checked if exists
     * @return true if the entry exists and false if not
     */
    TBool EntryExistsL(TUid aUid) const;

    /**
     * Gets uids of entries stored in registry.
     *
     * @since S60 v3.2
     * @param[out] aUids reference to an array where stored uids
     *          are returned
     */
    void GetEntryUidsL(RArray<TUid>& aUids) const;

    /**
     * Gets uids of entries stored in registry and belonging to
     * the given drive.
     *
     * @since S60 v3.2
     * @param[out] aUids reference to an array where stored uids
     *          are returned
     * @param aDrive drive whose entry uids must be returned
     */
    void GetEntryUidsL(RArray<TUid>& aUids, TInt aDrive) const;

    /**
     * Gets the entry belonging to the given uid.
     * The entry is represented as a set of properties.
     * If no entry found for the given uid aProperties is null else
     * it is allocated a new CJavaPropertyArray object containing the read
     * entry.
     *
     * @since S60 v3.2
     * @param aUid the uid of entry we are interested in
     * @param[out] aProperties entry is read in this parameter
     * @param aAllEntries decision if we want to read entry from not
     *                    presented MMC
     */
    void GetEntryL(TUid aUid,
                   CJavaPropertyArray*& aProperties,
                   TBool aAllEntries = EFalse) const;

    /**
     * Adds a new entry or updates an existent one with the given uid.
     * The entry is represented as a set of properties.
     *
     * @since S60 v3.2
     * @param aUid the uid of the entry to be added or updated
     * @param aProperties the entry to be added or updated
     * @param aDrive the drive to which the set entry belongs
     */
    void SetEntryL(TUid aUid,
                   const CJavaPropertyArray& aProperties,
                   TInt aDrive) const;

    /**
     * Returns a set of uids whose entries contains the properties (and
     * has the same values) given in the aProperties parameter.
     *
     * @since S60 v3.2
     * @param aProperties the set of properties which must exists
     *          (and must have the same values) in the looked entries
     * @param[out] aUids uids of the found entries are returned
     *          in this parameter
     * @param aAllEntries means, that method give all entries including
     *          entries, which regards with midlets stored in not presented
     *          media. EFalse means entries with regarding not presented
     *          media will not return.
     */
    void FindEntriesL(const CJavaPropertyArray& aProperties,
                      RArray<TUid>& aUids, TBool aAllEntries = EFalse) const;

    /**
     * Removes the entry belonging to the given uid.
     *
     * @since S60 v3.2
     * @param aUid uid of entry which must be deleted
     * @param aAllEntries means, that method give all entries including
     *          entries, which regards with midlets stored in not presented
     *          media. EFalse means entries with regarding not presented
     *          media will not return.
     */
    void RemoveEntryL(TUid aUid, TBool aAllEntries = EFalse) const;

    /**
     * Allocates a range of new uids.
     *
     * @since S60 v3.2
     * @param[out] aUids array reference where the allocated uids
     *          will be returned
     * @param aUidCount number of uids to be allocated
     */
    void AllocateUidsL(RArray<TUid>& aUids,
                       TInt aUidCount = 1) const;

    /**
     * Sets the backup state of registry.
     * If the registry is in backing up or restoring state
     * the client requests will not be executed.
     *
     * @since S60 v3.2
     * @param aBackupState the requested backup state
     */
    void SetBackupStateL(TBackupState aBackupState) const;

private:

    /**
     * Initializes server with integrity service transaction id
     * and the legacy api flag.
     *
     * @since S60 v3.2
     * @param aTransactionId integrity service transaction id
     * @param aLegacy legacy flag, must be set to true if
     *          legacy api wants to use server services
     * @return KErrNone if no error or the appropriate error code
     */
    TInt InitializeServer(TInt64 aTransactionId, TBool aLegacy) const;

    /**
     * Initializes server with use integrity session flag,
     * and the legacy api flag.
     *
     * @since S60 v3.2
     * @param aUseIntegrity true if registry should create an interior
     *          integrity session
     * @param aLegacy legacy flag, must be set to true if
     *          legacy api wants to use server services
     * @return KErrNone if no error or the appropriate error code
     */
    TInt InitializeServer(TBool aUseIntegrity, TBool aLegacy) const;

    /**
     * Sends to server a message containing the function id and the
     * arguments given in TIpcArgs argument.
     * The first and second arguments of TIpcArgs are reserved.
     *
     * @since S60 v3.2
     * @param aFunction funtion id
     * @param[out] aBuffer the received buffer is returned
     *          in this parameter
     * @param aArgs arguments to be sent for server, first and second
     *          argument must be left empty
     */
    void GetBuffer8FromServerL(TInt aFunction,
                               HBufC8*& aBuffer,
                               TIpcArgs& aArgs) const;

    /**
     * Sends to server a message containing the function id and the
     * arguments given in TIpcArgs argument.
     * The first and second arguments of TIpcArgs are reserved.
     *
     * @since S60 v3.2
     * @param aFunction funtion id
     * @param[out] aBuffer the received buffer is returned
     *          in this parameter
     * @param aArgs arguments to be sent for server, first and second
     *          argument must be left empty
     */
    void GetBuffer16FromServerL(TInt aFunction,
                                HBufC16*& aBuffer,
                                TIpcArgs& aArgs) const;

    /**
     * Sends to server a message containing the function id and the
     * arguments given in TIpcArgs argument.
     * The first and second arguments of TIpcArgs are reserved.
     *
     * @since S60 v3.2
     * @param aFunction funtion id
     * @param[out] aBuffer the received buffer is returned
     *          in this parameter
     * @param aArgs arguments to be sent for server, first and second
     *          argument must be left empty
     * @param a8BitDesc if true a 8 bit descriptor must be allocated
     *          in other case a 16 bit descriptor must be allocated
     */
    void GetBufferFromServerL(TInt aFunction,
                              TAny** aBuffer,
                              TIpcArgs& aArgs,
                              TBool a8BitDesc) const;
    /**
     * Connects to server.
     *
     * @since S60 v3.2
     * @return KErrNone if connection was successful or an error code
     */
    TInt Connect();

private:

};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif //__JAVAREGCLIENT_H
