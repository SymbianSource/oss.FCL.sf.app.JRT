/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.runtime.rtport;

import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.utils.Uid;

public class RuntimeInfoImpl implements RuntimeInfo
{

    UiToolkitRegisterImpl tk = new UiToolkitRegisterImpl();

    public UiToolkitRegister getUiToolkitRegister()
    {
        return tk;
    }

    public Object getApplicationId()
    {
//        return ApplicationInfo.getInstance().getName();
        return this;
    }

    public int getApplicationUid()
    {
        try
        {
            String u = ApplicationInfo.getInstance().getUid().getStringValue();
            return Integer.parseInt(u.substring(1,u.length()-1), 16);
        }
        catch(Throwable t)
        {
        }
        System.out.println("RuntimeInfoImpl.getApplicationUid(): USING DEFAULT");
        return 0x2001843A; // The JavaInstaller uid.
    }

    public String getApplicationDomain()
    {
        /*try
        {
            return ApplicationInfo.getInstance().getProtectionDomain();
        }
        catch (Throwable t)
        {
        }*/
        System.out.println("RuntimeInfoImpl.getApplicationDomain(): USING DEFAULT");
        return RuntimeInfo.MANUFACTURER_DOMAIN;
    }

    public void notifyExitCmd(int aUid)
    {
        try
        {
            ApplicationUtils.getInstance().notifyExitCmd();
        }
        catch(Throwable t)
        {
            System.out.println("RuntimeInfoImpl.notifyExitCmd(): Failed");
        }
    }
}
