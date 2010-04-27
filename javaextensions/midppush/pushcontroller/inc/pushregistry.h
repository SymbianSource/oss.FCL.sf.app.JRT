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
* Description:
*
*/


#ifndef PUSHREGISTRY_H
#define PUSHREGISTRY_H

#include <string>
#include <list>

namespace java
{
namespace util
{
class Uid;
}
namespace push
{
/**
 * This interface provides native side functionality for PushRegistry java class.
 */

class PushRegistry
{
public:

    virtual ~PushRegistry() {}

    /**
     * This operation creates ServerConnection object to the Server Connection Plugin
     * and stores new dynamic push connection to the push registry db.
     * @param aUri     Push connection URI. Cannot be empty. Syntax validation of this argument
     *                 is done in the java side by PushValidator interface.
     * @param aMidlet  Class name of the MIDlet. Cannot be empty.
     *                 Note: This operation does not validate whether value of this
     *                       argument match to the class name of the MIDlet.
     * @param aFilter  Connection URL indicating which senders are allowed to cause
     *                 the MIDlet to be launched. Cannot be empty. Syntax validation of this
     *                 argument is done in the java side by PushValidator interface.
     * @param aUid     UID of the application.
     * @param aIsUidOfThisMidlet indicates whether uid belongs to MIDlet where this
     *                           operation was called.
     * @throws         PushException with following error codes:
     *                 GENERAL_COMMS_ERROR:
     *                 sending a comms message failed.
     *                 INCORRECT_COMMS_MSG:
     *                 reading/writing to comms msg failed.
     *                 DB_ERROR:
     *                 storing dynamic push registration fails to the db.
     *                 COMMON_SRV_CONN_PLUGIN_ERROR:
    *                 creation of ServerConnection object failed.
    *                 PUSH_CONNECTION_ALREADY_EXISTS:
    *                 other push connection with same URI already exists.
    *                 note: this does not concern normal server connection.
     *                 LibraryLoaderException with following error codes:
     *                 OPENING_LIBRARY_FAILED if opening a Server Connection Plugin failed.
     *                                        E.g. in the situation where certain protocol
     *                                        is not supported.
     *                 CANNOT_FIND_FUNCTION_FAILURE if function function by "getServerConnectionFactory"
     *                                        name is not found from ServerConnection Plugin.
     */
    virtual void registerDynamicPushConn(const std::wstring& aUri,
                                         const std::wstring& aMidlet,
                                         const std::wstring& aFilter,
                                         const java::util::Uid& aUid,
                                         bool aIsUidOfThisMidlet) = 0;

    /**
     * This operation returns all static and dynamic push connections of the MIDlet.
     * @param aAvailable 'true' means that only active push URIs is returned.
     *                   I.e. URIs where message has arrived.
     *                   'false' means that all static and dynamic push registrations
     *                   of the MIDlet is returned.
     * @param aUid       UID of the MIDlet Suite.
     * @param aUriList   Output parameter. List of URIs.
     * @throws           This operation does not throw exceptions.
     *
     */
    virtual void listConnections(bool aAvailable,const java::util::Uid& aUid,
                                 std::list<std::wstring>& aUriList) = 0;

    /**
     * This operation returns filter of the push registration.
     * @param aUri push connection URI.
     * @param aSuiteUid UID of the MIDlet suite.
     * @return filter of the push registration.
     * @throws  PushException with following error codes:
     *          SRV_CONN_NOT_FOUND:
     *          Push registration does not exists.
     */
    virtual std::wstring getFilter(const std::wstring& aUri,
                                   const java::util::Uid& aSuiteUid) = 0;

    /**
     * This operation returns filter of the push registration.
     * @param aUri push connection URI.
     * @param aSuiteUid UID of the MIDlet suite.
     * @return filter of the push registration.
     * @throws  PushException with following error codes:
     *          SRV_CONN_NOT_FOUND:
     *          Push registration does not exists.
     */
    virtual std::wstring getClassNameOfMidlet(const std::wstring& aUri,
            const java::util::Uid& aSuiteUid) = 0;

    /**
     * This operation is used in the implementation of PushRegistry.getMIDlet() operation.
     * This operation checks whether this MIdlet has push registrations.
     * This operation does not thrown exception.
     * @param aUri push connection URI.
     * @param aUid UID of the application.
     * @return 'true' if aUri matches to the push connection.
     */
    virtual bool isPushConnection(const std::wstring& aUri,
                                  const java::util::Uid& aUid) = 0;

    /**
     * This operation deletes dynamic push registration from the db and changes
     * existing (if exists) type of the ServerConection to normal server connection.
     * @param   aUri push connection URI.
     * @param   aUid UID of the application.
     * @param aIsUidOfThisMidlet indicates whether uid belongs to MIDlet where this
           *                           operation was called.
     * @throws  PushException with following error codes:
     *          GENERAL_COMMS_ERROR:
           *          sending a comms message failed.
           *          INCORRECT_COMMS_MSG:
           *          reading/writing to comms msg failed.
           *          NOT_DYNAMIC_PUSH_URI:
     *          URI is static push URI.
           *          URI_BELONGS_TO_OTHER_MIDLET:
           *          Push registration has been reserved for other MIDlet.
           *          SRV_CONN_NOT_FOUND:
           *          Server connection is not found from the push registry db.
           *          DB_ERROR:
           *          Database error occurred.
     */
    virtual void unregisterDynamicPushConn(const std::wstring& aUri,
                                           const java::util::Uid& aUid,
                                           bool aIsUidOfThisMidlet) = 0;

    /**
     * This operation register a alarm time to launch a MIDlet.
     * @param aAlarmTimeInMillSecs MIDlet's launch time in milliseconds.
     * @param aSuiteUid UID of the MIDlet suite.
     * @param aClassNameOfMidlet class name of the MIDlet.
     * @param aIsUidOfThisMidlet indicates whether uid belongs to MIDlet where this
           *                           operation was called.
     * @throws  PushException with following error codes:
     *          GENERAL_COMMS_ERROR:
     *          sending a comms message failed.
     *          INCORRECT_COMMS_MSG:
     *          Incorrect comms message.
     *          INVALID_ALARM_TIME:
     *          alarm time is less than current time.
     *          DB_ERROR:
     *          Database error occurred during storing alarm time.
     */
    virtual long long registerAlarm(const long long& aAlarmTimeInMilliSecs,
                                    const java::util::Uid& aSuiteUid,
                                    const std::wstring& aClassNameOfMidlet,
                                    bool aIsUidOfThisMidlet) = 0;
};

}//end namespace push
}//end namespace java

#endif // PUSHREGISTRY_H

