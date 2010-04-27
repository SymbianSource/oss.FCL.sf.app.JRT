/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTSERVER_H
#define SWTSERVER_H


#include <e32base.h>
#include "eswt.h"


/**
 * CSwtServer
 * Server to execute methods within the UI thread
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtServer)
        : public CServer2
{
// Data types
public:
    enum TAction
    {
        EExecute = 0
    };
private:
    class CSwtSession;

// Methods
public:
    IMPORT_C static CSwtServer* NewL(TAny* aObj);
    IMPORT_C virtual ~CSwtServer();

    static TVersion     Version();

private:
    CSwtServer();

// From CServer2
protected:
    TInt RunError(TInt aError);

private:
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;
};


NONSHARABLE_CLASS(CSwtServer::CSwtSession)
        : public CSession2
{
public:
    inline CSwtSession(CSwtServer& aServer);

private:
    TInt DoServiceL(TAction aAction, const TAny* aParam);

// From CSession2
public:
    void ServiceL(const RMessage2& aMessage);

private:
    CSwtServer& iServer;
};


#endif // SWTSERVER_H
