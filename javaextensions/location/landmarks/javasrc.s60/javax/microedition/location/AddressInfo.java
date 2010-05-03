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
* Description:  JSR-179 Location API version 1.0.1 AddressInfo class
 *
*/


// PACKAGE
package javax.microedition.location;

// IMPORTS
import java.util.Vector;

// CLASS DESCRIPTION
/**
 * JSR-179 Location API version 1.0.1 AddressInfo class
 *
 * Please refer JSR-179 spec
 *
 */
public class AddressInfo
{
    public static final int EXTENSION = 1;

    public static final int STREET = 2;

    public static final int POSTAL_CODE = 3;

    public static final int CITY = 4;

    public static final int COUNTY = 5;

    public static final int STATE = 6;

    public static final int COUNTRY = 7;

    public static final int COUNTRY_CODE = 8;

    public static final int DISTRICT = 9;

    public static final int BUILDING_NAME = 10;

    public static final int BUILDING_FLOOR = 11;

    public static final int BUILDING_ROOM = 12;

    public static final int BUILDING_ZONE = 13;

    public static final int CROSSING1 = 14;

    public static final int CROSSING2 = 15;

    public static final int URL = 16;

    public static final int PHONE_NUMBER = 17;

    private static final String INVALID_FIELD_ID_ERROR
    = "Field ID is invalid: ";

    // Value indicating the number of the supported fields. Note
    // that the value is package private!
    static final int NUM_FIELDS = PHONE_NUMBER;

    // Vector holding the values of address info fields
    private Vector iFieldValues;

    /**
     * Default constructor of this class
     */
    public AddressInfo()
    {
        iFieldValues = new Vector(NUM_FIELDS);
        // By default all values are null
        for (int i = 0; i < NUM_FIELDS; i++)
        {
            iFieldValues.addElement(null);
        }
    }

    /**
     * Please refer JSR-179 spec
     */
    public String getField(int aField)
    {
        if (aField <= 0 || aField > NUM_FIELDS)
        {
            throw new IllegalArgumentException(INVALID_FIELD_ID_ERROR + aField);
        }
        // Returns null if there is no element in the field
        return (String) iFieldValues.elementAt(aField - 1);
    }

    /**
     * Please refer JSR-179 spec
     */
    public void setField(int aField, String aValue)
    {
        if (aField <= 0 || aField > NUM_FIELDS)
        {
            throw new IllegalArgumentException(INVALID_FIELD_ID_ERROR + aField);
        }
        iFieldValues.setElementAt(aValue, aField - 1);
    }
}

// End of file
