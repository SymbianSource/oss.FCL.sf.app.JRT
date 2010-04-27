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
 * @file MContentAccessPolicy.h
 *
 * @internalComponent
 *
 */

#ifndef MCONTENTACCESSPOLICY_H
#define MCONTENTACCESSPOLICY_H

#include <e32base.h>
#include "j2me/midp2/core/MVersionedInterface.h"

namespace MIDP
{

/**
 *
 * An abstract operational interface to a Content access policy.
 *
 */

class MContentAccessPolicy: public MVersionedInterface
{
};

} // end of namespace MIDP

#endif // MCONTENTACCESSPOLICY_H
