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
* Description:  javaregserversession definition
*
*/


#ifndef JAVAREGSERVERSESSION_H
#define JAVAREGSERVERSESSION_H

#include <e32base.h>

namespace Java
{
namespace Manager
{
namespace Registry
{

// FORWARD DECLARATIONS
class CJavaRegServer;
class CJavaPropertyArray;

/**
 * Java Registry server session object.
 * For each connected client it is created a new object of this class.
 * This class is responsible to process the messages received from clients.
 *
 * @since S60 v3.2
 */
class CJavaRegServerSession : public CSession2
{

public:

    /**
     * Constructor
     */
    CJavaRegServerSession();

    /**
     * Destructor
     */
    ~CJavaRegServerSession();

    /**
     * Returns the member variable: iLegacy
     *
     * @since S60 v3.2
     * @return The member iLegacy.
     */
    TBool IsLegacy() const;

private:

    /**
     * Returns server object.
     *
     * @since S60 v3.2
     */
    CJavaRegServer* Server() const;

    /**
     * Handles the servicing of a client request that has been passed
     * to the server.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     */
    void ServiceL(const RMessage2& aMessage);

    /**
     * Processes the messages received from clients.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     */
    void DispatchMessageL(const RMessage2&  aMessage);

    /**
     * Reads use integrity session flag from client message and creates
     * a new integrity session if needed.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     */
    void InitializeServerUseIntegrL(const RMessage2& aMessage);

    /**
     * Reads from client message uid and check if entry
     * exists belonging to this uid.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     */
    void EntryExistsL(const RMessage2& aMessage);

    /**
     * Gets stored uids and write it back in client message.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     */
    void GetEntryUidsL(const RMessage2& aMessage);

    /**
     * Reads uid from client message. Gets entry for given uid
     * and writes back the entry to client message.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     * @param aAllEntries contain information if method gives all entries
     *          including entries from not presented MMC
     */
    void GetEntryL(const RMessage2& aMessage, TBool aAllEntries = EFalse);

    /**
     * Reads find criteria from client message. Looks for entries
     * and writes back the found entries to client message.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     */
    void FindEntriesL(const RMessage2& aMessage);

    /**
     * Reads find criteria from client message. Looks for entries
     * and writes back the found entries to client message. This version
     * looks for entris which are related to midlets in not presented media
     *
     * @since S60 v5.0
     * @param aMessage message containing the details of client request
     */
    void FindAllEntriesL(const RMessage2& aMessage);

    /**
     * Reads from client message the backup state and sets it for server.
     *
     * @since S60 v3.2
     * @param aMessage message containing the details of client request
     */
    void SetBackupState(const RMessage2& aMessage);

    /**
     * Obtains uid from the given message.
     *
     * @since S60 v3.2
     * @param aIndex index where uid can be found in message
     * @param aMessage message containing the uid
     * @return the obtained uid
     */
    TUid UidFromMessageL(TInt aIndex, const RMessage2&  aMessage);

    /**
     * Obtains property array from the given message.
     *
     * @since S60 v3.2
     * @param aIndex index where property array can be found in message
     * @param aMessage message containing the property array
     * @return the obtained property array
     */
    CJavaPropertyArray* PropertiesFromMessageL(TInt aIndex,
            const RMessage2& aMessage);

    /**
     * Writes in client allocated 16 bit buffer the descriptor containing
     * the array of uids encoded and send back to client the length of
     * this buffer to be able for client to allocate a bigger descriptor
     * if needed.
     *
     * @since S60 v3.2
     * @param[out] aUids uid array to be written
     * @param aMessage client-server message reference
     */
    void WriteBackUidArrayL(RArray<TUid>& aUids,
                            const RMessage2& aMessage);

private:    //data

    // flag indicating if the session is initiated by the backup process
    TBool iBackupSession;

    // legacy flag, indicates if legacy api is the requester of
    // server services or not
    TBool iLegacy;

};

}//namespace Registry
}//namespace Manager
}//namespace Java

#endif // JAVAREGSERVERSESSION_H
