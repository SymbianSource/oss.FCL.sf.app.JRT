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
 * Class for holding attribute information.
 * Instances of this class are immutable.
 */
public class Attribute
{
    private String iName = null;
    private String iValue = null;
    private boolean iTrusted = false;

    /**
     * Private default constructor, not to be used by clients.
     */
    private Attribute()
    {
    }

    /**
     * Constructor for untrusted attribute.
     * @param aName attribute name
     * @param aValue attribute value
     */
    public Attribute(String aName, String aValue)
    {
        this(aName, aValue, false);
    }

    /**
     * Constructor for attribute.
     * @param aName attribute name
     * @param aValue attribute value
     * @param aTrusted tells if attribute is trusted
     */
    public Attribute(String aName, String aValue, boolean aTrusted)
    {
        if (aName == null)
        {
            throw new NullPointerException
            ("Attribute name is null, value is " + aValue);
        }
        if (aValue == null)
        {
            throw new NullPointerException
            ("Attribute value is null, name is " + aName);
        }
        iName = aName;
        iValue = aValue;
        iTrusted = aTrusted;
    }

    /**
     * Get attribute name.
     * @return attribute name
     */
    public String getName()
    {
        return iName;
    }

    /**
     * Get attribute value.
     * @return attribute value
     */
    public String getValue()
    {
        return iValue;
    }

    /**
     * Get int type attribute value.
     * @return attribute value
     * @throws NumberFormatException if value is not a valid integer
     */
    public int getIntValue()
    {
        return Integer.parseInt(iValue);
    }

    /**
     * Is attribute trusted. Attribute is trusted if it is defined
     * in Manifest of trusted application, otherwise it is untrusted.
     * @return true if attribute is trusted, false otherwise
     */
    public boolean isTrusted()
    {
        return iTrusted;
    }

    /**
     * Returns a hash code value for this object.
     */
    public int hashCode()
    {
        return iName.hashCode() + iValue.hashCode();
    }

    /**
     * Indicates whether given object is equal to this one.
     */
    public boolean equals(Object aObj)
    {
        if (!(aObj instanceof Attribute))
        {
            return false;
        }
        Attribute attr = (Attribute)aObj;
        if (this.getName().equals(attr.getName()) &&
                this.getValue().equals(attr.getValue()) &&
                this.isTrusted() == attr.isTrusted())
        {
            return true;
        }
        return false;
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append(getName()).append(": ").append(getValue());
        if (isTrusted())
        {
            buf.append(" (trusted)");
        }
        return buf.toString();
    }
}
