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


#ifndef SWTBROWSERCONNECTIONOBSERVER_H
#define SWTBROWSERCONNECTIONOBSERVER_H


#include <es_sock.h>


/**
 * Cconnection stage observer
 */
class MConnectionStageObserver
{
public:
    virtual void ConnectionStageAchievedL() = 0;
};


/**
 * CSwtBrowserConnectionObserver
 * This is the notifier class which listens to connection stage changes.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserConnectionObserver)
        : public CActive
{
public:
    /**
     * Constructor
     */
    CSwtBrowserConnectionObserver();

    /**
     * Destructor
     */
    virtual ~CSwtBrowserConnectionObserver();

    /**
     * Starts the notification service
     * after it has been started you can expect a notification
     * @param aConnName connection name returned by RConnection::Name()
     * @param aStageToObserve the stage you want to notified about
     * @param aObserver the observer you want to register for notification
     * @param aCompleteIfAlreadyAtStage if ETrue then you get a notification even
     * if the connection is in the stage you defined when you start the service
     */
    void StartNotificationL(TName* aConnName,
                            TInt aStageToObserve,
                            MConnectionStageObserver* aObserver,
                            TBool aCompleteIfAlreadyAtStage= ETrue);

protected:
    /**
     * Attach to connection, the name of which is the aConnName.
     * @param aConnName connection name returned by RConnection::Name()
     */
    void DoOpenAgentL(TName* aConnName);

    /**
     * Detach from connection.
     */
    void DoCloseAgent();

private:

// From CActive
    void DoCancel();
    void RunL();

private://Data
    /**
     * The socket server
     */
    RSocketServ iServer;

    /**
     * The connection on the question
     */
    RConnection iConnection;

    /**
     * The object that keeps connection stage info.
     */
    TNifProgressBuf iProgressBuf;

    /**
     * The connection stage observer
     */
    TInt iStageToObserve;

    /**
     * The connection stage observer
     */
    MConnectionStageObserver* iObserver;
};


#endif // SWTBROWSERCONNECTIONOBSERVER_H
