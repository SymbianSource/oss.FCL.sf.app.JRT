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
 * @file    ConnectionFilterType.h
 *
 * @internalComponent
 *
 */

#ifndef CONNECTIONFILTERTYPE_H
#define CONNECTIONFILTERTYPE_H

namespace MIDP
{

/**
 *
 * An enumeration representing possible connection filter types.
 *
 */

enum TConnectionFilterType
{
    EInvalidFilterType,
    ENegativeAddressFilter,
    EPositiveAddressFilter,
    ENegativePortFilter,
    EPositivePortFilter,
    EDynamicFilter
};

} // end of namespace MIDP

#endif // CONNECTIONFILTERTYPE_H
