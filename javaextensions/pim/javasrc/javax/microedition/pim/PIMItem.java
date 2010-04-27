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
* Description:  PIM item interface base.
 *
*/


// PACKAGE
package javax.microedition.pim;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant PIMItem interface.
 */
public interface PIMItem
{
    public static final int BINARY = 0;

    public static final int BOOLEAN = 1;

    public static final int DATE = 2;

    public static final int INT = 3;

    public static final int STRING = 4;

    public static final int STRING_ARRAY = 5;

    public static final int ATTR_NONE = 0;

    public static final int EXTENDED_FIELD_MIN_VALUE = 16777216;

    public static final int EXTENDED_ATTRIBUTE_MIN_VALUE = 16777216;

    public PIMList getPIMList();

    public void commit() throws PIMException;

    public boolean isModified();

    public int[] getFields();

    public byte[] getBinary(int aField, int aIndex);

    public void addBinary(int aField, int aAttributes, byte[] aValue,
                          int aOffset, int aLength);

    public void setBinary(int aField, int aIndex, int aAttributes,
                          byte[] aValue, int aOffset, int aLength);

    public long getDate(int aField, int aIndex);

    public void addDate(int aField, int aAttributes, long aValue);

    public void setDate(int aField, int aIndex, int aAttributes, long aValue);

    public int getInt(int aField, int aIndex);

    public void addInt(int aField, int aAttributes, int aValue);

    public void setInt(int aField, int aIndex, int aAttributes, int aValue);

    public String getString(int aField, int aIndex);

    public void addString(int aField, int aAttributes, String aValue);

    public void setString(int aField, int aIndex, int aAttributes, String aValue);

    public boolean getBoolean(int aField, int aIndex);

    public void addBoolean(int aField, int aAttributes, boolean aValue);

    public void setBoolean(int aField, int aIndex, int aAttributes,
                           boolean aValue);

    public String[] getStringArray(int aField, int aIndex);

    public void addStringArray(int aField, int aAttributes, String[] aValue);

    public void setStringArray(int aField, int aIndex, int aAttributes,
                               String[] aValue);

    public int countValues(int aField);

    public void removeValue(int aField, int aIndex);

    public int getAttributes(int aField, int aIndex);

    public void addToCategory(String aCategory) throws PIMException;

    public void removeFromCategory(String aCategory);

    public String[] getCategories();

    public int maxCategories();
}
