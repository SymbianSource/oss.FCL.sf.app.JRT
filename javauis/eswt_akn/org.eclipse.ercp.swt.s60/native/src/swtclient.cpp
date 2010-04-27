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


#include "swtclient.h"
#include "swtserver.h"
#include "coreuiavkoneswt.h"


// ======== MEMBER FUNCTIONS ========


TInt RSwtClient::Connect()
{
    RServer2 server =
        java::ui::CoreUiAvkonEswt::getInstance().getEswtSupport().getServerHandle();
    TInt error = CreateSession(server, CSwtServer::Version());
    if (error != KErrNone)
    {
        return error;
    }
    return ShareAuto();
}


/**
 * Executes a functor in the UI thread
 *
 * @param aFunctor The functor to execute
 *
 * @return The functor's leave code.
 */
TInt RSwtClient::Execute(const MSwtFunctor& aFunctor) const
{
    const TIpcArgs params(&aFunctor);
    return SendReceive(CSwtServer::EExecute, params);
}
