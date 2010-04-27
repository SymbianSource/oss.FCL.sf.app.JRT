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
package com.nokia.mj.impl.rt.test;

/*
-jcl:cdc11:nokiaextcdc -Dcom.nokia.jvm.port=j9.JvmPortCdc -Dcom.nokia.rt.port=midptest -Dcom.ibm.j9.ext.dirs=c:\java\jsr\classes\ -Dno_anonymous com.nokia.mj.impl.rt.midptest.MidpTest
-jcl:cldc11:nokiaextcldc -Dcom.nokia.jvm.port=j9.JvmPortCldc -Dcom.nokia.rt.port=midptest -Dcom.ibm.j9.ext.dirs=c:\java\jsr\classes\ -Dno_anonymous com.nokia.mj.impl.rt.midptest.MidpTest

java -Djava.ext.dirs=z:\epoc32\winscw\c\java\jsr\classes -Dcom.nokia.jvm.port=sun.JvmPortJ2se -Dcom.nokia.rt.port=midptest com.nokia.mj.impl.rt.midptest.MidpTest
java -Djava.ext.dirs=${JAVA_BIN_ROOT}/jsr/classes -Dcom.nokia.jvm.port=sun.JvmPortJ2se -Dcom.nokia.rt.port=midptest com.nokia.mj.impl.rt.midptest.MidpTest



AccessControllerImpl:

    private void init()
    {
        this.MSID = MSID;
//        grantedPermissions = null; // Read it from the storage
        grantedPermissions = new PolicyBasedPermission[1]; // Read it from the storage
        PolicyBasedPermissionImpl pi = new PolicyBasedPermissionImpl("com.nokia.mj.impl.rt.test.TestPermission","",null, 0);
        grantedPermissions[0] = pi;
        securityPromptHandler = new SecurityPromptHandler();
    }

*/

import com.nokia.mj.impl.rt.support.*;
import java.security.AccessControlException;

public final class TestMain
{
    static boolean useAnonymousApproach = true;

    public TestMain()
    {
        System.out.println("***TestMain()***\n");
        System.out.println("java.version: " + System.getProperty("java.version"));
        System.out.println("java.fullversion: " + System.getProperty("java.fullversion")+"\n");

        System.out.println("Test starts ");

        try
        {
            try
            {
                JvmInternal.enableRuntimeExit();
            }
            catch (Throwable th)
            {
            }
//            Jvm.loadSystemLibrary("");

            TestPermission p = new TestPermission("Test");
            try
            {
                ApplicationUtils.getInstance().checkPermission(p);
                System.out.println("TestPermission1 ACCEPTED.");
            }
            catch (AccessControlException a)
            {
                System.out.println("TestPermission1 DENIED.");
            }
            catch (Throwable th)
            {
                System.out.println("TestPermission1 FAILED: " + th);
                th.printStackTrace();
            }

            try
            {
                ApplicationUtils.getInstance().checkPermission(p,
                        new PromptTextProvider()
                {
                    public String[] getText()
                    {
                        String[] sa = new String[1];
                        sa[0] = "The Text";
                        return sa;
                    }
                    public String getText(int index)
                    {
                        if (index == 0)
                        {
                            return "The Text";
                        }
                        return null;
                    }
                }
                                                              );
                System.out.println("TestPermission3 ACCEPTED.");
            }
            catch (AccessControlException a)
            {
                System.out.println("TestPermission3 DENIED.");
            }
            catch (Throwable th)
            {
                System.out.println("TestPermission3 FAILED: " + th);
                th.printStackTrace();
            }


            ApplicationInfo appInfo = ApplicationInfo.getInstance();

            System.out.println("getRuntimeType: " + appInfo.getRuntimeType());
            System.out.println("getProtectionDomain: " + appInfo.getProtectionDomain());
            System.out.println("getUid: " + appInfo.getUid());
            System.out.println("getSuiteUid: " + appInfo.getSuiteUid());
            System.out.println("getSuiteName: " + appInfo.getSuiteName());
            System.out.println("getName: " + appInfo.getName());
            System.out.println("getVendor: " + appInfo.getVendor());
            System.out.println("getVersion: " + appInfo.getVersion());
            System.out.println("getRootPath: " + appInfo.getRootPath());
            System.out.println("getMainClass: " + appInfo.getMainClass());
            System.out.println("getAttribute: " + appInfo.getAttribute("MIDlet-1"));

            System.out.println("no_anonymous: " + System.getProperty("no_anonymous"));
            System.out.println("test.version: " + System.getProperty("test.version"));
            System.out.println("test2.version: " + System.getProperty("test2.version"));
            System.out.println("test3: " + System.getProperty("test3"));
            System.out.println("test.dynamic: " + System.getProperty("test.dynamic"));
            System.out.println("test.dynamic: " + System.getProperty("test.dynamic"));
            System.out.println("test.dynamic2: " + System.getProperty("test.dynamic2"));
            System.out.println("test.dynamic2: " + System.getProperty("test.dynamic2"));
            try
            {
                System.setProperty("testColon", ":colon");
                java.util.Properties pr = System.getProperties();
                System.out.println("CDC test.version: " + pr.getProperty("test.version"));
                System.out.println("CDC test.dynamic: " + pr.getProperty("test.dynamic"));
                System.out.println("CDC test.dynamic2: " + pr.getProperty("test.dynamic2"));
                System.out.println("CDC testColon: " + pr.getProperty("testColon"));
//
//                pr.list(System.out);
//                pr.list(new java.io.PrintWriter(System.out));
            }
            catch (Throwable th)
            {
                System.out.println("Skipping CLDC "+th);
                //th.printStackTrace();
            }

            if (System.getProperty("no_anonymous") != null)
            {
                useAnonymousApproach = false;
            }
            System.out.println("Using anonymous = " + useAnonymousApproach);
            for (int k = 0; k< 1; k++)
            {
                new Test1();
                new Test2();
                new Test3();
                new Test4();
                new Test5();
                new Test6();
            }
            System.gc();
            System.gc();
            JvmInternal.runFinalization();
            useAnonymousApproach = false;
            FinalizerStatistics.printStatus();

            ApplicationUtils appUtils = ApplicationUtils.getInstance();
            //Get the name of the application.
            appUtils.addShutdownListener(new ShutdownListener()
            {
                public void shuttingDown()
                {
                    System.out.println("shuttingDown1");
                }

            });

            appUtils.addShutdownListener(new ShutdownListener()
            {
                public void shuttingDown()
                {
                    System.out.println("shuttingDown2");
                }

            });

            appUtils.addShutdownListener(new ShutdownListener()
            {
                public void shuttingDown()
                {
                    System.out.println("shuttingDown3");
                }

            });

            appUtils.addShutdownListener(new ShutdownListener()
            {
                public void shuttingDown()
                {
                    System.out.println("shuttingDown4");
                }

            });

            appUtils.notifyExitCmd();



        }

        catch (Throwable th)
        {
            System.err.println("Error in tests: "+th);
            th.printStackTrace();
        }
        System.out.println("Test ends ");
    }

}

