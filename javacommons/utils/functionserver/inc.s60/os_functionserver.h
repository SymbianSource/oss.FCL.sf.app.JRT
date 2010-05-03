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
* Description:  ?Description
*
*/


#ifndef OS_FUNCTIONSERVER_H
#define OS_FUNCTIONSERVER_H

#include <e32base.h>
#include <string>

#include "javaosheaders.h"

namespace java
{
namespace util
{

class FunctionServer;

NONSHARABLE_CLASS(ServerShutdown) : public CActive
{
public:
    ServerShutdown();
    void startShutDown();

protected:
    virtual void RunL();
    virtual TInt RunError(TInt error);
    virtual void DoCancel();

};


/**
 *
 */
NONSHARABLE_CLASS(OsFunctionServer) : public CServer2
{
// Data types
public:
    enum TAction
    {
        EExecute = 0,
        EStop
    };

private:
    class CFunctionSession;
// Methods
public:
    ~OsFunctionServer();

    OsFunctionServer();
    inline void osThreadInit();
    inline void osThreadClean();
    void startOsServer();

    void startShutDown();
    void stopServerInsideServerThread();

// From CServer2
protected:
    TInt RunError(TInt aError);

private:
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    CTrapCleanup* mCleanUp;

    ServerShutdown mShutdown;
};


NONSHARABLE_CLASS(OsFunctionServer::CFunctionSession)
        : public CSession2
{
public:
    inline CFunctionSession();

private:
    TInt doServiceL(TAction aAction, const TAny* aParam);

// From CSession2
public:
    void ServiceL(const RMessage2& aMessage);

};


/**
 *
 */
inline void OsFunctionServer::osThreadInit()
{
    mCleanUp = CTrapCleanup::New();
}

/**
 *
 */
inline void OsFunctionServer::osThreadClean()
{
    delete mCleanUp;
    mCleanUp = 0;
}

} //end namespace util
} //end namespace java
#endif // OS_FUNCTIONSERVER_H

