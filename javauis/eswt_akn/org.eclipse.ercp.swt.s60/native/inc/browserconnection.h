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


#ifndef BROWSERCONNECTION_H
#define BROWSERCONNECTION_H




class RSocketServ;
class RConnection;


/**
 * MBrowserConnection
 * This class provides a interface for the eSWT Browser towards a concrete
 * implementation of a eSWT browser connection manager.
 * @lib eswt
 */
NONSHARABLE_CLASS(MBrowserConnection)
{
public:
    /**
     * Sets RequestedAP
     * @param aRequestedAP the requested AP id
     */
    virtual void SetRequestedAP(TUint32 aRequestedAPId) = 0;

    /**
     * A query function to find out whether there is a connection which
     * was matches with the "current" connection parameters
     * @return ETrue if the condition above is satisfied EFalse otherwise.
     */
    virtual TBool Connected() = 0;

    /**
     * Returns the currently (or the last used) AP id
     * @return the currently (or the last used) AP id
     */
    virtual TUint32 CurrentAPId() const = 0;

    /**
     * Call this the get the current bearer to be used
     * @return the bearer type associated with the current AP
     */
    virtual TInt CurrentBearerType() const = 0;

    /**
     * Closes the connection and sets the manager to offline mode
     */
    virtual void Disconnect() = 0;

    /**
     * Starts the connection process
     * @return a error code
     */
    virtual TInt StartConnectionL() = 0;

    /**
     * Returns the RConnection object used to create connection.
     * @return RConnection object
     */
    virtual RConnection& Connection() = 0;

    /**
     * Return name of the connection created.
     * Ownership of name is handed over.
     * @return name of the connection
     */
    virtual TName* ConnectionNameL() = 0;

    /**
     * Return socket server
     * @return socket server
     */
    virtual RSocketServ& SocketServer() = 0;
};

#endif // BROWSERCONNECTION_H
