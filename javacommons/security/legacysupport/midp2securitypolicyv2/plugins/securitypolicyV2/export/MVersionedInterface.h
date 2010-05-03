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
 * @file    MVersionedInterface.h
 *
 * @internalComponent
 *
 */

#ifndef MVERSIONEDINTERFACE_H
#define MVERSIONEDINTERFACE_H

#include <e32std.h>

namespace MIDP
{

/**
 *
 * A mixin interface used to identify explicitly versioned interfaces.
 *
 */

class MVersionedInterface
{
public:

    virtual TUint32 InterfaceVersion(void) const = 0;
};

}

#endif // MVERSIONEDINTERFACE_H
