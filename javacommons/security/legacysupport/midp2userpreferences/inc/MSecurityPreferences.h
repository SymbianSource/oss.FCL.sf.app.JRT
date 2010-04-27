/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies).
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
 * @file    MSecurityPreferences.h
 *
 * @internalComponent
 *
 */

#ifndef MSECURITYPREFERENCES_H
#define MSECURITYPREFERENCES_H

#include "j2me/midp2/security/mpermission.h"

namespace MIDP
{
class MFunctionGroupBinding;

/**
 *
 * An abstract operational interface to user security preferences for an
 * individual MIDlet suite.
 *
 */

class MSecurityPreferences
{
public:

    virtual ~MSecurityPreferences() = 0;

    /**
     *
     * Returns the current interaction mode for the given function group binding.
     *
     * @param aBinding the function group binding.
     *
     */

    virtual MPermission::TMode InteractionModeL(const MFunctionGroupBinding& aBinding) = 0;

    /**
     *
     * Sets the current interaction mode for the given function group binding.
     *
     */

    virtual void InteractionModeL(const MFunctionGroupBinding& aBinding, MPermission::TMode aMode) = 0;

    virtual TBool IsFromDefaults(void) const = 0;

    /**
     *
     * Sets the current interaction mode for the given function group binding.Does not
     * update persistent state like InteractionModeL(const MFunctionGroupBinding& aBinding, MPermission::TMode aMode)
     */
    virtual void SetInteractionMode(const MFunctionGroupBinding&, MPermission::TMode) = 0;

};

inline MSecurityPreferences::~MSecurityPreferences()
{
}

} // end of namespace MIDP

#endif // MSECURITYPREFERENCES_H
