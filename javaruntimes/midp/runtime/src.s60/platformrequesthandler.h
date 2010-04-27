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
* Description:  This class provides container for message.
*
*/


#ifndef PLATFORMREQUESTHANDLER_H
#define PLATFORMREQUESTHANDLER_H

#include "platformrequestinterface.h"

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

OS_NONSHARABLE_CLASS(PlatformRequestHandler) : public PlatformRequestInterface
{
public:

    PlatformRequestHandler();
    virtual ~PlatformRequestHandler();

    // Methods of PlatformRequestInterface.
    virtual void handleUri(const std::wstring& aUri);

private:

    // Internal operations.
    void launchAppL(const std::wstring& aUri);

    bool startsArbitraryNativeApp(const std::wstring& aUri);
    void launchNativeAppL(const std::wstring& aUri);

    // Not implemented.
    PlatformRequestHandler(const PlatformRequestHandler &x);
    PlatformRequestHandler &operator=(const PlatformRequestHandler &x);
};

} // end namespace runtime
} // end namespace java

#endif // PLATFORMREQUESTHANDLER_H
