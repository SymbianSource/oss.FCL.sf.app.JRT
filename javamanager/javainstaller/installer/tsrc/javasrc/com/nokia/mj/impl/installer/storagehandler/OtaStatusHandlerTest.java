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


package com.nokia.mj.impl.installer.storagehandler;

import com.nokia.mj.impl.installer.storagehandler.OtaStatusHandler;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusNotification;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * OtaStatusHandler unit tests.
 */
public class OtaStatusHandlerTest extends TestCase implements InstallerMain
{
    private static final Uid TEST_UID = PlatformUid.createUid("[1234abcd]");
    private static final Uid TEST_UID2 = PlatformUid.createUid("[4321abcd]");

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new OtaStatusHandlerTest("testGetStatusReportBody", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OtaStatusHandlerTest)tc).testGetStatusReportBody();
            }
        }));

        suite.addTest(new OtaStatusHandlerTest("testCompareNotifications", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OtaStatusHandlerTest)tc).testCompareNotifications();
            }
        }));

        suite.addTest(new OtaStatusHandlerTest("testGetNoNotifications", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OtaStatusHandlerTest)tc).testGetNoNotifications();
            }
        }));

        suite.addTest(new OtaStatusHandlerTest("testAddGetRemoveNotifications", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OtaStatusHandlerTest)tc).testAddGetRemoveNotifications();
            }
        }));

        suite.addTest(new OtaStatusHandlerTest("testAddNotificationTwice", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OtaStatusHandlerTest)tc).testAddNotificationTwice();
            }
        }));

        suite.addTest(new OtaStatusHandlerTest("testRemoveNonexistentNotification", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OtaStatusHandlerTest)tc).testRemoveNonexistentNotification();
            }
        }));

        suite.addTest(new OtaStatusHandlerTest("testUpdateNotification", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OtaStatusHandlerTest)tc).testUpdateNotification();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public OtaStatusHandlerTest()
    {
    }

    public OtaStatusHandlerTest(String aTestName, TestMethod aTestMethod)
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
        cleanStorage();
    }

    protected void tearDown()
    {
        cleanStorage();
    }

    /**
     * Test getting status report body.
     */
    public void testGetStatusReportBody()
    {
        try
        {
            OtaStatusNotification n = createNotification(
                                          TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                                          OtaStatusCode.SUCCESS, "http://notifyme");
            assertTrue("Incorrect message " + n.getStatusReportBody(),
                       n.getStatusReportBody().equals("900 Success"));

            n = createNotification(
                    TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                    OtaStatusCode.LOSS_OF_SERVICE, "http://notifyme");
            assertTrue("Incorrect message " + n.getStatusReportBody(),
                       n.getStatusReportBody().equals("903 Loss of Service"));

            n = createNotification(
                    TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                    0, "http://notifyme");
            assertTrue("Incorrect message " + n.getStatusReportBody(),
                       n.getStatusReportBody().equals("0"));
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test comparing notifications.
     */
    public void testCompareNotifications()
    {
        try
        {
            OtaStatusNotification n1 = createNotification(
                                           TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                                           OtaStatusCode.SUCCESS, "http://notifyme");
            // Sleep a moment so that created notifications
            // get different timestamps.
            sleep(100);
            OtaStatusNotification n2 = createNotification(
                                           TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                                           OtaStatusCode.SUCCESS, "http://notifyme");
            //Log.log("testCompareNotifications: n1=\n" + n1);
            //Log.log("testCompareNotifications: n2=\n" + n2);
            assertTrue("Comparing notification to itself failed", n1.compareTo(n1) == 0);
            assertTrue("Comparing notification to bigger failed", n1.compareTo(n2) < 0);
            assertTrue("Comparing notification to smaller failed", n2.compareTo(n1) > 0);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test that OtaStatusHandler does not return any notifications
     * when no notifications exist.
     */
    public void testGetNoNotifications()
    {
        try
        {
            OtaStatusHandler osh = new OtaStatusHandler();
            OtaStatusNotification[] notifications = osh.getNotifications();
            assertTrue("Notifications found from empty storage!",
                       (notifications == null || notifications.length == 0));
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test adding notifications to storage, and getting and removing them.
     */
    public void testAddGetRemoveNotifications()
    {
        try
        {
            OtaStatusHandler osh = new OtaStatusHandler();
            OtaStatusNotification n1 = createNotification(
                                           TEST_UID2, OtaStatusNotification.TYPE_INSTALL,
                                           OtaStatusCode.SUCCESS, "http://notifyme");
            sleep(100);
            OtaStatusNotification n2 = createNotification(
                                           TEST_UID2, OtaStatusNotification.TYPE_INSTALL,
                                           OtaStatusCode.ATTRIBUTE_MISMATCH, "http://notifyme2");
            sleep(100);
            OtaStatusNotification n3 = createNotification(
                                           TEST_UID, OtaStatusNotification.TYPE_UNINSTALL,
                                           OtaStatusCode.SUCCESS, "http://notifyme3");
            sleep(100);
            OtaStatusNotification n4 = createNotification(
                                           TEST_UID, OtaStatusNotification.TYPE_UNINSTALL,
                                           OtaStatusCode.INSUFFICIENT_MEMORY, "http://notifyme4");

            osh.addNotification(n1);
            osh.addNotification(n2);
            osh.addNotification(n3);
            osh.addNotification(n4);

            OtaStatusNotification[] notifications = osh.getNotifications();
            assertTrue("1: Getting notifications returned null", notifications != null);
            assertTrue("1: Incorrect number of notifications", notifications.length == 4);
            assertTrue("1: Notification 1 does not match", notificationEqualsLog(notifications[0], n3));
            assertTrue("1: Notification 2 does not match", notificationEqualsLog(notifications[1], n4));
            assertTrue("1: Notification 3 does not match", notificationEqualsLog(notifications[2], n1));
            assertTrue("1: Notification 4 does not match", notificationEqualsLog(notifications[3], n2));

            osh.removeNotification(n2);

            notifications = osh.getNotifications();
            assertTrue("2: Getting notifications returned null", notifications != null);
            assertTrue("2: Incorrect number of notifications", notifications.length == 3);
            assertTrue("2: Notification 1 does not match", notificationEqualsLog(notifications[0], n3));
            assertTrue("2: Notification 2 does not match", notificationEqualsLog(notifications[1], n4));
            assertTrue("2: Notification 3 does not match", notificationEqualsLog(notifications[2], n1));

            osh.removeNotification(n1);

            notifications = osh.getNotifications();
            assertTrue("3: Getting notifications returned null", notifications != null);
            assertTrue("3: Incorrect number of notifications", notifications.length == 2);
            assertTrue("3: Notification 1 does not match", notificationEqualsLog(notifications[0], n3));
            assertTrue("3: Notification 2 does not match", notificationEqualsLog(notifications[1], n4));

            osh.removeNotification(n4);

            notifications = osh.getNotifications();
            assertTrue("4: Getting notifications returned null", notifications != null);
            assertTrue("4: Incorrect number of notifications", notifications.length == 1);
            assertTrue("4: Notification 1 does not match", notificationEqualsLog(notifications[0], n3));

            osh.removeNotification(n3);

            notifications = osh.getNotifications();
            assertTrue("5: Notifications found from empty storage!", notifications == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test adding the same notification twice to storage.
     */
    public void testAddNotificationTwice()
    {
        try
        {
            OtaStatusHandler osh = new OtaStatusHandler();
            OtaStatusNotification n1 = createNotification(
                                           TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                                           OtaStatusCode.SUCCESS, "http://notifyme");

            osh.addNotification(n1);

            OtaStatusNotification[] notifications = osh.getNotifications();
            assertTrue("1: Getting notifications returned null", notifications != null);
            assertTrue("1: Incorrect number of notifications", notifications.length == 1);
            assertTrue("1: Notification does not match", notificationEqualsLog(notifications[0], n1));

            try
            {
                osh.addNotification(n1);
                // OTA_STATUS_TABLE does not have primary key, which
                // means that re-adding already existing notification
                // does not throw exception.
                //assertTrue("1: Adding notification twice did not throw exception", false);
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                assertTrue("1: Unexpected exception: " + ex, false);
            }

            osh.removeNotification(n1);

            notifications = osh.getNotifications();
            assertTrue("2: Notifications found from empty storage!", notifications == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test removing nonexistent notification from storage.
     */
    public void testRemoveNonexistentNotification()
    {
        try
        {
            OtaStatusHandler osh = new OtaStatusHandler();
            OtaStatusNotification n1 = createNotification(
                                           TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                                           OtaStatusCode.SUCCESS, "http://notifyme");
            osh.removeNotification(n1);
            // OTA_STATUS_TABLE does not have primary key, which
            // means that removing nonexistent notification
            // does not throw exception.
            //assertTrue("Removing nonexistent notification did not throw exception", false);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test updating notification to storage.
     */
    public void testUpdateNotification()
    {
        try
        {
            OtaStatusHandler osh = new OtaStatusHandler();
            OtaStatusNotification n1 = createNotification(
                                           TEST_UID, OtaStatusNotification.TYPE_INSTALL,
                                           OtaStatusCode.SUCCESS, "http://notifyme");

            osh.addNotification(n1);

            OtaStatusNotification[] notifications = osh.getNotifications();
            assertTrue("1: Getting notifications returned null", notifications != null);
            assertTrue("1: Incorrect number of notifications", notifications.length == 1);
            assertTrue("1: Notification 1 does not match", notificationEqualsLog(notifications[0], n1));

            n1.setLatestRetryTime(System.currentTimeMillis());
            n1.setRetryCount(n1.getRetryCount()+1);
            osh.updateNotification(n1);

            notifications = osh.getNotifications();
            assertTrue("2: Getting notifications returned null", notifications != null);
            assertTrue("2: Incorrect number of notifications", notifications.length == 1);
            assertTrue("2: Notification 1 does not match", notificationEqualsLog(notifications[0], n1));

            osh.removeNotification(n1);

            notifications = osh.getNotifications();
            assertTrue("5: Notifications found from empty storage!", notifications == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Helper method for creating OtaStatusNotification
     * with current time as its creation time.
     */
    private static OtaStatusNotification createNotification(
        Uid aUid, int aType, int aCode, String aUrl)
    {
        return new OtaStatusNotification(
                   aUid, System.currentTimeMillis(),
                   aType, aCode, aUrl, 0, 0);
    }

    /**
     * Tests if given two notifications have all the same members.
     */
    private static boolean notificationEquals(
        OtaStatusNotification aN1, OtaStatusNotification aN2)
    {
        if (aN1 == null && aN2 == null)
        {
            return true;
        }

        if ((aN1 == null && aN2 != null) ||
                (aN1 != null && aN2 == null))
        {
            return false;
        }

        if (!aN1.getUid().equals(aN2.getUid()))
        {
            return false;
        }
        if (aN1.getCreationTime() != aN2.getCreationTime())
        {
            return false;
        }
        if (aN1.getType() != aN2.getType())
        {
            return false;
        }
        if (aN1.getOtaCode() != aN2.getOtaCode())
        {
            return false;
        }
        if (!aN1.getUrl().equals(aN2.getUrl()))
        {
            return false;
        }
        if (aN1.getLatestRetryTime() != aN2.getLatestRetryTime())
        {
            return false;
        }
        if (aN1.getRetryCount() != aN2.getRetryCount())
        {
            return false;
        }
        return true;
    }

    private static boolean notificationEqualsLog(
        OtaStatusNotification aN1, OtaStatusNotification aN2)
    {
        boolean result = notificationEquals(aN1, aN2);
        if (!result)
        {
            Log.log("Notifications are not equal:\n" + aN1 + "\n" + aN2);
        }
        return result;
    }

    private void cleanStorage()
    {
        StorageSession session = StorageFactory.createSession();
        if (session != null)
        {
            session.open(StorageNames.JAVA_OTA_DATABASE_NAME);
            session.startTransaction();
            session.remove(StorageNames.OTA_STATUS_TABLE, TEST_UID);
            session.remove(StorageNames.OTA_STATUS_TABLE, TEST_UID2);
            session.commitTransaction();
            session.close();
            session.destroySession();
        }
    }

    private static void sleep(long aMillis)
    {
        try
        {
            Thread.sleep(aMillis);
        }
        catch (InterruptedException ie)
        {
            // ignore
        }
    }
}
