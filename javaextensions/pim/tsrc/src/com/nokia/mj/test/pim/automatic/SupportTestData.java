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


// PACKAGE
package com.nokia.mj.test.pim.automatic;
// IMPORTS
import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;

// CLASS DEFINITION
/**
 * This is a data structure class for the test to easy
 * the development and maintenance of the PIM API FUTEs
 * There are functions for adding and getting the support
 * test data
 */

public class SupportTestData
{
    private Vector iSupportedFields = null;
    private Vector iUnsupportedFields = null;
    private Hashtable iSupportedAttributes = null;
    private Hashtable iSupportedArrayElements = null;
    private Hashtable iFieldValues = null;
    private Hashtable iVersitValues = null;

    /**
     * Constructor. Initializes the data structures
     */
    public SupportTestData()
    {
        iSupportedFields = new Vector();
        iUnsupportedFields = new Vector();
        iSupportedAttributes = new Hashtable();
        iSupportedArrayElements = new Hashtable();
        iFieldValues = new Hashtable();
        iVersitValues = new Hashtable();
    }

    /**
     * Adds new supported field to test data. These fields
     * can be feched with getSupportedFields()
     *
     * @param aSupportedField Value of the added supported field
     */
    public void addSupportedField(int aSupportedField)
    {
        iSupportedFields.addElement(new Integer(aSupportedField));
    }

    /**
     * Returns added supported fields. Empty vector if there are
     * no supported fields
     *
     * @return Vector containing added supported fields
     */
    public Vector getSupportedFields()
    {
        return iSupportedFields;
    }

    /**
     * Adds new unsupported field to test data. These fields
     * can be feched with getUnsupportedFields()
     *
     * @param aUnsupportedField Value of the added unsupported field
     */
    public void addUnsupportedField(int aUnsupportedField)
    {
        iUnsupportedFields.addElement(new Integer(aUnsupportedField));
    }

    /**
     * Returns added unsupported fields. Empty vector if there are
     * no supported fields
     *
     * @return Vector containing added unsupported fields
     */
    public Vector getUnsupportedFields()
    {
        return iUnsupportedFields;
    }

    /**
     * Adds new supported attributes to a field
     *
     * @param aField which is to be added
     * @param aAttributes Attributes of this field
     */
    public void addSupportedAttributes(int aField, int aAttributes)
    {
        iSupportedAttributes.put(new Integer(aField), new Integer(aAttributes));
    }

    /**
     * Returns the fields used in supported attribute tests
     *
     * @return Vector containing the fields of the supported attributes
     */
    public Vector getSupportedAttributeFields()
    {
        Vector fields = new Vector();

        Enumeration keys = iSupportedAttributes.keys();

        for (int i = 0; i < iSupportedAttributes.size(); i++)
        {
            int field = ((Integer) keys.nextElement()).intValue();
            fields.addElement(new Integer(field));
        }

        return fields;
    }

    /**
     * Returns the supported attributes of a specific field
     *
     * @return Array containing supported attributes of this field
     */
    public int getSupportedAttributes(int aField)
    {
        return ((Integer) iSupportedAttributes.get(
                    new Integer(aField))).intValue();
    }

    /**
     * Adds new supported array element to supported array elements
     *
     * @param aField Field which array element is to be added
     * @param aArrayElement Array element to be added
     */
    public void addSupportedArrayElement(int aField, int aArrayElement)
    {
        Vector arrayElements = null;
        if (iSupportedArrayElements.containsKey(new Integer(aField)))
        {
            arrayElements = ((Vector) iSupportedArrayElements.get(
                                 new Integer(aField)));
            if (arrayElements == null)
            {
                arrayElements = new Vector();
            }
            arrayElements.addElement(new Integer(aArrayElement));
        }
        else
        {
            arrayElements = new Vector();
            arrayElements.addElement(new Integer(aArrayElement));
        }

        iSupportedArrayElements.put(new Integer(aField), arrayElements);
    }

