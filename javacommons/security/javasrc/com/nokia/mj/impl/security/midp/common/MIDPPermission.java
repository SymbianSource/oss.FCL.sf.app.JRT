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

package com.nokia.mj.impl.security.midp.common;

/**
 * MIDP authorization is about protecting access to sensitive functionality.
 * One of the main concepts in MIDP authorization is Permission:
 *    - the means to protect access to sensitive functionality
 *    - it is a triplet {permission class, target, action} where target defines
 *      the protected resource and the action defines the action to be carried
 *      upon the resource
 * This class encapsulates a Permission object.
 */
public class MIDPPermission
{
    /**
     * The name of the permission
     */
    protected String name;

    /**
     * The target of the permission
     */
    protected String target;

    /**
     * The action list
     */
    protected String actionList;

    /**
     * The individual actions.
     * We keep them explicitly so that the parsing of the individual actions is
     * done only once
     */
    private String[] actions;

    public MIDPPermission(String name, String target, String actionList)
    {
        this.name = name;
        this.target = target;
        this.actionList = actionList;
    }

    public MIDPPermission(String name, String target)
    {
        this(name, target, null);
    }

    public String getName()
    {
        return name;
    }

    public String getTarget()
    {
        return target;
    }

    public String getActionList()
    {
        return actionList;
    }

    public String toString()
    {
        StringBuffer sb = new StringBuffer();
        sb.append("\nPermission name:" + name);
        sb.append(" Permission target:" + target);
        sb.append(" Permission action list:" + actionList);
        return sb.toString();
    }

    public boolean equals(Object o)
    {
        if (this == o)
        {
            return true;
        }
        if (!(o instanceof MIDPPermission))
        {
            return false;
        }
        MIDPPermission p = (MIDPPermission)o;
        if ((this.name == null && p.getName() != null)
                || (this.name != null && !this.name.equals(p.getName()))
                || (this.target == null && p.getTarget() != null && p.getTarget().length() != 0)
                || (this.target != null && this.target.length() != 0 && !this.target.equals(p.getTarget()))
                || (this.actionList == null && p.getActionList() != null && p.getActionList().length() != 0)
                || (this.actionList != null && this.actionList.length() != 0
                    && !this.actionList.equals(p.getActionList())))
        {
            return false;
        }
        return true;
    }

    protected MIDPPermission()
    {
    }
}
