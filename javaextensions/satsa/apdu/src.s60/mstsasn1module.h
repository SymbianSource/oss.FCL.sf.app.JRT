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


#ifndef MSTSASN1MODULE_H
#define MSTSASN1MODULE_H

//  INCLUDES
#include <e32std.h>

namespace java
{
namespace satsa
{

// CLASS DECLARATION

// Base class for all Access Control decoding related classes

NONSHARABLE_CLASS(MSTSASN1Module)
{

public: // New functions

    /**
     * To be implemented in derived classes, used to
     * decrypt raw data to current instance.
     * Will leave with KErrArgument if give data is not valid
     * @param aRawData raw-data to be parsed in this instance
     */
    virtual void DecodeL(const TDesC8& aRawData) = 0;
}
;

} // namespace satsa
} // namespace java
#endif // MSTSASN1MODULE_H
// End of File
