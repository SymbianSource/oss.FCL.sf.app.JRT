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

#ifndef PLATFORMREQUESTHANDLER_H
#define PLATFORMREQUESTHANDLER_H

#include "platformrequestinterface.h"

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

class PlatformRequestHandler : public PlatformRequestInterface
{
public:

    PlatformRequestHandler();
    virtual ~PlatformRequestHandler();

    // Methods of PlatformRequestInterface.
    virtual void handleUri(const std::wstring& aUri);

private:

    // Not implemented.
    PlatformRequestHandler(const PlatformRequestHandler &x);
    PlatformRequestHandler &operator=(const PlatformRequestHandler &x);

};

} // end namespace runtime
} // end namespace java

#endif // PLATFORMREQUESTHANDLER_H
