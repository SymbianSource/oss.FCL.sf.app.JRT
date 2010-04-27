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
* Description:  PIM list interface base.
 *
*/


// PACKAGE
package javax.microedition.pim;

// IMPORTS
import java.util.Enumeration;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant PIMList interface.
 */
public interface PIMList
{
    public static final String UNCATEGORIZED = null;

    public abstract String getName();

    public abstract void close() throws PIMException;

    public abstract Enumeration items() throws PIMException;

    public abstract Enumeration items(PIMItem aMatchingItem)
    throws PIMException;

    public abstract Enumeration items(String aMatchingValue)
    throws PIMException;

    public abstract Enumeration itemsByCategory(String aCategory)
    throws PIMException;

    public String[] getCategories() throws PIMException;

    public boolean isCategory(String aCategory) throws PIMException;

    public void addCategory(String aCategory) throws PIMException;

    public void deleteCategory(String aCategory, boolean aDeleteUnassignedItems)
    throws PIMException;

    public void renameCategory(String aCurrentCategory, String aNewCategory)
    throws PIMException;

    public int maxCategories();

    public boolean isSupportedField(int aField);

    public int[] getSupportedFields();

    public boolean isSupportedAttribute(int aField, int aAttribute);

    public int[] getSupportedAttributes(int aField);

    public boolean isSupportedArrayElement(int aStringArrayField,
                                           int aArrayElement);

    public int[] getSupportedArrayElements(int aStringArrayField);

    public int getFieldDataType(int aField);

    public String getFieldLabel(int aField);

    public String getAttributeLabel(int aAttribute);

    public String getArrayElementLabel(int aStringArrayField, int aArrayElement);

    public int maxValues(int aField);

    public int stringArraySize(int aStringArrayField);
}
