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
* Description:  javasizehelpsession definitions
*
*/


#ifndef JAVASIZEHELPSESSION_H
#define JAVASIZEHELPSESSION_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <badesca.h>
#include <s32mem.h>

// FORWARD DECLARATIONS
class CJavaSizeHelpServer;

// CLASS DECLARATION
/**
* This class creates IPC serverprovides for JavaSizeHelper.
* It allows for trusted clients to retrieve size of
* Java install applications.
*
*/
class CJavaSizeHelpSession : public CSession2
{
public:
    // constructor
    /**
    * CJavaSizeHelpSession::CJavaSizeHelpSession constructor
    *
    */
    CJavaSizeHelpSession();

    /**
    * CJavaSizeHelpSession::Create add session
    *
    */
    void Create();

private:
    /**
    * CJavaSizeHelpSession::~CJavaSizeHelpSession destructor
    *
    */
    ~CJavaSizeHelpSession();

    /**
    * CJavaSizeHelpSession::Server
    *
    */
    CJavaSizeHelpServer& Server();

    /**
    * CJavaSizeHelpSession::~ServiceL handle request from client
    *
    */
    void ServiceL(const RMessage2& aMessage);

    /**
    * CJavaSizeHelpSession::GetUserUsedDiskSpaceL method gets size of application
    * The function may leave with one of the system-wide error codes.
    *
    */
    void GetUserUsedDiskSpaceL(const RMessage2& aMessage);
};

#endif // JAVASIZEHELPSESSION_H
