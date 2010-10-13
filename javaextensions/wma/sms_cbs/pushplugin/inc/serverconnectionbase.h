/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SERVERCONNECTIONBASE_H
#define SERVERCONNECTIONBASE_H

#include "serverconnection.h"
#include "javasmsattributes.h"


namespace java
{
namespace wma
{
/**
 *  ServerConnectionBase abstract class extends interface  ServerConnection.
 *  It defines the interfaces that are to be implemented by the corresponding
 *  protocol ServerConnection class.
 *
 *  @smsserverconnection.dll
 *  @
 */

class ServerConnectionBase: public java::push::ServerConnection
{

public:
    /**
     * factory method to create the server connection
     * @param[in] aUri - uri on which the server connection to be opened
     * @param[in] aFilter - filter to allow messages to be received from
     * desired sender.
     * @return ServerConnection
     */
    static ServerConnection* getServerConnection(const std::wstring& aUri,
            const std::wstring& aFilter);

    virtual ~ServerConnectionBase()
    {

    }
    /**
     * Opens the connection. It registers the server URI to the messaging system
     *  for receiving messages.
     * Throws COMMON_SRV_CONN_PLUGIN_ERROR exception if there is an error while
     * registering the connection.
     * @param[in] aListener: Listener to be notified when message arrives.
     * @param[in] aAppLaunched : bool to notify listening by midlet or push.
     */
    OS_IMPORT virtual void open(java::push::ConnectionListener* aListener,
                                bool aAppLaunched) =0;
    /**
     * from ServerConnection
     * Return the push register URI
     * @return uri as std::wstring
     */
    OS_IMPORT virtual std::wstring getUri() const;
    /**
     * from ServerConnection
     * Sets the Filter
     * @param aFilter - filter which defines the acceptable messages
     * @return
     */
    OS_IMPORT virtual void setFilter(const std::wstring& aFilter);
    /**
     * from ServerConnection
     * Return the filter string
     * @return std::wstring
     */
    OS_IMPORT virtual std::wstring getFilter() const;
    /**
     * Return the number of messages that have arrived and to be read by the
     * application.
     * @return int - number of messages that needs to be read by application
     */
    OS_IMPORT int getMessagesOnStore() const;
    /**
     * retrieves the received message
     * @param[out] aSmsBuf:the message buffer into which the received message is
     * written into.
     * @return SUCCESS or FAILURE status
     */
    virtual int retrieveMessage(TJavaMessageParametersBuf& aSmsBuf)=0;
protected:
    //Constructor
    ServerConnectionBase(const std::wstring& aUri, const std::wstring& aFilter);
    /**
     * Deletes a message on store.
     */
    void deleteMessage();

    /**
     * Creates a Message store , to save incoming messages
     * so that the application can read the message from the store.
     * @param path:the message store path
     * @returns SUCCESS or FAILURE
     */
    int createMessageStore(std::wstring aPath, bool aDeleteStore);

    /**
     * Removes the directory specified.
     * @param dirPath: the directory that has to be removed
     * @returns SUCCESS or FAILURE
     */
    int removeDir(const std::wstring aDirPath);
    /**
     * Creates the specified directory if it doesn't exists .
     * @param dirPath the directory path
     * @returns SUCCESS or FAILURE
     */
    int makeDirAll(const std::wstring aDirPath);

protected:
    // Flag to indicate whether the server connection is opened by push or by
    // application;
    bool mIsAppLaunched;
    // Flag to indicate whether listening or not
    bool mIsListening;
    //port on which the push connection is opened
    int mPort;
    // Uri of the push connection
    std::wstring mUri;
    //Filter
    std::wstring mFilter;
    //the number of messages in store
    int mMessagesOnStore;
    //next message to be stored
    int mNextMessageInStore;
    //first message in store
    int mFirstMessageInStore;
    //message store
    std::wstring mMessageStoreDirName;
};

}// end of namespace wma
}// end of namespace java
#endif // SERVERCONNECTIONBASE_H

