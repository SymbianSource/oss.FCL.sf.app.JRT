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
* Description:  javasizehelpserver definitions
*
*/


#ifndef JAVASIZEHELPSERVER_H
#define JAVASIZEHELPSERVER_H

// INCLUDES
#include <e32base.h>    // CTrapCleanup

// CLASS DECLARATION
/**
* This class creates IPC serverprovides for JavaSizeHelper.
* It allows for trusted clients to retrieve size of
* Java install applications.
*
*/
class CJavaSizeHelpServer : public CServer2
{
public:

    /**
    * Creates new java size helper server object.
    *
    */
    static CJavaSizeHelpServer* NewL();

    /**
    * Creates new java size helper server object.
    *
    */
    static CJavaSizeHelpServer* NewLC();

    /**
     * CJavaSizeHelpServer::~CJavaSizeHelpServer() destructor
     *
     */
    virtual ~CJavaSizeHelpServer();


    // general methods
    /**
     * CJavaSizeHelpServer::AddSession add session
     *
     */
    void AddSession();

    /**
     * CJavaSizeHelpServer::RemoveSession deletes session
     *
     */
    void RemoveSession();

private:
    // constructor
    /**
     * CJavaSizeHelpServer::CJavaSizeHelpServer constructor
     *
     * @since S60 vMCL
     */
    CJavaSizeHelpServer();

    /**
    * CJavaSizeHelpServer::Construct
    *
    */
    void Construct();

    /**
    * CJavaSizeHelpServer::NewSessionL create new session
    * The function may leave with one of the system-wide error codes.
    *
    */
    CSession2* NewSessionL(const TVersion& aVersion, const RMessage2& aMessage) const;

    TInt RunError(TInt aError);

private:
    //data
    /**
    * CJavaSizeHelpServer::iSessionCount member, the client class
    * to the javasizehelp server.
    */
    TInt iSessionCount;

};

#endif // JAVASIZEHELPSERVER_H
