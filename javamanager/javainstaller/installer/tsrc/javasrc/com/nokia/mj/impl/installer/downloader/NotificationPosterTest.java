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


package com.nokia.mj.impl.installer.downloader;

import com.nokia.mj.impl.installer.storagehandler.OtaStatusHandler;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusNotification;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.OtaStatusCode;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * NotificationPoster unit tests.
 */
public class NotificationPosterTest extends TestCase implements InstallerMain
{
    private static final String TEST_URL = "http://195.134.231.83:7070/java-server/getresult.jsp?phone=CE0CBA106ADB77A9298A143E5136CD2B&tID=900_success";

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new NotificationPosterTest("testIsNotificationUrl", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((NotificationPosterTest)tc).testIsNotificationUrl();
            }
        }));

        suite.addTest(new NotificationPosterTest("testNotifyStatusGcf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((NotificationPosterTest)tc).testNotifyStatusGcf();
            }
        }));

        suite.addTest(new NotificationPosterTest("testNotifyStatusFailureGcf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((NotificationPosterTest)tc).testNotifyStatusFailureGcf();
            }
        }));

        suite.addTest(new NotificationPosterTest("testNotifyPendingGcf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((NotificationPosterTest)tc).testNotifyPendingGcf();
            }
        }));

        suite.addTest(new NotificationPosterTest("testNotifyPendingFailureGcf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((NotificationPosterTest)tc).testNotifyPendingFailureGcf();
            }
        }));

        suite.addTest(new NotificationPosterTest("testNotifyPendingFailureLimitGcf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((NotificationPosterTest)tc).testNotifyPendingFailureLimitGcf();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public NotificationPosterTest()
    {
    }

    public NotificationPosterTest(String aTestName, TestMethod aTestMethod)
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
        removeAllNotificationsFromStorage();
    }

    protected void tearDown()
    {
        removeAllNotificationsFromStorage();
    }

    /**
     * Test isNotificationUrl method.
     */
    public void testIsNotificationUrl()
    {
        try
        {
            String[] urls = new String[]
            {
                "http://a.b.c/A",
                "https://a.b.c/A",
                "HTTP://a.b.c/A",
                "HTTPS://a.b.c/A",
            };
            for (int i = 0; i < urls.length; i++)
            {
                assertTrue("Expected true for " + urls[i],
                           NotificationPoster.isNotificationUrl(urls[i]));
            }

            urls = new String[]
            {
                "A",
                "file:///A",
            };
            for (int i = 0; i < urls.length; i++)
            {
                assertTrue("Expected false for " + urls[i],
                           !NotificationPoster.isNotificationUrl(urls[i]));
            }
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test a basic notification case.
     */
    public void testNotifyStatus(NotificationPoster poster)
    {
        try
        {
            OtaStatusNotification notification = createNotification(1);
            poster.notifyStatus(notification);
            // Check that notification is not saved to storage.
            OtaStatusHandler otaStatusHandler = new OtaStatusHandler();
            OtaStatusNotification[] notifications = otaStatusHandler.getNotifications();
            assertTrue("Notification exists in storage", notifications == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    public void testNotifyStatusGcf()
    {
        NotificationPoster poster = GcfNotificationPoster.getNotificationPoster();
        testNotifyStatus(poster);
    }

    /**
     * Test case where notification sending fails and notification is saved to storage.
     */
    public void testNotifyStatusFailure(NotificationPoster poster)
    {
        try
        {
            OtaStatusNotification notification = new OtaStatusNotification
            (PlatformUid.createUid("1"), System.currentTimeMillis(),
             OtaStatusNotification.TYPE_INSTALL, OtaStatusCode.SUCCESS,
             "http://unsupportedurl", System.currentTimeMillis(), 0);
            poster.notifyStatus(notification);
            // Check that notification is saved to storage.
            OtaStatusHandler otaStatusHandler = new OtaStatusHandler();
            OtaStatusNotification[] notifications = otaStatusHandler.getNotifications();
            assertTrue("Notification does not exist in storage", notifications != null);
            assertTrue("Number of notifications should be 1 but is " + notifications.length,
                       notifications.length == 1);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    public void testNotifyStatusFailureGcf()
    {
        NotificationPoster poster = GcfNotificationPoster.getNotificationPoster();
        testNotifyStatusFailure(poster);
    }

    /**
     * Test sending pending notifications.
     */
    public void testNotifyPending(NotificationPoster poster)
    {
        try
        {
            OtaStatusNotification[] notifications = new OtaStatusNotification[]
            {
                createNotification(1),
                createNotification(2),
                createNotification(3),
                createNotification(4),
                createNotification(5),
            };
            OtaStatusHandler otaStatusHandler = new OtaStatusHandler();
            for (int i = 0; i < notifications.length; i++)
            {
                otaStatusHandler.addNotification(notifications[i]);
            }
            poster.notifyPendingStatuses();
            poster.waitForCompletion();
            // Check that notifications got removed from storage.
            notifications = otaStatusHandler.getNotifications();
            assertTrue("Notifications still exist", notifications == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    public void testNotifyPendingGcf()
    {
        NotificationPoster poster = GcfNotificationPoster.getNotificationPoster();
        testNotifyPending(poster);
    }

    /**
     * Test that when sending some of the pending notifications fail,
     * they are still retained in storage.
     */
    public void testNotifyPendingFailure(NotificationPoster poster)
    {
        try
        {
            OtaStatusNotification[] notifications = new OtaStatusNotification[]
            {
                createNotification(1),
                createNotification(2),
                createNotification(3),
                new OtaStatusNotification(PlatformUid.createUid("4"),
                                          System.currentTimeMillis(),
                                          OtaStatusNotification.TYPE_INSTALL,
                                          OtaStatusCode.SUCCESS,
                                          "http://unsupportedurl",
                                          System.currentTimeMillis(), 0),
                createNotification(5),
            };
            OtaStatusHandler otaStatusHandler = new OtaStatusHandler();
            for (int i = 0; i < notifications.length; i++)
            {
                otaStatusHandler.addNotification(notifications[i]);
            }
            poster.notifyPendingStatuses();
            poster.waitForCompletion();
            // Check that one notification is still left into storage.
            notifications = otaStatusHandler.getNotifications();
            assertTrue("Notification does not exist in storage", notifications != null);
            assertTrue("Number of notifications should be 1 but is " + notifications.length,
                       notifications.length == 1);
            assertTrue("Notification retry count should be 1 but is " +
                       notifications[0].getRetryCount(),
                       notifications[0].getRetryCount() == 1);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    public void testNotifyPendingFailureGcf()
    {
        NotificationPoster poster = GcfNotificationPoster.getNotificationPoster();
        testNotifyPendingFailure(poster);
    }

    /**
     * Test that when sending pending notifications fail too many times,
     * notifications are removed from storage.
     */
    public void testNotifyPendingFailureLimit(NotificationPoster poster)
    {
        try
        {
            OtaStatusNotification[] notifications = new OtaStatusNotification[]
            {
                new OtaStatusNotification(PlatformUid.createUid("1"),
                                          System.currentTimeMillis(),
                                          OtaStatusNotification.TYPE_INSTALL,
                                          OtaStatusCode.SUCCESS,
                                          "http://unsupportedurl",
                                          System.currentTimeMillis(), 0),
                new OtaStatusNotification(PlatformUid.createUid("2"),
                                          System.currentTimeMillis(),
                                          OtaStatusNotification.TYPE_INSTALL,
                                          OtaStatusCode.SUCCESS,
                                          "http://unsupportedurl",
                                          System.currentTimeMillis(), 0),
            };
            OtaStatusHandler otaStatusHandler = new OtaStatusHandler();
            for (int i = 0; i < notifications.length; i++)
            {
                otaStatusHandler.addNotification(notifications[i]);
            }
            int maxRetryCount = 5;
            for (int i = 0; i < maxRetryCount; i++)
            {
                poster.notifyPendingStatuses();
                poster.waitForCompletion();
                // Check that notifications are still left into storage.
                notifications = otaStatusHandler.getNotifications();
                assertTrue("Notifications do not exist in storage", notifications != null);
                assertTrue("Number of notifications should be 2 but is " +
                           notifications.length, notifications.length == 2);
            }
            // Try to post notifications one more time and check that
            // after this the notifications are removed from storage.
            poster.notifyPendingStatuses();
            poster.waitForCompletion();
            notifications = otaStatusHandler.getNotifications();
            assertTrue("Notifications still exist in storage", notifications == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    public void testNotifyPendingFailureLimitGcf()
    {
        NotificationPoster poster = GcfNotificationPoster.getNotificationPoster();
        testNotifyPendingFailureLimit(poster);
    }

    private static OtaStatusNotification createNotification(int aIndex)
    {
        OtaStatusNotification notification = new OtaStatusNotification
        (PlatformUid.createUid(Integer.toString(aIndex)),
         System.currentTimeMillis(),
         OtaStatusNotification.TYPE_INSTALL, OtaStatusCode.SUCCESS,
         TEST_URL + aIndex, System.currentTimeMillis(), 0);
        return notification;
    }

    private static void removeAllNotificationsFromStorage()
    {
        try
        {
            OtaStatusHandler otaStatusHandler = new OtaStatusHandler();
            OtaStatusNotification[] notifications = otaStatusHandler.getNotifications();
            if (notifications != null)
            {
                for (int i = 0; i < notifications.length; i++)
                {
                    otaStatusHandler.removeNotification(notifications[i]);
                }
            }
        }
        catch (Exception ex)
        {
            System.out.println("removeAllNotificationsFromStorage: Unexpected exception: " + ex);
            ex.printStackTrace();
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
        }
    }
}
