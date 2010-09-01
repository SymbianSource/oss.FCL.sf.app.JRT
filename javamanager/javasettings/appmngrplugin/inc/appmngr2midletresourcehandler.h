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
* Description:  MidletAppInfo represents installed Java applications
*
*/

#ifndef APPMNGR2MIDLETRESOURCEHANDLER_H
#define APPMNGR2MIDLETRESOURCEHANDLER_H

#include <e32base.h>
#include <eikenv.h>                     // CEikonEnv

/**
 * Resource handler
 *
 *
 * @lib appmngr2midletplugin.dll
 * @since S60 v9.2
 */
class CAppMngr2MidletResourceHandler : CBase
{
public:

    /**
    * Two-phased constructor.
    */
    static CAppMngr2MidletResourceHandler* NewL();

    /**
     * Destructor.
     */
    virtual ~CAppMngr2MidletResourceHandler();

private:

    CAppMngr2MidletResourceHandler();

    /**
     * Second phase constructor
     *
     * @since S60 9.2
     */
    void ConstructL();

public:

    /**
     */
    TInt AddResourceFileL();

    /**
     */
    void DeleteResourceFile(TInt);

private:

    CEikonEnv* iEnv;
    TFileName iResourceFileName;
};

#endif // APPMNGR2MIDLETRESOURCEHANDLER_H

