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


#include "cswtclient.h"
#include "swtserver.h"
#include "swtdisplay.h"
#include "swtfunctor.h"
#include "swtcliententry.h"


namespace SWT
{
EXPORT_C MSwtClient* CreateClientL()
{
    return CSwtClient::NewL();
}
}


// ======== MEMBER FUNCTIONS ========


CSwtClient* CSwtClient::NewL()
{
    CSwtDisplay* display = CSwtDisplay::CurrentOrNull();

    CSwtClient* self = NULL;
    // return NULL if called outside java ui thread or display is not created
    if (display)
    {
        self = new(ELeave)CSwtClient(display);
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(self);
    }
    return self;
}

CSwtClient::~CSwtClient()
{
    iClient.Close();
}

CSwtClient::CSwtClient(MSwtDisplay* aDisplay)
{
    iDisplay = aDisplay;
}

void CSwtClient::ConstructL()
{
    // connect to display's server
    User::LeaveIfError(iClient.Connect());
    // allow clients from other threads
    User::LeaveIfError(iClient.ShareAuto());
}

TInt CSwtClient::Execute(const MSwtFunctor& aFunctor) const
{
    return iClient.Execute(aFunctor);
}

MSwtDisplay* CSwtClient::Display() const
{
    return iDisplay;
}
