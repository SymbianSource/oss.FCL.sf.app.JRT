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
* Description:  MidletAppInfo implementation
*
*/

#include "appmngr2midletresourcehandler.h"
#include "s60commonutils.h"

// MIDlet Runtime plugin resource
_LIT(KFileAppMngr2MidletResource, "javaapplicationsettings.rsc");

CAppMngr2MidletResourceHandler* CAppMngr2MidletResourceHandler::NewL()
{
    CAppMngr2MidletResourceHandler* self = new(ELeave) CAppMngr2MidletResourceHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CAppMngr2MidletResourceHandler::CAppMngr2MidletResourceHandler()
{
}

void CAppMngr2MidletResourceHandler::ConstructL()
{
    iResourceFileName = java::util::S60CommonUtils::ResourceLanguageFileNameL(
                            KFileAppMngr2MidletResource);
    iEnv = CEikonEnv::Static();
}

CAppMngr2MidletResourceHandler::~CAppMngr2MidletResourceHandler()
{
}

TInt CAppMngr2MidletResourceHandler::AddResourceFileL()
{
    return iEnv->AddResourceFileL(iResourceFileName);
}

void CAppMngr2MidletResourceHandler::DeleteResourceFile(TInt aResourceFileOffset)
{
    if (aResourceFileOffset > 0)
    {
        iEnv->DeleteResourceFile(aResourceFileOffset);
    }
}
