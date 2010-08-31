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


package com.nokia.mj.impl.utils;

/**
 * JAVADOCS ARE UNDER CONSTRUCTION!!
 * <p>
 * Unique identifier.
 */
public class Uid
{
    private String iValue = null;

    /**
     * Default constructor.
     */
    protected Uid()
    {
    }

    /**
     * Constructor.
     */
    protected Uid(String aValue)
    {
        iValue = aValue;
    }

    /**
     * Creates a new Uid object from given string.
     * If given string is not a valid Uid, returns null.
     */
    public static Uid createUid(String aValue)
    {
        if(aValue == null || aValue.length() == 0)
        {
            return null;
        }
        return new Uid(aValue);
    }

    /**
     * Returns string value for this Uid.
     */
    public String getStringValue()
    {
        return iValue;
    }

    /**
     * Returns a hash code value for the object.
     */
    public int hashCode()
    {
        return iValue.hashCode();
    }

    /**
     * Indicates whether given object is equal to this one.
     */
    public boolean equals(Object aObj)
    {
        if(!(aObj instanceof Uid))
        {
            return false;
        }
        Uid uid = (Uid)aObj;
        if(this.getStringValue().equals(uid.getStringValue()))
        {
            return true;
        }
        return false;
    }

    /**
     * Compare this Uid to given one.
     * @return a negative integer, zero, or a positive integer
     * if this instance is less than, equal to, or greater than
     * the given Uid.
     */
    public int compareTo(Uid aUid)
    {
        return toString().compareTo(aUid.toString());
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        return getStringValue();
    }
}
