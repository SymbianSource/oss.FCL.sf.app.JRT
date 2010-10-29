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


package com.nokia.mj.impl.installer.applicationregistrator;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Uid;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * ApplicationRegistrator unit tests.
 */
public class ApplicationRegistratorTest extends TestCase implements InstallerMain
{

    private static final String DEFAULT_SVG_ICON_PATH_NAME = "c:\\private\\102033E6\\installer\\jiutdata\\utils\\javahelper.mif";

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new ApplicationRegistratorTest("testStartAndCloseSession", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testStartAndCloseSession();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testStartAndCommitSession", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testStartAndCommitSession();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testUidInUse", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testUidInUse();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testGetGroupName", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testGetGroupName();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRegisterApplication", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRegisterApplication();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRegisterApplicationSucceeded", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRegisterApplicationSucceeded();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testUnregisterApplication", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testUnregisterApplication();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRollbackRegistration", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRollbackRegistration();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRollbackSucceeded", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRollbackSucceeded();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRegAppWithoutIcon", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRegAppWithoutIcon();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRegAppWithoutIconSucceeded", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRegAppWithoutIconSucceeded();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testUnregAppWithoutIcon", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testUnregAppWithoutIcon();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRegAppWithSuiteIcon", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRegAppWithSuiteIcon();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testRegAppWithSuiteIconSucceeded", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testRegAppWithSuiteIconSucceeded();
            }
        }));

        suite.addTest(new ApplicationRegistratorTest("testUnregAppWithSuiteIcon", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ApplicationRegistratorTest)tc).testUnregAppWithSuiteIcon();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public ApplicationRegistratorTest()
    {
    }

    public ApplicationRegistratorTest(String aTestName, TestMethod aTestMethod)
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
    }

    protected void tearDown()
    {
    }

    public void testStartAndCloseSession()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            appReg.startSession();
            // close the session immediately
            appReg.closeSession();
            // make sure that closeSession can be called even if the
            // session has already been closed
            appReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }

    public void testStartAndCommitSession()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            appReg.startSession();
            // commit the session immediately
            appReg.commitSession(true);

        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }

    public void testUidInUse()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // this Uid should be valid in S60
            Uid uid = Uid.createUid("[A0032000]");
            boolean inUse = false;

            // session must be started first
            try
            {
                inUse = appReg.uidInUse(uid);
                assertTrue("Calling UidInUse should not succeed without session!", false);
            }
            catch (InstallerException ie)
            {
                // this exception is espected
            }

            appReg.startSession();
            assertFalse("Uid should not be in use", appReg.uidInUse(uid));
            appReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }


    public void testGetGroupName()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // this Uid should be valid but non existing in S60
            Uid uid = Uid.createUid("[E0032000]");
            String groupName = "";

            // session must be started first
            try
            {
                groupName = appReg.getGroupName(uid);
                assertTrue("Calling getGroupName should not succeed without session!", false);
            }
            catch (InstallerException ie)
            {
                // this exception is espected
            }

            appReg.startSession();
            // there is no application with this uid, so this throws exception
            try
            {
                groupName = appReg.getGroupName(uid);
            }
            catch (InstallerException ie)
            {
                // ignore this expected exception
            }
            appReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }

    public void testRegisterApplication()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            Uid uid = Uid.createUid("[20032000]");
            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                uid,
                // String aGroupName,
                //"Games",
                "VeryVeryLongGroupNameThatMustBeShortened",
                // String aMIDletName,
                "GameMIDlet",
                // String aTargetDrive,
                "C:",
                // String aIconFileName contains the name of the converted icon,
                "c:\\private\\102033E6\\installer\\jiutdata\\icon1.mbm",
                // String aJarFileName,
                "c:\\private\\102033E6\\installer\\jiutdata\\utils\\einstein_EN_FR_IT_DE_ES_N97_v2942.jar",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            // session must be started first
            try
            {
                appReg.registerApplication(appRegInfo);
                assertTrue("Calling registerApplication should not succeed without session!", false);
            }
            catch (InstallerException ie)
            {
                // this exception is espected
            }

            appReg.startSession();

            // convert icon before registering
            StringBuffer iconSuffix = new StringBuffer();
            boolean convOk = appReg.convertIcon("icon.png", "c:\\private\\102033E6\\installer\\jiutdata\\icon1.mbm",
                                                "c:\\private\\102033E6\\installer\\jiutdata\\utils\\einstein_EN_FR_IT_DE_ES_N97_v2942.jar", iconSuffix);
            assertTrue("Icon conversion failed.", convOk);
            // Allowed icon suffixes .mbm and .png.
            String expectedSuffix = ".mbm";
            if (iconSuffix.toString().equals(".png"))
            {
                expectedSuffix = ".png";
            }
            assertTrue("Suffix of the converted icon is incorrect.",
                       iconSuffix.toString().equals(expectedSuffix));

            appReg.registerApplication(appRegInfo);
            appReg.commitSession(true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.commitSession(true);
        }
    }

    public void testRegisterApplicationSucceeded()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // application with this Uid was registered in testRegisterApplication
            Uid uid = Uid.createUid("[20032000]");
            String groupName = "";
            boolean uidInUse = false;

            appReg.startSession();
            groupName = appReg.getGroupName(uid);
            if (SifRegistrator.getSifMode() > 0)
            {
                // When USIF is in use ApplicationRegistrator should not
                // have registered anything.
                assertTrue(
                    "The group name was not empty, it was " + groupName,
                    "".equals(groupName));
            }
            else
            {
                assertTrue(
                    "The group name was not VeryVeryLongGrou, it was " + groupName,
                    "VeryVeryLongGrou".equals(groupName));
            }

            uidInUse = appReg.uidInUse(uid);
            if (SifRegistrator.getSifMode() > 0)
            {
                // When USIF is in use ApplicationRegistrator should not
                // have registered anything.
                assertTrue("uidInUse returned true value", false == uidInUse);
            }
            else
            {
                assertTrue("uidInUse returned false value", true == uidInUse);
            }

            appReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }

    public void testUnregisterApplication()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // application with this Uid was registered in testRegisterApplication
            Uid uid = Uid.createUid("[20032000]");

            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                uid,
                // String aGroupName,
                "",
                // String aMIDletName,
                "",
                // String aTargetDrive,
                "",
                // String aIconFileName, dummy
                "",
                // String aJarFileName, dummy
                "",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            appReg.startSession();
            appReg.unregisterApplication(appRegInfo);
            appReg.commitSession(true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }


    public void testRollbackRegistration()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            Uid uid = Uid.createUid("[20032001]");
            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                uid,
                // String aGroupName,
                "Games",
                // String aMIDletName,
                "GameMIDlet1",
                // String aTargetDrive,
                "C:",
                // String aIconFileName,  must be accessible and have \\ characters
                DEFAULT_SVG_ICON_PATH_NAME,
                // String aJarFileName,
                "c:\\private\\102033E6\\installer\\jiutdata\\utils\\einstein_EN_FR_IT_DE_ES_N97_v2942.jar",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            appReg.startSession();
            appReg.registerApplication(appRegInfo);
            appReg.rollbackSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }

    public void testRollbackSucceeded()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // application with this Uid was registered in testRollbackRegistration
            Uid uid = Uid.createUid("[20032001]");
            String groupName = "";
            boolean uidInUse = false;

            appReg.startSession();

            try
            {
                uidInUse = appReg.uidInUse(uid);
                assertTrue("uidInUse returned true although registration was rolled back", false == uidInUse);
            }
            catch (Exception e)
            {
                e.printStackTrace();
                assertTrue("Unexpected exception: " + e, false);
            }

            try
            {
                groupName = appReg.getGroupName(uid);
                // If the application cannot be found, returns ""
                assertTrue("getGroupName succeeded even though application registration was rolled back.",
                           "".equals(groupName));
            }
            catch (InstallerException e2)
            {
                e2.printStackTrace();
                assertTrue("Unexpected exception: " + e2, false);
            }

            appReg.closeSession();
        }
        catch (Exception ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }


    public void testRegAppWithoutIcon()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            Uid uid = Uid.createUid("[A0032005]");
            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                uid,
                // String aGroupName,
                "",
                // String aMIDletName,
                "NoIconMidlet",
                // String aTargetDrive,
                "C:",
                // String aIconFileName,
                DEFAULT_SVG_ICON_PATH_NAME,
                // String aJarFileName,
                "c:\\private\\102033E6\\installer\\jiutdata\\utils\\einstein_EN_FR_IT_DE_ES_N97_v2942.jar",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            // session must be started first
            try
            {
                appReg.registerApplication(appRegInfo);
                assertTrue("Calling registerApplication should not succeed without session!", false);
            }
            catch (InstallerException ie)
            {
                // this exception is espected
            }

            appReg.startSession();
            appReg.registerApplication(appRegInfo);
            appReg.commitSession(true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.commitSession(true);
        }
    }

    public void testRegAppWithoutIconSucceeded()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // application with this Uid was registered in testRegAppWithoutIcon
            Uid uid = Uid.createUid("[A0032005]");
            String groupName = "";
            boolean uidInUse = false;

            appReg.startSession();
            groupName = appReg.getGroupName(uid);
            assertTrue(
                "The group name was not empty, it was " + groupName,
                "".equals(groupName));

            uidInUse = appReg.uidInUse(uid);
            if (SifRegistrator.getSifMode() > 0)
            {
                // When USIF is in use ApplicationRegistrator should not
                // have registered anything.
                assertTrue("uidInUse returned true value", false == uidInUse);
            }
            else
            {
                assertTrue("uidInUse returned false value", true == uidInUse);
            }

            appReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }

    public void testUnregAppWithoutIcon()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // application with this Uid was registered in testRegAppWithoutIcon
            Uid uid = Uid.createUid("[A0032005]");

            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                uid,
                // String aGroupName,
                "",
                // String aMIDletName,
                "NoIconMidlet",
                // String aTargetDrive,
                "C:",
                // String aIconFileName,
                "",
                // String aJarFileName,
                "",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            appReg.startSession();
            appReg.unregisterApplication(appRegInfo);
            appReg.commitSession(true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }


    public void testRegAppWithSuiteIcon()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            Uid uid = Uid.createUid("[A0032FF5]");
            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                uid,
                // String aGroupName,
                "Suite",
                // String aMIDletName,
                "SuiteIconMidlet",
                // String aTargetDrive,
                "C:",
                // String aIconFileName,
                "c:\\private\\102033E6\\installer\\jiutdata\\icon2.mif",
                // String aJarFileName,
                "c:\\private\\102033E6\\installer\\jiutdata\\utils\\MIDPTestMisc.jar",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            // session must be started first
            try
            {
                appReg.registerApplication(appRegInfo);
                assertTrue("Calling registerApplication should not succeed without session!", false);
            }
            catch (InstallerException ie)
            {
                // this exception is espected
            }

            appReg.startSession();

            // convert svg icon before registering
            StringBuffer iconSuffix = new StringBuffer();
            boolean convOk = appReg.convertIcon("gear.svg", "c:\\private\\102033E6\\installer\\jiutdata\\icon2.mif",
                                                "c:\\private\\102033E6\\installer\\jiutdata\\utils\\MIDPTestMisc.jar", iconSuffix);
            assertTrue("Icon conversion failed.", convOk);
            // Allowed icon suffixes are .mif and .svg.
            String expectedSuffix = ".mif";
            if (iconSuffix.toString().equals(".svg"))
            {
                expectedSuffix = ".svg";
            }
            assertTrue("Suffix of the converted icon is incorrect.",
                       iconSuffix.toString().equals(expectedSuffix));

            appReg.registerApplication(appRegInfo);
            appReg.commitSession(true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.commitSession(true);
        }
    }

    public void testRegAppWithSuiteIconSucceeded()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // application with this Uid was registered in testRegAppWithSuiteIcon
            Uid uid = Uid.createUid("[A0032FF5]");
            String groupName = "";
            boolean uidInUse = false;

            appReg.startSession();
            groupName = appReg.getGroupName(uid);
            if (SifRegistrator.getSifMode() > 0)
            {
                // When USIF is in use ApplicationRegistrator should not
                // have registered anything.
                assertTrue(
                    "The group name was not empty, it was " + groupName,
                    "".equals(groupName));
            }
            else
            {
                assertTrue(
                    "The group name was not Suite, it was " + groupName,
                    "Suite".equals(groupName));
            }

            uidInUse = appReg.uidInUse(uid);
            if (SifRegistrator.getSifMode() > 0)
            {
                // When USIF is in use ApplicationRegistrator should not
                // have registered anything.
                assertTrue("uidInUse returned true value", false == uidInUse);
            }
            else
            {
                assertTrue("uidInUse returned false value", true == uidInUse);
            }

            appReg.closeSession();
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }


    public void testUnregAppWithSuiteIcon()
    {
        ApplicationRegistrator appReg = new ApplicationRegistrator();

        try
        {
            // application with this Uid was registered in testRegAppWithSuiteIcon
            Uid uid = Uid.createUid("[A0032FF5]");

            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                uid,
                // String aGroupName,
                "",
                // String aMIDletName,
                "",
                // String aTargetDrive,
                "",
                // String aIconFileName,
                "",
                // String aJarFileName,
                "",
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            appReg.startSession();
            appReg.unregisterApplication(appRegInfo);
            appReg.commitSession(true);
        }
        catch (InstallerException ie)
        {
            ie.printStackTrace();
            assertTrue("Unexpected exception: " + ie, false);
            appReg.closeSession();
        }
    }

}
