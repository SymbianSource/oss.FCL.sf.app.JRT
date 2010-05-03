/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "platformrequesthandler.h"
#include "exceptionbase.h"

using namespace java::runtime;
using namespace java::util;

PlatformRequestInterface* getPlatformRequestHandlerObj()
{
    JELOG2(EJavaRuntime);
    return new PlatformRequestHandler();
}

/**
 *
 */
PlatformRequestHandler::PlatformRequestHandler()
{
}

/**
 *
 */
PlatformRequestHandler::~PlatformRequestHandler()
{
}

/**
 * Missing implementation.
 */
void PlatformRequestHandler::handleUri(const std::wstring& aUri)
{
    throw ExceptionBase(PlatformRequestInterface::CONNECTION_NOT_SUPPORTED,
                        "Not implemented",__FILE__,__FUNCTION__,__LINE__);
}

