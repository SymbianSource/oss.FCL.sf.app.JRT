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
* Description:  Platform request interface.
 *
*/


#ifndef PLATFORMREQUESTINTERFACE_H
#define PLATFORMREQUESTINTERFACE_H

#include <string>

#include "javaosheaders.h"

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

/**
 * Interface for platform specific implementation of MIDlet.platformRequest()
 * method.
 */
OS_NONSHARABLE_CLASS(PlatformRequestInterface)
{
public:

    static const int CONNECTION_NOT_SUPPORTED = -11100;
    static const int UNEXPECTED_ERROR = -11101;

    virtual ~PlatformRequestInterface() {}

    /**
    * This operation launches application which can manage URI.
    * @param aUri wanted to be executed.
    * @throws ExceptionBase with following error codes:
    *         CONNECTION_NOT_SUPPORTED:
    *         - scheme of the URI is not supported in the device.
    *         INTERNAL_ERROR:
    *         - Launching an application ffailed to internal error.
    */
    virtual void handleUri(const std::wstring& aUri) = 0;

};

} // end namespace runtime
} // end namespace java

java::runtime::PlatformRequestInterface* getPlatformRequestHandlerObj();

#endif // PLATFORMREQUESTINTERFACE_H
