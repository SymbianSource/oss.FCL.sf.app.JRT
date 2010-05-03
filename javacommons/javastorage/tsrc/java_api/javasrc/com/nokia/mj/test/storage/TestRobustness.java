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


package com.nokia.mj.test.storage;

import com.nokia.mj.test.storage.utils.StorageSessionTestUtils;
import com.nokia.mj.impl.storage.*;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageSession OTA table specific test cases. See test methods for test
 * case details.
 */
public class TestRobustness extends TestCase implements InstallerMain, StorageNames
{
    /**
     * Directory for JavaStorage tests.
     */
    private static final String iTestRoot = "./jstest";

    /**
     * Directory for JavaStorage journal and temp files.
     */
    private static final String iIsRoot = iTestRoot + "/js";

    private StorageSession iSession = null;
    private StorageSessionTestUtils iJtu = null;

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestRobustness("testAttributeValues", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRobustness)tc).testAttributeValues();
            }
        }));

        suite.addTest(new TestRobustness("testInvalidAttributeValues", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestRobustness)tc).testInvalidAttributeValues();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestRobustness()
    {
    }

    public TestRobustness(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    protected void setUp()
    {
        iSession = StorageFactory.createSession();
        iJtu = new StorageSessionTestUtils();
    }

    protected void tearDown()
    {
        if (iSession != null)
        {
            try
            {
                iSession.destroySession();
            }
            catch (StorageException se)
            {
                // No can do
                System.out.println("TearDown failed: " + se.toString());
            }
        }
    }

    /**
     * Test complicated attribute values. Write and read attributes with
     * complicated values to/from storage.
     *
     * 1. Value is 'null' string.
     * 2. Value contains ';' char.
     * 3. Value contains ''' char.
     * 4. Value contains '*' char.
     * 5. Value contains '!' char.
     * 6. Value contains ' ' char.
     * 7. Value contains '?' char.
     * 8. Value contains '#' char.
     * 9. Value contains '@' char.
     * 10. Value contains '€' char.
     * 11. Value contains '%' char.
     * 12. Value contains '/' char.
     * 13. Value contains '\' char.
     * 14. Value contains '\\' string.
     * 15. Value contains ':' char.
     * 16. Value contains '.' char.
     * 17. Value contains ',' char.
     * 18. Value contains '-' char.
     * 19. Value contains '_' char.
     * 20. Value contains '=' char.
     * 21. Value contains '>' char.
     * 22. Value contains '<' char.
     * 23. Value contains '|' char.
     * 24. Value contains '&' char.
     * 25. Value contains '&&' string.
     * 26. Value contains '||' string.
     * 27. Value contains '[' char.
     * 28. Value contains '[]' string.
     * 29. Value contains '{' char.
     * 30. Value contains '{}' string.
     * 31. Value contains 'AND' string.
     * 32. Value contains 'OR' string.
     * 33. Value contains '''' string.
     * 34. Value contains '$' char.
     * 35. Value contains '"' char.
     * 36. Value contains '\n' char.
     * 37. Value contains '\t' char.
     * 38. Value contains '\0' char.
     */
    public void testAttributeValues()
    {
        // 1. Value is 'null' string.
        executeAttributeTest("at1", "null");

        // 2. Value contains ';' char.
        executeAttributeTest("at2", "Great;Vendor");

        // 3. Value contains ''' char.
        executeAttributeTest("at3", "Band'Char'Inside`Again");

        // 4. Value contains '*' char.
        executeAttributeTest("at4", "Foo*Bar");

        // 5. Value contains '!' char.
        executeAttributeTest("at5", "Call Me!");

        // 6. Value contains ' ' char.
        executeAttributeTest("at6", "White space");

        // 7. Value contains '?' char.
        executeAttributeTest("at7", "Do you question me?");

        // 8. Value contains '#' char.
        executeAttributeTest("at8", "# code");

        // 9. Value contains '@' char.
        executeAttributeTest("at9", "temporary@address.com");

        // This is EURO character but SVN converts it to some different!
        // 10. Value contains Euro char. Two different euro codes
        executeAttributeTest("at10", "\u20ac");

        // 11. Value contains '%' char.
        executeAttributeTest("at11", "Done 100%");

        // 12. Value contains '/' char.
        executeAttributeTest("at12", "By/Gone");

        // 13. Value contains '\' char.
        executeAttributeTest("at13", "Gone\by");

        // 14. Value contains '\\' string.
        executeAttributeTest("at14", "Gone\\Beyond");

        // 15. Value contains ':' char.
        executeAttributeTest("at15", "Hmm:mmH");

        // 16. Value contains '.' char.
        executeAttributeTest("at16", "Dot.Point");

        // 17. Value contains ',' char.
        executeAttributeTest("at17", "comma,separated");

        // 18. Value contains '-' char.
        executeAttributeTest("at18", "Score-Me");

        // 19. Value contains '_' char.
        executeAttributeTest("at19", "Under_Score");

        // 20. Value contains '=' char.
        executeAttributeTest("at20", "Im=Equal");

        // 21. Value contains '>' char.
        executeAttributeTest("at21", "Greater>Value");

        // 22. Value contains '<' char.
        executeAttributeTest("at22", "Below<Boundary");

        // 23. Value contains '|' char.
        executeAttributeTest("at23", "Almos|Spacer");

        // 24. Value contains '&' char.
        executeAttributeTest("at24", "And&");

        // 25. Value contains '&&' string.
        executeAttributeTest("at25", "Double&&A");

        // 26. Value contains '||' string.
        executeAttributeTest("at26", "OrOr||");

        // 27. Value contains '[' char.
        executeAttributeTest("at27", "Keep[Separated");

        // 28. Value contains '[]' string.
        executeAttributeTest("at28", "[Surround Me]");

        // 29. Value contains '{' char.
        executeAttributeTest("at29", "Curly{One");

        // 30. Value contains '{}' string.
        executeAttributeTest("at30", "{Fully Curled}");

        // 31. Value contains 'AND' string.
        executeAttributeTest("at31", "Me AND Your");

        // 32. Value contains 'OR' string.
        executeAttributeTest("at32", "first OR second");

        // 33. Value contains '''' string.
        executeAttributeTest("at33", "'Oh'");

        // 34. Value contains '$' char.
        executeAttributeTest("at34", "$is Money");

        // 35. Value contains '"' char.
        executeAttributeTest("at35", "Time \"is\" Money");

        // 36. Value contains '\n' char.
        executeAttributeTest("at36", "Why \n not");

        // 37. Value contains '\t' char.
        executeAttributeTest("at37", "Decoration \t is beatiful");
    }

    /**
     * Test invalid attribute values.
     *
     * 1. Value contains '\0' char. StorageException is thrown.
     * 2. Storage name contains '\0' char. StorageException is thrown.
     */
    public void testInvalidAttributeValues()
    {
        // 1. Value contains '\0' char.
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName =  APPLICATION_PACKAGE_TABLE;
            StorageEntry entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(VENDOR, "Terminated\0Middle"));

            entry.addAttribute(
                new StorageAttribute(ID, "MyBrokenValue"));

            iSession.write(tableName, entry);

            iSession.rollbackTransaction();
            assertTrue("No exp when containing invalid char", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            assertTrue("UnExp exp1: " + t.toString(), false);
        }
        iSession.close();

        // 2. Storage name contains '\0' char. StorageException is thrown.
        try
        {
            iSession.open("Got enough\0Nulls");
            assertTrue("No exp when DB name containing invalid char", false);
        }
        catch (StorageException se)
        {
            // PASSED
        }
        catch (Throwable t)
        {
            assertTrue("UnExp exp2: " + t.toString(), false);
        }
    }
    private void executeAttributeTest(String aUid, String aValue)
    {
        try
        {
            iSession.open();
            iSession.startTransaction();

            String tableName =  APPLICATION_PACKAGE_TABLE;
            StorageEntry entry = new StorageEntry();

            entry.addAttribute(
                new StorageAttribute(VENDOR, aValue));

            assertTrue("Populate failed",
                       iJtu.populate(iSession, tableName, aUid, entry));

            assertTrue("Check failed",
                       iJtu.checkParams(iSession, tableName, entry, 1));

            // Utils add ID to entry
            entry.removeAttribute(ID);

            assertTrue("Remove failed",
                       iJtu.remove(iSession, tableName, aUid, entry));

            iSession.rollbackTransaction();
            iSession.close();
        }
        catch (Throwable t)
        {
            iSession.rollbackTransaction();
            iSession.close();
            assertTrue("UnExp exp1: " + t.toString(), false);
        }
    }
}
