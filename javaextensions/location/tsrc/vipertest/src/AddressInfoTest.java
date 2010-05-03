/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
import javax.microedition.location.*;

/*
 * This suite of testcases tests that:
 * - AddressInfo object can be created and changed
 * - legal parameters are accepted
 * - illegal parameters are not accepted
 */
public class AddressInfoTest extends ViperUnitTest
{
    private AddressInfo currentAddressInfo;

    public AddressInfoTest()
    {
        super("AddressInfoTest");
    }

    protected void runTest() throws java.lang.Throwable
    {
        testConstructor();
        testSetters();
        testIllegalArgumentSet();
        testIllegalArgumentGet();
    }

    // Test that constructor works and that default values are correct.
    void testConstructor()
    {
        setCurrentTest("testConstructor()");

        // Create a AddressInfo object with correct parameters and check that
        // the values are unchanged when read.
        AddressInfo addrInfo = new AddressInfo();

        // Check the defaults
        assertTrue(addrInfo.getField(AddressInfo.EXTENSION) == null
                   && addrInfo.getField(AddressInfo.STREET) == null
                   && addrInfo.getField(AddressInfo.POSTAL_CODE) == null
                   && addrInfo.getField(AddressInfo.CITY) == null
                   && addrInfo.getField(AddressInfo.COUNTY) == null
                   && addrInfo.getField(AddressInfo.DISTRICT) == null
                   && addrInfo.getField(AddressInfo.STATE) == null
                   && addrInfo.getField(AddressInfo.COUNTRY) == null
                   && addrInfo.getField(AddressInfo.COUNTRY_CODE) == null
                   && addrInfo.getField(AddressInfo.BUILDING_NAME) == null
                   && addrInfo.getField(AddressInfo.BUILDING_FLOOR) == null
                   && addrInfo.getField(AddressInfo.BUILDING_ROOM) == null
                   && addrInfo.getField(AddressInfo.BUILDING_ZONE) == null
                   && addrInfo.getField(AddressInfo.CROSSING1) == null
                   && addrInfo.getField(AddressInfo.CROSSING2) == null
                   && addrInfo.getField(AddressInfo.URL) == null
                   && addrInfo.getField(AddressInfo.PHONE_NUMBER) == null,
                   "Default AddressInfo values incorrect");

    }

    // Test that all the setters work correctly.
    void testSetters()
    {
        setCurrentTest("testSetters()");

        // List the new values
        String extension = "Flat 4";
        String street = "11 Mount Avenue";
        String city = "London";
        String county = "Ealing";
        String postal_code = "W5 1QB";
        String state = "England";
        String district = "Middlesex";
        String country = "United Kingdom";
        String country_code = "GB";
        String building_name = "The Castle";
        String building_floor = "3";
        String building_room = "Front Room";
        String building_zone = "Upstairs";
        String crossing1 = "Mount Avenue";
        String crossing2 = "Eaton Rise";
        String url = "http://www.upmystreet.co.uk";
        String phone_number = "+358401234567";

        // Define a AddressInfo object with non-default parameters and check
        // that the values are unchanged when read.
        AddressInfo addrInfo = new AddressInfo();
        addrInfo.setField(AddressInfo.EXTENSION, extension);
        addrInfo.setField(AddressInfo.STREET, street);
        addrInfo.setField(AddressInfo.POSTAL_CODE, postal_code);
        addrInfo.setField(AddressInfo.CITY, city);
        addrInfo.setField(AddressInfo.COUNTY, county);
        addrInfo.setField(AddressInfo.DISTRICT, district);
        addrInfo.setField(AddressInfo.STATE, state);
        addrInfo.setField(AddressInfo.COUNTRY, country);
        addrInfo.setField(AddressInfo.COUNTRY_CODE, country_code);
        addrInfo.setField(AddressInfo.BUILDING_NAME, building_name);
        addrInfo.setField(AddressInfo.BUILDING_FLOOR, building_floor);
        addrInfo.setField(AddressInfo.BUILDING_ROOM, building_room);
        addrInfo.setField(AddressInfo.BUILDING_ZONE, building_zone);
        addrInfo.setField(AddressInfo.CROSSING1, crossing1);
        addrInfo.setField(AddressInfo.CROSSING2, crossing2);
        addrInfo.setField(AddressInfo.URL, url);
        addrInfo.setField(AddressInfo.PHONE_NUMBER, phone_number);

        // Check that the retrieved values are the same as the input ones
        assertTrue(
            addrInfo.getField(AddressInfo.EXTENSION) == extension
            && addrInfo.getField(AddressInfo.STREET) == street
            && addrInfo.getField(AddressInfo.POSTAL_CODE) == postal_code
            && addrInfo.getField(AddressInfo.CITY) == city
            && addrInfo.getField(AddressInfo.COUNTY) == county
            && addrInfo.getField(AddressInfo.DISTRICT) == district
            && addrInfo.getField(AddressInfo.STATE) == state
            && addrInfo.getField(AddressInfo.COUNTRY) == country
            && addrInfo.getField(AddressInfo.COUNTRY_CODE) == country_code
            && addrInfo.getField(AddressInfo.BUILDING_NAME) == building_name
            && addrInfo.getField(AddressInfo.BUILDING_FLOOR) == building_floor
            && addrInfo.getField(AddressInfo.BUILDING_ROOM) == building_room
            && addrInfo.getField(AddressInfo.BUILDING_ZONE) == building_zone
            && addrInfo.getField(AddressInfo.CROSSING1) == crossing1
            && addrInfo.getField(AddressInfo.CROSSING2) == crossing2
            && addrInfo.getField(AddressInfo.URL) == url
            && addrInfo.getField(AddressInfo.PHONE_NUMBER) == phone_number,
            "Retrieved AddressInfo values different from input");

    }

    // Test that an exception is thrown if the field used in setter is
    // undefined.
    void testIllegalArgumentSet()
    {
        setCurrentTest("testIllegalArgumentSet()");

        AddressInfo addrInfo = new AddressInfo();

        // List the illegal field and value to set
        int illegalField = 77;
        String street = "11 Mount Avenue";

        try
        {
            addrInfo.setField(illegalField, street);

            // If this point is reached, something is wrong
            assertTrue(false, "No exception thrown when illegal field used");

        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
        }
    }

    // Test that an exception is thrown if the requested field is undefined.
    void testIllegalArgumentGet()
    {
        setCurrentTest("testIllegalArgumentGet()");

        AddressInfo addrInfo = new AddressInfo();

        int illegalField = 77;

        try
        {
            addrInfo.getField(illegalField);

            // If this point is reached, something is wrong
            assertTrue(false, "No exception thrown when illegal field used");

        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
        }
    }
}
