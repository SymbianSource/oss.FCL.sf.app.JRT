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
* Description: DebugApiImpl provides implementation for Degug API methods
*
*/


#ifndef DEBUGAPIIMPL_H
#define DEBUGAPIIMPL_H

#include <string>
#include <list>
#include <unistd.h>
#include "javaosheaders.h"

namespace java
{
// forward declarations
namespace util
{
class Uid;
}

namespace debug
{
OS_NONSHARABLE_CLASS(DebugApiImpl)
{
public:
    DebugApiImpl();
    virtual ~DebugApiImpl();

    int installApp(const std::wstring& aFilename,
                   java::util::Uid& aSuiteUid,
                   std::list<java::util::Uid>& aAppUid);
    int uninstallApp(const java::util::Uid& aSuiteUid);

    int startApp(const java::util::Uid& aAppUid, const std::wstring& aUeiParameters, pid_t& aPid);
    int stopApp(const java::util::Uid& aAppUid);
};

} // end namespace debug
} // end namespace java


#endif // DEBUGAPIIMPL_H
