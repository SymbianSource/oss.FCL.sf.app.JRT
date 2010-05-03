/*
* Copyright (c) 1999-2005 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef JESSERVER_H
#define JESSERVER_H


/**
 * Event server async shutdown interface.
 */
class MJesShutdown
{
public:
    virtual void Shutdown() = 0;
};

/**
 * Event server async shutdown handler.
 */
class CJesShutdown : public CActive
{
public:
    CJesShutdown(MJesShutdown*);

    void Start();

protected:
    virtual void RunL();
    virtual TInt RunError(TInt aError);
    virtual void DoCancel();

private:
    MJesShutdown* iShutdown;
};

/**
 * Java Event Server
 */
class CJesServer : public CServer2
{
public:
    IMPORT_C CJesServer(MJesShutdown* aShutdown);
    ~CJesServer();

    void Shutdown();

private:
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

private:
    CJesShutdown iShutdown;
};

#endif // JESSERVER_H
