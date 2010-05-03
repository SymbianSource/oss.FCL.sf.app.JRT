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
package com.nokia.mj.test.pim.automatic;
//IMPORTS
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ContactList.createContact()
 */
public abstract class OpenMultipleLists
        extends MIDPTest
{
    /** Test-wide PIM instance. */
    private PIM iPim;

    /** Test-wide list type from derived class. */
    private int iListType;

    public OpenMultipleLists(String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
        iPim = PIM.getInstance();
        iListType = listType();
    }

    public void runTest() throws TestEndThrowable
    {
        log("+runTest()");
        runTestByMode(PIM.READ_WRITE);
        runTestByMode(PIM.READ_ONLY);
        runTestByMode(PIM.WRITE_ONLY);

        log("-runTest()");
        passTest();
    }


    /**
     * TEMPLATE METHOD: Determines the list type.
     *
     * Filled in derived classes.
     */
    abstract protected int listType();


    /**
     * Runs the test in given list access mode.
     */
    private void runTestByMode(
        int aMode)
    throws TestEndThrowable
    {
        log("+runTestByMode()");
        log("List type: " + iListType);
        log("List mode: " + aMode);
        try
        {
            String[] listNames = iPim.listPIMLists(iListType);

            log("Iterating through " + listNames.length + " different lists.");
            for (int i = 0; i < listNames.length; i++)
            {
                String listName = listNames[ i ];
                int numListInstances = 3;

                log("Part 1: open several instances & close them at the end.");

                log("Opening " + numListInstances
                    + " list instances of " + listName);

                PIMList[] listInstances = openMultipleListInstances(
                                              iListType,
                                              listName,
                                              aMode,
                                              numListInstances);

                closeMultipleListInstances(listInstances);

                log("Part 2: open and close several instances right away.");

                iterateOpenAndCloseList(
                    iListType,
                    listName,
                    aMode,
                    numListInstances);
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            failTest("Unexpected exception: " + e);
        }

        log("-runTestByMode()");
    }

    /**
     * Opens given number of instances of given list at the same time
     * and closes all the instances at the end.
     */
    private PIMList[] openMultipleListInstances(
        int aListType,
        String aListName,
        int aMode,
        int aNumListInstances)
    throws PIMException
    {
        log("+openMultipleListInstances()");

        PIMList[] listInstances = new PIMList[ aNumListInstances ];
        for (int i = 0; i < aNumListInstances; i++)
        {
            listInstances[ i ] = iPim.openPIMList(
                                     aListType,
                                     aMode,
                                     aListName);
        }

        log("-openMultipleListInstances()");
        return listInstances;
    }


    /**
     * Closes given lists.
     */
    private void closeMultipleListInstances(
        PIMList[] aListInstances)
    throws PIMException
    {
        log("+closeMultipleListInstances()");
        for (int i = 0; i < aListInstances.length; i++)
        {
            aListInstances[ i ].close();
        }
        log("-closeMultipleListInstances()");
    }


    /**
     * Opens and closes given list given times. The list is closed
     * before a new instance is opened.
     */
    private void iterateOpenAndCloseList(
        int aListType,
        String aListName,
        int aMode,
        int aNumIterations)
    throws PIMException
    {
        log("+iterateOpenAndCloseList()");

        for (int i = 0; i < aNumIterations; i++)
        {
            log("Opening list " + aListName + ", iteration "
                + i + "/" + aNumIterations + "...");

            PIMList list = iPim.openPIMList(aListType, aMode, aListName);
            list.close();
        }

        log("-iterateOpenAndCloseList()");
    }

}
