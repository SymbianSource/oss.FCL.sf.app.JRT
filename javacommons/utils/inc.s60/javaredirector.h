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
* Description: Redirector redirects standard outs and log messages
*
*/


#ifndef JAVAREDIRECTOR_H
#define JAVAREDIRECTOR_H

#include <e32std.h>
#include "javaosheaders.h"

namespace java
{
namespace util
{

OS_NONSHARABLE_CLASS(Redirector) : public RSessionBase
{
public:
    OS_IMPORT Redirector();
    OS_IMPORT virtual ~Redirector();

    OS_IMPORT TInt connect();
    OS_IMPORT void close();

    OS_IMPORT TInt systemOut(const TDesC8& aData);
    OS_IMPORT TInt systemErr(const TDesC8& aData);
    OS_IMPORT static TInt log(const TDesC8& aData);

private:
    TInt send(TInt aFunction, const TDesC8& aData);

    TInt mStatus;
};


} // end namespace util
} // end namespace java

#endif // JAVAREDIRECTOR_H
