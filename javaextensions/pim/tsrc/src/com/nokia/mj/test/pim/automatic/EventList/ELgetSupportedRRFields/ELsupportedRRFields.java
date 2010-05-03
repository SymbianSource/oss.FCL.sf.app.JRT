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


//PACKAGE
package com.nokia.mj.test.pim.automatic.EventList.ELgetSupportedRRFields;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.RepeatRule;
import javax.microedition.pim.EventList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.EventList.getSupportedRepeatRuleFields().
 *
 */
public class ELsupportedRRFields
        extends MIDPTest
{

    public ELsupportedRRFields(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private String[] iRequiredEventLists = null;
    private Vector iRequiredFrequencies = new Vector(); // vector of int[]
    private Vector iRequiredFields = new Vector();  // vector of int[]
    private PIM p = null;
    private String[] eventListNames = null;
    private EventList[] el = null;

    public void runTest() throws TestEndThrowable
    {

        // setup test preconditions
        try
        {
            // get data from property file
            //log("parse properties");
            parseProperties();

            //log("open lists");
            p = PIM.getInstance();
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);

            int listCount = eventListNames.length;
            el = new EventList[ listCount ];

            for (int i = 0; i < listCount; i++)
            {
                el[ i ] = (EventList) p.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE,
                                                    eventListNames[ i ]);
            }
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        } // end of try-catch

        // check that required number of event lists is found
        if (eventListNames.length != iRequiredEventLists.length)
        {
            log("Number of EventLists not equal to required.");
            if (eventListNames.length < iRequiredEventLists.length)
            {
                failTest("Implementation lacks the required EventLists.");
            }
        }

        // test that all EventLists supports only required fields for all required frequencies
        for (int i=0; i < el.length; i++)
        {
            // get required frequencies first
            int[] reqArr = (int[]) iRequiredFrequencies.elementAt(i);
            log("EventList "+ iRequiredEventLists[i] + " requires frequencies: " + listAsString(reqArr));

            if (reqArr[0] != -1)
            {
                testFields(el[i], reqArr, (Vector) iRequiredFields.elementAt(i));
            }
        }

        // test that IAE is thrown if invalid frequency field is given as parameter
        int[] invalids = {-1000, -1, 0, 1, 15, 20, 200, 4096};
        for (int i=0; i < invalids.length; i++)
        {
            try
            {
                el[0].getSupportedRepeatRuleFields(invalids[i]);
                failTest("EventList.getSupportedRepeatRuleFields("+invalids[i]+") did not throw IAE!");
            }
            catch (IllegalArgumentException iae)
            {
                //ok
            }
            catch (Exception e)
            {
                failTest("EventList.getSupportedRepeatRuleFields("+invalids[i]+") threw wrong exception: "+e);
            }
        }

        // all is well
        passTest();

    }

    // test that EventList supports only required fields for all required frequencies
    private void testFields(EventList el, int[] frequencies, Vector validFieldVectors) throws TestEndThrowable
    {
        log("Testing required fields for frequencies for EventList "+ el.getName());

        // test all required frequencies
        for (int i=0; i < frequencies.length; i++)
        {
            // get supporteg fields
            int[] fields = el.getSupportedRepeatRuleFields(frequencies[i]);

            // get valid fields
            int[] reqArr = (int[]) validFieldVectors.elementAt(i);

            // compare fields
            if (reqArr[0] != -1)
            {
                log("Required fields for frequency "+frequencies[i] +": "+ listAsString(reqArr));
                if (!areEqual(fields, reqArr))
                {
                    log("Supported fields for frequency "+frequencies[i] +": "+ listAsString(fields));
                    failTest("EventList does not have the required" +
                             " fields for frequency "+ frequencies[i]);
                }
            }
            else
            {
                log("Required fields for frequency "+frequencies[i] +": EMPTY");
                if (fields.length != 0)
                {
                    log(listAsString(fields));
                    failTest("EventList has more than required" +
                             " fields for frequency "+ frequencies[i]);
                }
            }
        }
    }

    // tests that two int arrays have same values (may be in different order)
    private boolean areEqual(int[] a1, int[] a2)
    {
        if (a1.length != a2.length)
        {
            return false;
        }

        int[] arr1 = sort(a1);
        int[] arr2 = sort(a2);

        for (int i = 0; i < arr1.length; i++)
        {
            if (arr1[i] != arr2[i])
            {
                return false;
            }
        }
        return true;
    }

    // sorts an int array to ascending order
    // bubblesort is fine
    private int[] sort(int[] arr)
    {
        if (arr.length < 2)
        {
            return arr;
        }
        for (int j = 0; j < arr.length-1; j++)
        {
            for (int i = 0; i < arr.length-1-j; i++)
            {
                if (arr[i] > arr[i+1])
                {
                    int tmp = arr[i];
                    arr[i] = arr[i+1];
                    arr[i+1] = tmp;
                }
            }
        }
        return arr;
    }

    private String listAsString(int[] arr)
    {
        String val = "";
        for (int i = 0; i < arr.length; i++)
        {
            val += arr[i] + " ";
        }
        return val.trim();
    }

    private void parseProperties()
    {
        iRequiredEventLists = new String[] { "appointment", "event", "anniversary" };

        iRequiredFrequencies.addElement(new int[] { 16, 17, 18, 19 });
        iRequiredFrequencies.addElement(new int[] { -1 });
        iRequiredFrequencies.addElement(new int[] { 19 });

        // required fields for each frequency in list appointment
        Vector v1 = new Vector();
        v1.addElement(new int[] { 0 });
        v1.addElement(new int[] { 0, 128 });
        v1.addElement(new int[] { 0 });
        v1.addElement(new int[] { 0 });
        iRequiredFields.addElement(v1);

        // required fields for each frequency in list event
        Vector v2 = new Vector();
        iRequiredFields.addElement(v2);

        // required fields for each frequency in list anniversary
        Vector v3 = new Vector();
        v3.addElement(new int[] { 0 });
        iRequiredFields.addElement(v3);
    }
}