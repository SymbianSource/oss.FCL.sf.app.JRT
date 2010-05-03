/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  javaregserverpolicycheck implementation
*
*/


#include "javaregserver.h"
#include "javaregserversession.h"
#include "javaregserverallowedid.h"
#include "logger.h"

// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CJavaRegServer::CustomSecurityCheckL
// ---------------------------------------------------------------------------
//
// using namespace Java::Utilities;
using namespace Java::Manager::Registry;

CPolicyServer::TCustomResult CJavaRegServer::CustomSecurityCheckL(
    const RMessage2&  aMsg,
    TInt&  aAction,
    TSecurityInfo&  /*aMissing*/)
{
    JELOG(EJavaStorage, "CJavaRegServer::CustomSecurityCheckL()");
    switch (aMsg.Function())
    {
    case EInitializeServerTransId:
    case ESetEntry:
    case ERemoveEntry:
    case ERemoveEntryFromAll:
    case EAllocateUids:
        // Pass if the client is Java Installer OR OSGi VM OR SDK debug agent
        if (aMsg.SecureId() == AllowedClientSecureId_Installer ||
                aMsg.SecureId() == AllowedClientSecureId_OsgiVm ||
                aMsg.SecureId() == AllowedClientSecureId_SDKDebugAgent)
        {
            return CPolicyServer::EPass;
        }
        // Pass if the client is using the legacy API AND
        // it's either JavaHelperServer OR AppInstUi.
        // Written as an 'elseif' only for better readability's sake.
        else if (((CJavaRegServerSession*)(aMsg.Session()))->IsLegacy() &&
                 (aMsg.SecureId() == AllowedClientSecureId_JavaHelperServer ||
                  aMsg.SecureId() == AllowedClientSecureId_AppInstUi))
        {
            return CPolicyServer::EPass;
        }
        else
        {
            aAction = CPolicyServer::EFailClient;
            return CPolicyServer::EFail;
        }
        // falltrough not possible here
    case ESetBackupState:
        if (aMsg.SecureId() == AllowedClientSecureId_Backup)
        {
            return CPolicyServer::EPass;
        }
        else
        {
            aAction = CPolicyServer::EFailClient;
            return CPolicyServer::EFail;
        }
        // falltrough not possible here
    default:
        aAction = CPolicyServer::EFailClient;
        return CPolicyServer::EFail;
        // falltrough not possible here
    }
}
