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
* Description:
 *
*/


#include "logger.h"
#include "pushexception.h"
#include "pusherrorcodes.h"
#include "javacommonutils.h"
#include "serverconnectionbase.h"
#include "cbsserverconnectionfactory.h"

using namespace java::util;

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static CbsServerConnectionFactory* sCbsConnFac = 0;
#endif

#ifdef __SYMBIAN32__
ServerConnectionFactory& getServerConnectionFactory()
{
#else
extern "C"
ServerConnectionFactory& getServerConnectionFactory()
{
#endif
    JELOG2(EWMA);

    //CONTEXT_REMOVAL
#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000130);
    CbsServerConnectionFactory* retObj = Pls<CbsServerConnectionFactory>(uid);
    return *retObj;
#else
    if (sCbsConnFac == 0)
    {
        sCbsConnFac = new CbsServerConnectionFactory();
    }
    return *sCbsConnFac;
#endif

    /*
     Context& context = Context::getContext();
     void* ptr = context.getService("smspushplugin::ServerConnectionFactory");
     if(NULL == ptr)
     {
     CbsServerConnectionFactory* newObj = new CbsServerConnectionFactory();
     context.addService("smspushplugin::ServerConnectionFactory",newObj);
     return *newObj;
     }
     ServerConnectionFactory* retObj = reinterpret_cast
     <ServerConnectionFactory*>(ptr);
     return *retObj;
     */
}

OS_EXPORT CbsServerConnectionFactory& CbsServerConnectionFactory::getFactory()
{
    JELOG2(EWMA);
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<CbsServerConnectionFactory&>(scf);
}

/**
 *
 */
OS_EXPORT CbsServerConnectionFactory::CbsServerConnectionFactory()
{
    JELOG2(EWMA);
}

/**
 *
 */
OS_EXPORT CbsServerConnectionFactory::~CbsServerConnectionFactory()
{
    JELOG2(EWMA);
}

/**
 *
 */
OS_EXPORT ServerConnection* CbsServerConnectionFactory::createSrvConnObj(
    const std::wstring& aUri,const std::wstring& aFilter)
{
    JELOG2(EWMA);
    return ServerConnectionBase::getServerConnection(aUri,aFilter);
}

OS_EXPORT bool CbsServerConnectionFactory::isMultipleSrvConnAllowed()
{
    JELOG2(EWMA);
    return false;
}

