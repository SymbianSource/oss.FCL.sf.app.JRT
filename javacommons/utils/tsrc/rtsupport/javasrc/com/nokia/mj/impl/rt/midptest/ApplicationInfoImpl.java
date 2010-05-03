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

package com.nokia.mj.impl.rt.midptest;

import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public class ApplicationInfoImpl extends ApplicationInfo
{

    /**
     */
    public String getRuntimeType()
    {
        System.out.println("(test) ApplicationInfoImpl.getRuntimeType()");
        return "MIDP_TEST";
    }

    /**
     */
    public String getProtectionDomain()
    {
        System.out.println("(test) ApplicationInfoImpl.getProtectionDomain()");
        return "TEST TTPD";
    }

    /**
     */
    public Uid getUid()
    {
        System.out.println("(test) ApplicationInfoImpl.getUid()");
        int tmpUid = 1234;
        return Uid.createUid(Integer.toString(tmpUid));
    }

    /**
     */
    public Uid getSuiteUid()
    {
        System.out.println("(test) ApplicationInfoImpl.getSuiteUid()");
        int tmpUid = 5678;
        return Uid.createUid(Integer.toString(tmpUid));
    }

    /**
     */
    public String getSuiteName()
    {
        System.out.println("(test) ApplicationInfoImpl.getSuiteName()");
        return "Test Suite";
    }

    /**
     */
    public String getName()
    {
        System.out.println("(test) ApplicationInfoImpl.getName()");
        return "Test Name";
    }

    /**
     */
    public String getVendor()
    {
        System.out.println("(test) ApplicationInfoImpl.getVendor()");
        return "Nokia Test";
    }

    /**
     */
    public String getVersion()
    {
        System.out.println("(test) ApplicationInfoImpl.getVersion()");
        return "1.2.3";
    }

    /**
     */
    public String getRootPath()
    {
        System.out.println("(test) ApplicationInfoImpl.getRootPath()");
        return "/home/";
    }

    /**
     */
    public String getMainClass()
    {
        System.out.println("(test) ApplicationInfoImpl.getMainClass()");
        return "TestMainClass";
    }

    /**
     */
    public String getAttribute(String key)
    {
        System.out.println("(test) ApplicationInfoImpl.getAttribute()");
        return "Test Attribute";
    }
}
