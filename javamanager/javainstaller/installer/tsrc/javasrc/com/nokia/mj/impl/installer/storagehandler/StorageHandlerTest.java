/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.storagehandler;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.storage.StorageAttribute;
import com.nokia.mj.impl.storage.StorageEntry;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Version;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * StorageHandler unit tests.
 */
public class StorageHandlerTest extends TestCase implements InstallerMain
{
    private static final int MAX_SUITE_COUNT = 8;
    private StorageHandler iStorageHandler = null;

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new StorageHandlerTest("testSuiteComparison", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testSuiteComparison();
            }
        }));

        suite.addTest(new StorageHandlerTest("testCommitTwice", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testCommitTwice();
            }
        }));

        suite.addTest(new StorageHandlerTest("testRollbackTwice", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testRollbackTwice();
            }
        }));

        suite.addTest(new StorageHandlerTest("testCommitRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testCommitRollback();
            }
        }));

        suite.addTest(new StorageHandlerTest("testRollbackCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testRollbackCommit();
            }
        }));

        suite.addTest(new StorageHandlerTest("testWriteReadByUidRemove", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testWriteReadByUidRemove();
            }
        }));

        suite.addTest(new StorageHandlerTest("testWriteReadByNameAndVendorRemove", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testWriteReadByNameAndVendorRemove();
            }
        }));

        suite.addTest(new StorageHandlerTest("testWriteCommitReadByUidRemoveCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testWriteCommitReadByUidRemoveCommit();
            }
        }));

        suite.addTest(new StorageHandlerTest("testWriteCommitReadByNameAndVendorRemoveCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testWriteCommitReadByNameAndVendorRemoveCommit();
            }
        }));

        suite.addTest(new StorageHandlerTest("testWriteRollbackReadByUid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testWriteRollbackReadByUid();
            }
        }));

        suite.addTest(new StorageHandlerTest("testWriteRollbackReadByNameAndVendor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testWriteRollbackReadByNameAndVendor();
            }
        }));

        suite.addTest(new StorageHandlerTest("testAllocateUid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testAllocateUid();
            }
        }));

        suite.addTest(new StorageHandlerTest("testGetSuiteUids", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testGetSuiteUids();
            }
        }));

        suite.addTest(new StorageHandlerTest("testGetSuiteUidFromAppUid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testGetSuiteUidFromAppUid();
            }
        }));

        suite.addTest(new StorageHandlerTest("testGetSuiteUidFromSuiteUid", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testGetSuiteUidFromSuiteUid();
            }
        }));

        suite.addTest(new StorageHandlerTest("testGetAppUids", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testGetAppUids();
            }
        }));

        suite.addTest(new StorageHandlerTest("testInstallPreinstall", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testInstallPreinstall();
            }
        }));

        suite.addTest(new StorageHandlerTest("testPreinstallInstall", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((StorageHandlerTest)tc).testPreinstallInstall();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public StorageHandlerTest()
    {
    }

    public StorageHandlerTest(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
        //System.out.println("----" + getName() + " setUp----");
        SuiteInfo[] suites = new SuiteInfo[MAX_SUITE_COUNT];
        for (int i = 0; i < suites.length; i++)
        {
            suites[i] = createSuiteInfo(i);
            cleanSuiteFromStorage(suites[i]);
        }
        iStorageHandler = new StorageHandler();
    }

    protected void tearDown()
    {
        iStorageHandler.close();
        iStorageHandler = null;
        //System.out.println("----" + getName() + " tearDown----");
    }

    public void testSuiteComparison()
    {
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suite2 = createSuiteInfo();
        // Test code for verifying that suite comparison works.
        assertTrue("expected suite to be equal", equals(suite, suite));
        assertTrue("expected suite2 to be equal", equals(suite, suite2));
        suite2.setAttribute("MIDlet-Version", "1.1");
        assertFalse("expected suite2 to not be equal", equals(suite, suite2));
    }

    public void testCommitTwice()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        tryCommitSession();
        tryCommitSessionFails();
    }

    public void testRollbackTwice()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        tryRollbackSession();
        tryRollbackSessionFails();
    }

    public void testCommitRollback()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        tryCommitSession();
        tryRollbackSessionFails();
    }

    public void testRollbackCommit()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        tryRollbackSession();
        tryCommitSessionFails();
    }

    public void testWriteReadByUidRemove()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));

        tryRemoveSuite(suiteOrig);
        tryReadNoSuite(suiteOrig.getUid());

        tryRollbackSession();

        // After rollback we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        tryReadNoSuite(suiteOrig.getUid());
        iStorageHandler.close();
    }

    public void testWriteReadByNameAndVendorRemove()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        suite = tryReadSuite(suiteOrig.getName(), suiteOrig.getVendor());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));

        tryRemoveSuite(suiteOrig);
        tryReadNoSuite(suiteOrig.getUid());

        tryRollbackSession();

        // After rollback we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        tryReadNoSuite(suiteOrig.getUid());
        iStorageHandler.close();
    }

    public void testWriteCommitReadByUidRemoveCommit()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));

        tryCommitSession();

        // After commit we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));
        iStorageHandler.close();

        iStorageHandler = new StorageHandler();
        tryStartSession();

        tryRemoveSuite(suiteOrig);
        tryReadNoSuite(suiteOrig.getUid());

        tryCommitSession();

        // After commit we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        tryReadNoSuite(suiteOrig.getUid());
        iStorageHandler.close();
    }

    public void testWriteCommitReadByNameAndVendorRemoveCommit()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        suite = tryReadSuite(suiteOrig.getName(), suiteOrig.getVendor());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));

        tryCommitSession();

        // After commit we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        suite = tryReadSuite(suiteOrig.getName(), suiteOrig.getVendor());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));
        iStorageHandler.close();

        iStorageHandler = new StorageHandler();
        tryStartSession();

        tryRemoveSuite(suiteOrig);
        tryReadNoSuite(suiteOrig.getUid());

        tryCommitSession();

        // After commit we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        tryReadNoSuite(suiteOrig.getUid());
        iStorageHandler.close();
    }

    public void testWriteRollbackReadByUid()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));

        tryRollbackSession();

        // After rollback we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        tryReadNoSuite(suiteOrig.getUid());
        iStorageHandler.close();
    }

    public void testWriteRollbackReadByNameAndVendor()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        SuiteInfo suiteOrig = suite;
        tryWriteSuite(suite);
        suite = tryReadSuite(suiteOrig.getName(), suiteOrig.getVendor());
        assertTrue("expected suite from storage to be equal with original",
                   equals(suite, suiteOrig));

        tryRollbackSession();

        // After rollback we have to get a new StorageHandler instance.
        iStorageHandler = new StorageHandler();
        tryReadNoSuite(suiteOrig.getUid());
        iStorageHandler.close();
    }

    public void testAllocateUid()
    {
        Uid uid = iStorageHandler.allocateUid("");
        assertTrue("expected uid to be not null", uid != null);
    }

    public void testGetSuiteUids()
    {
        tryStartSession();
        SuiteInfo[] suites = new SuiteInfo[MAX_SUITE_COUNT];
        for (int i = 0; i < suites.length; i++)
        {
            suites[i] = createSuiteInfo(i);
            tryWriteSuite(suites[i]);
        }

        try
        {
            Uid[] uids = iStorageHandler.getSuiteUids();
            assertTrue("expected uids to be not null", uids != null);
            assertTrue("expected uids.length to be suites.length",
                       uids.length == suites.length);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Exception at getSuiteUids: " + ex.toString(), false);
        }

        tryRollbackSession();

        try
        {
            StorageHandler storageHandler = new StorageHandler();
            Uid[] uids = storageHandler.getSuiteUids();
            storageHandler.close();
            assertTrue("expected uids to be null", uids == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Exception at getSuiteUids after rollback: " +
                       ex.toString(), false);
        }
    }

    public void testGetSuiteUidFromAppUid()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        tryWriteSuite(suite);

        Uid[] appUids = suite.getApplicationUids();
        assertTrue("expected appUids not to be null", appUids != null);
        for (int i = 0; i < appUids.length; i++)
        {
            Uid uid = iStorageHandler.getSuiteUid(appUids[i]);
            assertTrue("expected uids to be equal", suite.getUid().equals(uid));
        }

        tryRollbackSession();
    }

    public void testGetSuiteUidFromSuiteUid()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        tryWriteSuite(suite);

        Uid uid = iStorageHandler.getSuiteUid(suite.getUid());
        assertTrue("expected uids to be equal", suite.getUid().equals(uid));

        tryRollbackSession();
    }

    public void testGetAppUids()
    {
        tryStartSession();
        SuiteInfo suite = createSuiteInfo();
        tryWriteSuite(suite);

        Uid[] appUidsOrig = suite.getApplicationUids();
        Uid[] appUids = iStorageHandler.getAppUids(suite.getUid());
        assertTrue("expected appUids not to be null", appUids != null);
        assertTrue("appUids length does not match",
                   appUidsOrig.length == appUids.length);
        boolean uidFound = false;
        for (int i = 0; i < appUids.length; i++)
        {
            for (int j = 0; j < appUidsOrig.length; j++)
            {
                if (appUids[i].equals(appUidsOrig[j]))
                {
                    // Found matching uid.
                    uidFound = true;
                    break;
                }
            }
        }
        assertTrue("incorrect uid returned: " + appUids +
                   ", expected: " + appUidsOrig, uidFound);

        tryRollbackSession();
    }

    public void testInstallPreinstall()
    {
        tryStartSession();
        SuiteInfo suiteOrig = createSuiteInfo();

        suiteOrig.iPreinstallState = SuiteInfo.STATE_INSTALLED;
        tryWriteSuite(suiteOrig);
        SuiteInfo suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("suite is null", suite != null);
        assertTrue("preinstall state != INSTALLED: " + suite.iPreinstallState,
                   suite.iPreinstallState == SuiteInfo.STATE_INSTALLED);

        tryRemoveSuite(suiteOrig);
        int preinstallState = readPreinstallState(suiteOrig);
        assertTrue("preinstall state != -1: " + preinstallState,
                   preinstallState == -1);

        suiteOrig.iPreinstallState = SuiteInfo.STATE_PREINSTALLED;
        tryWriteSuite(suiteOrig);
        suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("suite is null", suite != null);
        assertTrue("preinstall state 2 != PREINSTALLED: " + suite.iPreinstallState,
                   suite.iPreinstallState == SuiteInfo.STATE_PREINSTALLED);

        tryRemoveSuite(suiteOrig);
        tryReadNoSuite(suiteOrig.getUid());

        preinstallState = readPreinstallState(suiteOrig);
        assertTrue("preinstall state 3 != NO_PREINSTALL: " + preinstallState,
                   preinstallState == SuiteInfo.STATE_NO_PREINSTALL);

        tryRollbackSession();
    }

    public void testPreinstallInstall()
    {
        tryStartSession();
        SuiteInfo suiteOrig = createSuiteInfo();

        suiteOrig.iPreinstallState = SuiteInfo.STATE_PREINSTALLED;
        tryWriteSuite(suiteOrig);
        SuiteInfo suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("suite is null", suite != null);
        assertTrue("preinstall state != PREINSTALLED: " + suite.iPreinstallState,
                   suite.iPreinstallState == SuiteInfo.STATE_PREINSTALLED);

        tryRemoveSuite(suiteOrig);
        int preinstallState = readPreinstallState(suiteOrig);
        assertTrue("preinstall state 2 != NO_PREINSTALL: " + preinstallState,
                   preinstallState == SuiteInfo.STATE_NO_PREINSTALL);

        suiteOrig.iPreinstallState = SuiteInfo.STATE_INSTALLED;
        tryWriteSuite(suiteOrig);
        suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("suite is null", suite != null);
        assertTrue("preinstall state 3 != NO_PREINSTALL: " + suite.iPreinstallState,
                   suite.iPreinstallState == SuiteInfo.STATE_NO_PREINSTALL);

        tryRemoveSuite(suiteOrig);
        preinstallState = readPreinstallState(suiteOrig);
        assertTrue("preinstall state 4 != NO_PREINSTALL: " + preinstallState,
                   preinstallState == SuiteInfo.STATE_NO_PREINSTALL);

        suiteOrig.iPreinstallState = SuiteInfo.STATE_PREINSTALLED;
        tryWriteSuite(suiteOrig);
        suite = tryReadSuite(suiteOrig.getUid());
        assertTrue("suite is null", suite != null);
        assertTrue("preinstall state 5 != PREINSTALLED: " + suite.iPreinstallState,
                   suite.iPreinstallState == SuiteInfo.STATE_PREINSTALLED);

        tryRollbackSession();
    }

    private SuiteInfo createSuiteInfo()
    {
        return createSuiteInfo(0);
    }

    private SuiteInfo createSuiteInfo(int aIndex)
    {
        Uid uid = Uid.createUid("[eeff000" + aIndex + "]");
        SuiteInfo suiteInfo = new SuiteInfo(uid);
        suiteInfo.setName("TestSuite");
        suiteInfo.setVendor("Nokia");
        suiteInfo.setVersion(Version.getVersion("1.0"));
        suiteInfo.setRootDir("/private/midlets/" + uid.getStringValue());
        suiteInfo.setJadPath(suiteInfo.getRootDir() + "/TestMidlet.jad");
        suiteInfo.setJarPath(suiteInfo.getRootDir() + "/TestMidlet.jar");
        suiteInfo.setMediaId(1234);
        suiteInfo.setInitialSize(50000);
        suiteInfo.setOnScreenKeypad(SuiteInfo.OSK_NO);
        suiteInfo.setAccessPoint("0");
        Hashtable attrs = new Hashtable();
        attrs.put("Created-By",
                  new Attribute("Created-By", "1.5.0_06 (Sun Microsystems Inc.)"));
        attrs.put("Manifest-Version",
                  new Attribute("Manifest-Version", "1.0"));
        attrs.put("MicroEdition-Configuration",
                  new Attribute("MicroEdition-Configuration", "CLDC-1.0"));
        attrs.put("MIDlet-Name",
                  new Attribute("MIDlet-Name", "TestSuite" + aIndex));
        attrs.put("MIDlet-Vendor",
                  new Attribute("MIDlet-Vendor", "Nokia"));
        attrs.put("MIDlet-1",
                  new Attribute("MIDlet-1", "MIDlet1, , test.MIDlet1"));
        attrs.put("MIDlet-2",
                  new Attribute("MIDlet-2", "MIDlet2, , test.MIDlet2"));
        attrs.put("MIDlet-Version",
                  new Attribute("MIDlet-Version", "1.0"));
        attrs.put("MicroEdition-Profile",
                  new Attribute("MicroEdition-Profile", "MIDP-1.0"));
        suiteInfo.setAttributes(attrs);
        Vector apps = new Vector();
        apps.addElement(new ApplicationInfo(Uid.createUid("[eeff001" + aIndex + "]"),
                                            "MIDlet-1", "", "test.MIDlet1", 0));
        apps.addElement(new ApplicationInfo(Uid.createUid("[eeff002" + aIndex + "]"),
                                            "MIDlet-2", "", "test.MIDlet2", 0));
        suiteInfo.setApplications(apps);
        return suiteInfo;
    }

    private boolean equals(SuiteInfo aSuiteInfo1, SuiteInfo aSuiteInfo2)
    {
        boolean result = true;
        if ((aSuiteInfo1 == null && aSuiteInfo2 != null) ||
                (aSuiteInfo1 != null && aSuiteInfo2 == null))
        {
            logEquals("other suite is null");
            result = false;
        }
        if (result && !aSuiteInfo1.getUid().equals(aSuiteInfo2.getUid()))
        {
            logEquals("uids don't match");
            result = false;
        }
        if (result && !aSuiteInfo1.getName().equals(aSuiteInfo2.getName()))
        {
            logEquals("names don't match");
            result = false;
        }
        if (result && !aSuiteInfo1.getVendor().equals(aSuiteInfo2.getVendor()))
        {
            logEquals("vendors don't match");
            result = false;
        }
        if (result && !aSuiteInfo1.getVersion().equals(aSuiteInfo2.getVersion()))
        {
            logEquals("versions don't match");
            result = false;
        }
        if (result && !aSuiteInfo1.getJadPath().equals(aSuiteInfo2.getJadPath()))
        {
            logEquals("jadPaths don't match");
            result = false;
        }
        if (result && !aSuiteInfo1.getJarPath().equals(aSuiteInfo2.getJarPath()))
        {
            logEquals("jarPaths don't match");
            result = false;
        }
        if (result && !aSuiteInfo1.getRootDir().equals(aSuiteInfo2.getRootDir()))
        {
            logEquals("rootDirs don't match");
            result = false;
        }
        if (result && !aSuiteInfo1.getAccessPoint().equals(aSuiteInfo2.getAccessPoint()))
        {
            logEquals("accessPoints don't match");
            result = false;
        }
        if (result && aSuiteInfo1.getMediaId() != aSuiteInfo2.getMediaId())
        {
            logEquals("mediaIds don't match");
            result = false;
        }
        if (result && aSuiteInfo1.getInitialSize() !=
                aSuiteInfo2.getInitialSize())
        {
            logEquals("initSize don't match");
            result = false;
        }
        if (result && aSuiteInfo1.getOnScreenKeypad() !=
                aSuiteInfo2.getOnScreenKeypad())
        {
            logEquals("onScreenKeypads don't match");
            result = false;
        }
        if (result)
        {
            Vector apps1 = aSuiteInfo1.getApplications();
            Vector apps2 = aSuiteInfo2.getApplications();
            if ((apps1 == null && apps2 != null) ||
                    (apps1 != null && apps2 == null))
            {
                logEquals("other app is null");
                result = false;
            }
            if (result && apps1.size() != apps2.size())
            {
                logEquals("app sizes don't match");
                result = false;
            }
            if (result)
            {
                for (int i = 0; i < apps1.size(); i++)
                {
                    if (!apps1.elementAt(i).equals(apps2.elementAt(i)))
                    {
                        logEquals("apps[" + i + "] don't match");
                        logEquals("apps1: " + apps1.elementAt(i));
                        logEquals("apps2: " + apps2.elementAt(i));
                        result = false;
                        break;
                    }
                }
            }
        }
        if (result && !equalTables(aSuiteInfo1.getAttributes(),
                                   aSuiteInfo2.getAttributes()))
        {
            logEquals("attributes don't match");
            result = false;
        }
        return result;
    }

    private boolean equalTables(Hashtable t1, Hashtable t2)
    {
        if (t1 == null && t2 == null)
        {
            return true;
        }
        boolean result = true;
        if ((t1 == null && t2 != null) ||
                (t1 != null && t2 == null))
        {
            result = false;
        }
        if (result && t1.size() != t2.size())
        {
            result = false;
        }
        if (result)
        {
            // Check that all values in t1 can be found from t2.
            Enumeration e = t1.keys();
            while (result && e.hasMoreElements())
            {
                Object key = e.nextElement();
                Object v1 = t1.get(key);
                Object v2 = t2.get(key);
                if (!v1.equals(v2))
                {
                    result = false;
                }
            }
        }
        if (result)
        {
            // Check that all values in t2 can be found from t1.
            Enumeration e = t2.keys();
            while (result && e.hasMoreElements())
            {
                Object key = e.nextElement();
                Object v1 = t1.get(key);
                Object v2 = t2.get(key);
                if (!v2.equals(v1))
                {
                    result = false;
                }
            }
        }
        return result;
    }

    private void logEquals(String aMsg)
    {
        System.out.println("StorageHandlerTest.equals: " + aMsg);
    }

    private void tryStartSession()
    {
        try
        {
            iStorageHandler.startSession();
        }
        catch (Exception ex)
        {
            Log.logError("tryStartSession exception", ex);
            assertTrue("Exception at startSession: " + ex.toString(), false);
        }
    }

    private void tryCommitSession()
    {
        try
        {
            iStorageHandler.commitSession();
        }
        catch (Exception ex)
        {
            Log.logError("tryCommitSession exception", ex);
            assertTrue("Exception at commitSession: " + ex.toString(), false);
        }
    }

    private void tryCommitSessionFails()
    {
        try
        {
            iStorageHandler.commitSession();
            assertTrue("No Exception at commitSession", false);
        }
        catch (Exception ex)
        {
            assertTrue("Exception at commitSession: " + ex.toString(), true);
        }
    }

    private void tryRollbackSession()
    {
        try
        {
            iStorageHandler.rollbackSession();
        }
        catch (Exception ex)
        {
            Log.logError("tryRollbackSession exception", ex);
            assertTrue("Exception at rollbackSession: " + ex.toString(), false);
        }
    }

    private void tryRollbackSessionFails()
    {
        try
        {
            iStorageHandler.rollbackSession();
            assertTrue("No Exception at rollbackSession", false);
        }
        catch (Exception ex)
        {
            assertTrue("Exception at rollbackSession: " + ex.toString(), true);
        }
    }

    private void tryWriteSuite(SuiteInfo aSuite)
    {
        try
        {
            iStorageHandler.writeSuiteInfo(aSuite);
        }
        catch (Exception ex)
        {
            Log.logError("tryWriteSuite exception", ex);
            assertTrue("Exception at writeSuiteInfo: " + ex.toString(), false);
        }
    }

    private SuiteInfo tryReadSuite(Uid aUid)
    {
        SuiteInfo suite = null;
        try
        {
            suite = new SuiteInfo(aUid);
            assertTrue("expected suite reading to succeed",
                       iStorageHandler.readSuiteInfo(suite));
        }
        catch (Exception ex)
        {
            Log.logError("tryReadSuite exception", ex);
            assertTrue("Exception at readSuiteInfo: " + ex.toString(), false);
        }
        return suite;
    }

    private void tryReadNoSuite(Uid aUid)
    {
        SuiteInfo suite = null;
        try
        {
            suite = new SuiteInfo(aUid);
            assertFalse("expected suite reading to fail",
                        iStorageHandler.readSuiteInfo(suite));
        }
        catch (Exception ex)
        {
            Log.logError("tryReadNoSuite exception", ex);
            assertTrue("Exception at readSuiteInfo: " + ex.toString(), false);
        }
    }

    private SuiteInfo tryReadSuite(String aName, String aVendor)
    {
        SuiteInfo suite = null;
        try
        {
            suite = new SuiteInfo(aName, aVendor);
            assertTrue("expected suite reading to succeed",
                       iStorageHandler.readSuiteInfo(suite));
        }
        catch (Exception ex)
        {
            Log.logError("tryReadSuite exception", ex);
            assertTrue("Exception at readSuiteInfo: " + ex.toString(), false);
        }
        return suite;
    }

    private void tryReadNoSuite(String aName, String aVendor)
    {
        SuiteInfo suite = null;
        try
        {
            suite = new SuiteInfo(aName, aVendor);
            assertFalse("expected suite reading to fail",
                        iStorageHandler.readSuiteInfo(suite));
        }
        catch (Exception ex)
        {
            Log.logError("tryReadNoSuite exception", ex);
            assertTrue("Exception at readSuiteInfo: " + ex.toString(), false);
        }
    }

    private void tryRemoveSuite(SuiteInfo aSuiteInfo)
    {
        try
        {
            iStorageHandler.removeSuiteInfo(aSuiteInfo);
        }
        catch (Exception ex)
        {
            Log.logError("tryRemoveSuite exception", ex);
            assertTrue("Exception at removeSuiteInfo: " + ex.toString(), false);
        }
    }

    /**
     * Reads preinstall state from INSTALL_STATE column
     * value from TBL_PREINSTALL table.
     * If INSTALL_STATE value is not found, returns -1.
     */
    private int readPreinstallState(SuiteInfo aSuiteInfo)
    {
        int result = -1;
        // Match preinstall state using name and vendor.
        StorageEntry query = new StorageEntry();
        query.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        query.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));
        query.addAttribute(new StorageAttribute(StorageNames.INSTALL_STATE, ""));
        StorageEntry[] entries =
            iStorageHandler.getSession().search
            (StorageNames.PREINSTALL_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            // Fill suite info.
            StorageEntry entry = entries[0];
            result = Integer.parseInt
                     (StorageHandler.getAttributeValue
                      (entry, StorageNames.INSTALL_STATE));
        }
        return result;
    }

    /**
     * Removes preinstall state for given suite from preinstall table.
     */
    private void removePreinstallState(StorageSession aSession, SuiteInfo aSuiteInfo)
    {
        if (aSuiteInfo == null)
        {
            return;
        }
        // Match preinstall state using name and vendor.
        StorageEntry matchEntry = new StorageEntry();
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));
        aSession.remove(StorageNames.PREINSTALL_TABLE, matchEntry);
    }

    /**
     * Removes suite data from MIDP_PACKAGE_TABLE table.
     */
    private void removeMidpPackageData(StorageSession aSession, SuiteInfo aSuiteInfo)
    {
        Uid uid = aSuiteInfo.getUid();
        StorageEntry matchEntry = new StorageEntry();
        matchEntry.addAttribute
        (new StorageAttribute(StorageNames.ID, uid.getStringValue()));
        StorageEntry[] entries =
            aSession.search(StorageNames.MIDP_PACKAGE_TABLE, matchEntry);
        if (entries != null && entries.length > 0)
        {
            // Entry exists, remove it.
            aSession.remove(StorageNames.MIDP_PACKAGE_TABLE, uid);
        }
    }

    private void cleanSuiteFromStorage(SuiteInfo aSuiteInfo)
    {
        StorageHandler storageHandler = new StorageHandler();
        try
        {
            storageHandler.startSession();
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Exception at cleanStorage startSession: " + ex.toString(), false);
            return;
        }

        try
        {
            storageHandler.removeSuiteInfo(aSuiteInfo);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Exception at cleanStorage removeSuiteInfo: " + ex.toString(), false);
            return;
        }

        try
        {
            removeMidpPackageData(storageHandler.getSession(), aSuiteInfo);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Exception at cleanStorage removeMidpPackageData: " + ex.toString(), false);
            return;
        }

        try
        {
            removePreinstallState(storageHandler.getSession(), aSuiteInfo);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Exception at cleanStorage removePreinstallState: " + ex.toString(), false);
            return;
        }

        try
        {
            storageHandler.commitSession();
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Exception at cleanStorage commitSession: " + ex.toString(), false);
            return;
        }
    }
}
