/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 *
 * @file    MConnectionFilter.h
 *
 * @internalComponent
 *
 */

#ifndef MCONNECTIONFILTER_H
#define MCONNECTIONFILTER_H

#include <e32base.h>

namespace MIDP
{

/**
 *
 * An abstract operational interface to a filter object
 * used to control access to 'connections'
 *
 */

class MConnectionFilter
{
public:

    /**
     *
     * Returns the type of the connection filter.
     *
     */

    virtual TInt Type(void) const = 0;

    /**
     *
     * Returns the symbolic type name of the connection filter.
     *
     */

    virtual const TDesC& TypeName(void) const = 0;

    /**
     *
     * Returns the scheme to which the connection filter applies.
     *
     */

    virtual const TDesC& Scheme(void) const = 0;

    /**
     *
     * Returns the connection filter parameters.
     *
     */

    virtual const RPointerArray<HBufC>& Params(void) const = 0;
};

} // end of namespace MIDP

#endif // MCONNECTIONFILTER_H
