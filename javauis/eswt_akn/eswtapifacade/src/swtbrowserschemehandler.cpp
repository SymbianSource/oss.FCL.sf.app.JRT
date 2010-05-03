/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/



#include <schemehandler.h>
#include "swtbrowserschemehandler.h"



// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CSwtBrowserSchemeHandler::CSwtBrowserSchemeHandler()
{
}


void CSwtBrowserSchemeHandler::ConstructL(const TDesC& aUrl)
{
    iSchemeHandler = CSchemeHandler::NewL(aUrl);
}


EXPORT_C CSwtBrowserSchemeHandler* CSwtBrowserSchemeHandler::NewL(const TDesC& aUrl)
{
    CSwtBrowserSchemeHandler* self
    = new(ELeave) CSwtBrowserSchemeHandler();
    CleanupStack::PushL(self);
    self->ConstructL(aUrl);
    CleanupStack::Pop(self);
    return self;
}


CSwtBrowserSchemeHandler::~CSwtBrowserSchemeHandler()
{
    delete iSchemeHandler;
    iSchemeHandler = NULL;
}


EXPORT_C void CSwtBrowserSchemeHandler::SetParameterList(CAiwGenericParamList* aParamList)
{
    ASSERT(iSchemeHandler);
    iSchemeHandler->SetParameterList(aParamList);
}


EXPORT_C void CSwtBrowserSchemeHandler::Observer(MAknServerAppExitObserver* aSchemeDoc)
{
    ASSERT(iSchemeHandler);
    iSchemeHandler->Observer(aSchemeDoc);
}


EXPORT_C void CSwtBrowserSchemeHandler::HandleUrlEmbeddedL()
{
    ASSERT(iSchemeHandler);
    iSchemeHandler->HandleUrlEmbeddedL();
}


EXPORT_C void CSwtBrowserSchemeHandler::HandleUrlStandaloneL()
{
    ASSERT(iSchemeHandler);
    iSchemeHandler->HandleUrlStandaloneL();
}