    /**
     * Returns the fields used in supported array elmement tests
     *
     * @return Vector containing the fields of the supported array elements
     */
    public Vector getSupportedArrayElementFields()
    {
        Vector fields = new Vector();

        Enumeration keys = iSupportedArrayElements.keys();

        for (int i = 0; i < iSupportedArrayElements.size(); i++)
        {
            int field = ((Integer) keys.nextElement()).intValue();
            fields.addElement(new Integer(field));
        }

        return fields;
    }

    /**
     * Returns the supported array elements of a specific field
     *
     * @param aField A fields which attributes should be returned
     * @return Array containing supported array elements of this field
     */
    public Vector getSupportedArrayElements(int aField)
    {
        return ((Vector) iSupportedArrayElements.get(
                    new Integer(aField)));
    }

    /**
     * Adds values for a PIM field. This method can be used
     * to define test data with field's value and expected versit
     * conversion result. Attributes of the field can also be defined
     *
     * NOTE: There are overloaded versions for all value types or if not
     *       please add one :)
     */
    public void addFieldData(
        int aField,
        int aAttributes,
        String aValue,
        String aVersitResult)
    {
        // Add new attributes for the field. Older attributes not overwritten
        addFieldAttributes(aField, aAttributes);
        addFieldValue(aField, aValue);
        addVersitValue(aField, aVersitResult);
    }

    public void addFieldData(
        int aField,
        int aAttributes,
        String[] aValues,
        String aVersitResult)
    {
        // Add new attributes for the field. Older attributes not overwritten
        addFieldAttributes(aField, aAttributes);
        addFieldValue(aField, aValues);
        addVersitValue(aField, aVersitResult);
    }

    public void addFieldData(
        int aField,
        int aAttributes,
        long aValue,
        String aVersitResult)
    {
        // Add new attributes for the field. Older attributes not overwritten
        addFieldAttributes(aField, aAttributes);
        addFieldValue(aField, new Long(aValue));
        addVersitValue(aField, aVersitResult);
    }

    /**
     * Returns initialized attribute combinations of the specified field
     */
    public Vector getFieldAttributeCombinations(int aField)
    {
        return ((Vector) iSupportedAttributes.get(new Integer(aField)));
    }

    /**
     * Returns the test value of specific field
     */
    public Vector getFieldValue(int aField)
    {
        return ((Vector) iFieldValues.get(new Integer(aField)));
    }

    /**
     *  Returns the versit test value of specific field
     */
    public Vector getVersitValue(int aField)
    {
        return ((Vector) iVersitValues.get(new Integer(aField)));
    }

    // PRIVATE MEMBER FUNCTIONS

    private void addFieldAttributes(int aField, int aAttributes)
    {
        Vector attributesArray = null;

        if (iSupportedAttributes.containsKey(new Integer(aField)))
        {
            attributesArray = ((Vector) iSupportedAttributes.get(
                                   new Integer(aField)));
            if (attributesArray == null)
            {
                attributesArray = new Vector();
            }
            attributesArray.addElement(new Integer(aAttributes));
        }
        else
        {
            attributesArray = new Vector();
            attributesArray.addElement(new Integer(aAttributes));
        }

        iSupportedAttributes.put(new Integer(aField), attributesArray);
    }

    private void addFieldValue(int aField, Object aValue)
    {
        Vector valueArray = null;

        if (iFieldValues.containsKey(new Integer(aField)))
        {
            valueArray = ((Vector) iFieldValues.get(
                              new Integer(aField)));
            if (valueArray == null)
            {
                valueArray = new Vector();
            }
            valueArray.addElement(aValue);
        }
        else
        {
            valueArray = new Vector();
            valueArray.addElement(aValue);
        }

        iFieldValues.put(new Integer(aField), valueArray);
    }

    private void addVersitValue(int aField, Object aValue)
    {
        Vector valueArray = null;

        if (iVersitValues.containsKey(new Integer(aField)))
        {
            valueArray = ((Vector) iVersitValues.get(
                              new Integer(aField)));
            if (valueArray == null)
            {
                valueArray = new Vector();
            }
            valueArray.addElement(aValue);
        }
        else
        {
            valueArray = new Vector();
            valueArray.addElement(aValue);
        }

        iVersitValues.put(new Integer(aField), valueArray);
    }
}

// End of file

