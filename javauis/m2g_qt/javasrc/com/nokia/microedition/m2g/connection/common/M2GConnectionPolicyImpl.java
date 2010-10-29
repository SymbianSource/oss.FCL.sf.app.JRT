/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g.connection.common;

import com.nokia.microedition.m2g.connection.M2GConnectionPolicy;

/**
 * Connection policy
 */
public class M2GConnectionPolicyImpl implements M2GConnectionPolicy
{
    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Ctor
     */
    public M2GConnectionPolicyImpl()
    {
    }

    /**
     * Get access rights
     * @see M2GConnectionPolicy#getAccessRight()
     */
    public boolean getAccessRight()
    {
        return true;
    }

}
