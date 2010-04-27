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
* Description:  This class provides runtime support MIDP runtime.
*
*/


#ifndef MIDPRUNTIMEINTERNALSUPPORT_H
#define MIDPRUNTIMEINTERNALSUPPORT_H

#include "javaosheaders.h"
#include "javauid.h"

namespace java // codescanner::namespace
{

namespace runtime // codescanner::namespace
{

OS_NONSHARABLE_CLASS(MidpStarterInternalSupport)
{

public:
    virtual void closeRuntimeInd() = 0;

    virtual void setUids(const java::util::Uid& midletUid,
                         const java::util::Uid& midletSuiteUid) = 0;

protected:
    // Destructor
    virtual ~MidpStarterInternalSupport() {}


};
} //end namespace runtime
} //end namespace java

#endif // MIDPRUNTIMEINTERNALSUPPORT_H

