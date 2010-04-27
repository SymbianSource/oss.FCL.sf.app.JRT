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


#ifndef SWTBROWSERCONNECTOR_H
#define SWTBROWSERCONNECTOR_H


#include <nifman.h>


class TCommDbConnPref;


/**
 * CSwtBrowserConnector
 * This class plays the role of a high level
 * state machine in the connection creation
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserConnector)
        : public CActive
{
public:
    /**
     * Constructor
     * @param aAgent an RGenerericAgent
     * @param aPriority the priority of the active object
     */
    CSwtBrowserConnector(RConnection& aConnection,
    TInt aPriority = EPriorityStandard);

    /**
     * Destructor
     */
    virtual ~CSwtBrowserConnector();

    /**
     * This will activate the active object and start the connection creation process
     * @param aSettings the overridesettings containing the connection characteristics
     * @param aStatus the status of the observer Active object
     */
    void StartConnection(TCommDbConnPref* aSettings, TRequestStatus& aStatus);

private:
// From CActive
    void DoCancel();
    void RunL();

private: // Data
    /**
     * External request status
     * Not own
     */
    TRequestStatus* iExternalRequestStatus;

    /**
     * The connection on the question
     * Not own
     */
    RConnection& iConnection;
};


/**
 * CSwtBrowserConnectorSyncWrapper
 * This is a wrapper class on top of
 * CSwtBrowserConnector enabling asynch->synch conversion
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtBrowserConnectorSyncWrapper)
        : public CActive
{
public:
    /**
     * Symbian OS constructor
     * @param aAgent a RGenerericAgent
     * @param aPriority the priority of the active object
     */
    static CSwtBrowserConnectorSyncWrapper * NewL(
        RConnection& aConnection, TInt aPriority = EPriorityStandard);

    /**
     * Symbian OS constructor
     * @param aAgent a RGenerericAgent
     * @param aPriority the priority of the active object
     */
    static CSwtBrowserConnectorSyncWrapper * NewLC(
        RConnection& aConnection, TInt aPriority = EPriorityStandard);

    /**
     * This will activate the active object and start the connection creation process
     * @param aSettings the overridesettings containing the connection characteristics
     * @return the error code
     */
    TInt Connect(TCommDbConnPref* aSettings);

    /**
     * Destructor
     */
    virtual ~CSwtBrowserConnectorSyncWrapper();

private:
// From CActive
    void RunL();
    void DoCancel();

protected:
    /**
     * Symbian OS second phase constructor
     * @param aAgent a RGenericAgent handle
     */
    void ConstructL(RConnection& aConnection);

    /**
     * Constructor
     * @param aPiority the priority of the active object
     */
    CSwtBrowserConnectorSyncWrapper(TInt aPriority);

private: //Data

    /**
     * The object for synchronizing connection operation
     */
    CActiveSchedulerWait iWait;

    /**
     * The ative connector
     * Own
     */
    CSwtBrowserConnector* iActiveConnector;
};

#endif // SWTBROWSERCONNECTOR_H
