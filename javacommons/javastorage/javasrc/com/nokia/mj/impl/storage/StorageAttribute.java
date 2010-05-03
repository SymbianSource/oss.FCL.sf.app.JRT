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


package com.nokia.mj.impl.storage;

/**
 * StorageAttribute represents one attribute in JavaStorageEntry. It is
 * basically a key value pair and it holds one storage column information.
 */
public final class StorageAttribute implements StorageNames
{
    /**
     * Escape delimiter is used to escape such characters that cannot exists
     * in SQL statement column value.
     */
    private static final char ESCAPE_CHAR_SQUOTE = '\'';

    private String iName;
    private String iValue;
    private int iType;

    /**
     * StorageAttribute value type Int.
     */
    public static final int INT_TYPE = 1;

    /**
     * StorageAttribute value type String. Default type.
     */
    public static final int STRING_TYPE = 2;

    /**
     * StorageAttribute value type NULL.
     */
    public static final int NULL_TYPE = 3;

    private StorageAttribute() {}

    /**
     * Constructor. Create attribute based on name and value. Type is set to
     * default i.e. STRING_TYPE.
     *
     * @param aName attribute name.
     * @param aValue attribute value. Null value is converted to empty String.
     * @throws StorageException if name is null or empty and if value type
     *         is unsupported.
     */
    public StorageAttribute(String aName, String aValue)
    throws StorageException
    {
        setAttribute(aName, aValue, STRING_TYPE);
    }

    /**
     * Constructor. Create attribute based on name, value and type.
     *
     * @param aName attribute name.
     * @param aValue attribute value. Null value is converted to empty String.
     * @param aAttributeType attribute type.
     * @throws StorageException if name is null or empty and if value type
     *         is unsupported.
     */
    public StorageAttribute(String aName, String aValue, int aAttributeType)
    throws StorageException
    {
        setAttribute(aName, aValue, aAttributeType);
    }

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Set attribute. Overrides existing value.
     *
     * @param aName attribute name.
     * @param aValue attribute value. If value is null type is set
     *               to NULL_TYPE.
     * @param aAttributeType type of attribute.
     * @throws StorageException if name is null or empty of value type
     *         is unsupported.
     */
    public void setAttribute(String aName, String aValue, int aAttributeType)
    throws StorageException
    {
        if (aName == null || aName.equals(""))
        {
            throw new StorageException("Invalid name argument");
        }

        if (aAttributeType < INT_TYPE || aAttributeType > NULL_TYPE)
        {
            throw new StorageException("Invalid type argument");
        }

        iName = aName;

        if (aValue == null || aAttributeType == NULL_TYPE)
        {
            iValue = "NULL";
            iType = NULL_TYPE;
        }
        else
        {
            iValue = aValue;
            iType = aAttributeType;
        }
    }

    /**
     * Get attribute name.
     *
     * @return name.
     */
    public String getName()
    {
        return iName;
    }

    /**
     * Get attribute value.
     *
     * @return value. If value is not set null is returned.
     */
    public String getValue()
    {
        if (iType != NULL_TYPE)
            return iValue;
        else
            return null;
    }

    /**
     * Get attribute type.<p>
     * NOTE: Attribute type is not stored to database. This is used
     * by the storage clients to inform StorageSession of INT value.
     * When reading data from storage all attributes have default type value
     * that is StorageSession.STRING_TYPE.
     *
     * @return type.
     */
    public int getType()
    {
        return iType;
    }

    /**
     * Print StorageAttribute as String.
     *
     * @return attribute in String format. NAME='VALUE' Type: TYPE'
     */
    public String toString()
    {
        return iName + "='" + iValue + "' Type: " + Integer.toString(iType);
    }

    /**
     * Returns a hash code value for this object.
     */
    public int hashCode()
    {
        return iName.hashCode() + iValue.hashCode() + iType;
    }

    /**
     * Indicates whether given object is equal to this one.
     */
    public boolean equals(Object aObj)
    {

        if (!(aObj instanceof StorageAttribute))
        {
            return false;
        }

        StorageAttribute attr = (StorageAttribute) aObj;

        if (this.getName().equals(attr.getName())
                && this.getValue().equals(attr.getValue())
                && this.getType() == attr.getType())
        {
            return true;
        }

        return false;
    }

    /*** ------------------------ PACKAGE PROTECTED ------------------------ */

    /**
     * Escape value. This is used internally when storage query is constructed.
     *
     * @return escaped StorageAttribute value.
     */
    String getEscapedValue()
    {
        if (iValue.indexOf(ESCAPE_CHAR_SQUOTE) != -1)
        {
            return escape();
        }
        else
        {
            return iValue;
        }
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Escape attribute value.
     *
     * @param  aValue to be escaped.
     * @return escaped string.
     */
    private String escape()
    {
        StringBuffer escapedVal = new StringBuffer(iValue);
        for (int i = 0; i < escapedVal.length(); i++)
        {
            if (escapedVal.charAt(i) == ESCAPE_CHAR_SQUOTE)
            {
                escapedVal.insert(i, ESCAPE_CHAR_SQUOTE);
                i++;
            }
        }
        return escapedVal.toString();
    }

    /*** ----------------------------- NATIVE ----------------------------- */
}
