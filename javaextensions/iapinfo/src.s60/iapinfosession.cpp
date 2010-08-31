/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  JavaCommDB class implementation
 *
*/
#include "iapinfosession.h"
#include "fs_methodcall.h"

#include "logger.h"

using namespace CommsDat;

IapInfoSession::IapInfoSession(): java::util::FunctionServer("MyIapServer")
{
    createServerToNewThread();

}

IapInfoSession* IapInfoSession::NewL()
{
    LOG(EJavaIapInfo, EInfo, "IapInfoSession + NewL()");
    IapInfoSession* self =   new(ELeave) IapInfoSession();
    CallMethodL(self, &IapInfoSession::ConstructL,self);
    return self;

}

void IapInfoSession::ConstructL()
{
    iDb = CMDBSession::NewL(KCDVersion1_1);

}

