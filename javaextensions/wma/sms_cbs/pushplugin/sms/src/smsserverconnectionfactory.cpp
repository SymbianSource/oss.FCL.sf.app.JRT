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
#include "serverconnectionbase.h"
#include "smsserverconnectionfactory.h"

using namespace java::util;
using namespace java::wma;

#if defined(__SYMBIAN32__) && defined(__WINSCW__)
#include <pls.h>
#else
static SmsServerConnectionFactory* sSmsSrvConnFac = 0;
#endif

#ifdef __SYMBIAN32__
ServerConnectionFactory& getServerConnectionFactory()
{
#else
extern "C" ServerConnectionFactory& getServerConnectionFactory()
{
#endif
    JELOG2(EWMA);

    //CONTEXT_REMOVAL
#if defined(__SYMBIAN32__) && defined(__WINSCW__)
    TUid uid = TUid::Uid(0xE0000129);
    SmsServerConnectionFactory* retObj = Pls<SmsServerConnectionFactory>(uid);
    return *retObj;
#else
    if (sSmsSrvConnFac == 0)
    {
        sSmsSrvConnFac = new SmsServerConnectionFactory();
    }
    return *sSmsSrvConnFac;
#endif

    /*
     Context& context = Context::getContext();
     void* ptr = context.getService("smspushplugin::ServerConnectionFactory");
     if(NULL == ptr)
     {
     SmsServerConnectionFactory* newObj = new SmsServerConnectionFactory();
     context.addService("smspushplugin::ServerConnectionFactory",newObj);
     return *newObj;
     }
     ServerConnectionFactory* retObj = reinterpret_cast
     <ServerConnectionFactory*>(ptr);
     return *retObj;
     */
}

OS_EXPORT SmsServerConnectionFactory& SmsServerConnectionFactory::getFactory()
{
    JELOG2(EWMA);
    ServerConnectionFactory& scf = ::getServerConnectionFactory();
    return reinterpret_cast<SmsServerConnectionFactory&>(scf);
}

/**
 *
 */
OS_EXPORT SmsServerConnectionFactory::SmsServerConnectionFactory()
{
    JELOG2(EWMA);
}

/**
 *
 */
OS_EXPORT SmsServerConnectionFactory::~SmsServerConnectionFactory()
{
    JELOG2(EWMA);
}


/**
 *
 */
OS_EXPORT ServerConnection* SmsServerConnectionFactory::createSrvConnObj(
    const std::wstring& aUri,const std::wstring& aFilter)
{
    JELOG2(EWMA);
    return ServerConnectionBase::getServerConnection(aUri,aFilter);
}

OS_EXPORT bool SmsServerConnectionFactory::isMultipleSrvConnAllowed()
{
    return false;
}

