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
* Description:  javasizehelpclient definitions
*
*/


#ifndef JAVASIZEHELPCLIENT_H
#define JAVASIZEHELPCLIENT_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>


// CLASS DECLARATION
/**
* This class provides the Internal API for JavaSizeHelper.
* It allows for trusted clients to retrieve size of
* Java install applications.
*
*/
class RJavaSizeHelpClient : public RSessionBase
{
public:

    // constructor
    /**
    * RJavaSizeHelpClient::RJavaSizeHelpClient constructor
    *
    */
    IMPORT_C RJavaSizeHelpClient();

    // destructor
    /**
    * RJavaSizeHelpClient::~RJavaSizeHelpClient destructor
    *
    */
    IMPORT_C ~RJavaSizeHelpClient();

    // general methods
    /**
    * RJavaSizeHelpClient::Connect method connet client with server
    *
    */
    IMPORT_C TInt Connect();
    /**
    * RJavaSizeHelpClient::GetUserUsedDiskSpace method gets size of application
    *
    * @param aDirPath The path of installed application directory.
    */
    IMPORT_C TInt GetUserUsedDiskSpace(const TDesC16& aDirPath) const;

private:
    /**
    * RJavaSizeHelpClient::StartServer method starts server
    *
    */
    TInt StartServer();
};

#endif // JAVASIZEHELPCLIENT_H

