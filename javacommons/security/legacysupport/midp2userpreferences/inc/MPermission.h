/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
@file
@publishedPartner
@released
*/

#ifndef MPERMISSION_H
#define MPERMISSION_H

#include <e32base.h>

namespace MIDP
{
/**
 * Abstraction of an individual permission.
 */
class MPermission
{
public:

    /**
     * Enumeration indicating whether the permission requires user interaction.
     */
    enum TType
    {
        EAllowed = 1,
        EUser    = 2
    };


    /**
     * Enumeration defining the actual permission level.
     */
    enum TMode
    {
        EInvalid = 0,
        EDenied  = EInvalid,
        EAllow   = 1,
        EBlanket = 2,
        EOneshot = 3,
        ESession = 4
    };

public:

    /** Destructor */
    virtual ~MPermission() = 0;

    /**
     * Return the name of this permission.
     * @return  Descriptor reference containing the name.
     */
    virtual const TDesC& Name(void) const   = 0;

    /**
     * Return the type of this permission.
     * @return  The permission type.
     */
    virtual TType Type(void) const          = 0;

    /**
     * Return the mode of this permission.
     * @return  The permission mode.
     */
    virtual TMode Mode(void) const          = 0;
};

inline MPermission::~MPermission()
{
}

}

#endif // MPERMISSION_H
