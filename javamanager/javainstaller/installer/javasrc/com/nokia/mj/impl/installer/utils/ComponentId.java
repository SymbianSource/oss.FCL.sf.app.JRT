/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.utils;

/**
 * Contains a component id.
 */
public final class ComponentId
{
    private int iId = 0;

    /**
     * Construct new ComponentId object.
     */
    public ComponentId()
    {
    }

    /**
     * Construct new ComponentId object.
     *
     * @param aId component id
     */
    public ComponentId(int aId)
    {
        iId = aId;
    }

    /**
     * Get component id.
     */
    public int getId()
    {
        return iId;
    }

    /**
     * Set component id.
     */
    public void setId(int aId)
    {
        iId = aId;
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer("ComponentId: ");
        buf.append(getId());
        return buf.toString();
    }
}
