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
* Description:  Abstract RepeatRule invoker to be implemented in the
 *                javax.microedition.pim package.
 *
*/


// PACKAGE
package com.nokia.mj.impl.pim;

// IMPORTS
import javax.microedition.pim.RepeatRule;

// CLASS DEFINITION
/**
 * Class which enables accessing javax.microedition.pim.RepeatRule package
 * private operations from com.nokia.microedition.pim package. The class is
 * abstract and it has a static instance member. The class defines operations
 * that can be accessed through it using the instance member. The class is
 * implemented in the javax.microedition.pim package and the static instance
 * member is set there, so the operations are delegated to the implementations
 * in that package.
 *
 * This is necessary because all of the PIM API implementation resides in the
 * latter package, but one concrete class, the RepeatRule, resides in the former
 * package.
 *
 * The class must be implemented by a class in the javax.microedition.pim
 * package.
 */
public abstract class RepeatRuleInvoker
{
    /** Instance of RepeatRuleInvoker. */
    private static RepeatRuleInvoker sInstance = null;

    /**
     * Sets the current instance.
     */
    static final protected void setInstance(RepeatRuleInvoker aInstance)
    {
        sInstance = aInstance;
    }

    /**
     * Gets the native side handle of \a aRepeatRule.
     */
    static final int getHandle(RepeatRule aRepeatRule)
    {
        return sInstance.doGetHandle(aRepeatRule);
    }

    /**
     * Disposes the old native side peer of \a aRepeatRule and sets the handle
     * to \a aRepeatRuleHandle.
     */
    static final void setNativePeer(RepeatRule aRepeatRule,
                                    int aRepeatRuleHandle)
    {
        sInstance.doSetNativePeer(aRepeatRule, aRepeatRuleHandle);
    }

    /**
     * Creates new Repeat Rule with given native side peer object.
     */
    static final RepeatRule createRepeatRule(

        int aRepeatRulehandle)
    {
        return sInstance.doCreateRepeatRule(

                   aRepeatRulehandle);
    }

    /**
     * Implementation of the getHandle() method.
     */
    protected abstract int doGetHandle(RepeatRule aRepeatRule);

    /**
     * Implementation of the setNativePeer() method.
     */
    protected abstract void doSetNativePeer(RepeatRule aRepeatRule,
                                            int aRepeatRuleHandle);

    /**
     * Implementation of createRepeatRule() method.
     */
    protected abstract RepeatRule doCreateRepeatRule(

        int aRepeatRuleHandle);

    /**
     * Direct construction prohibited.
     */
    protected RepeatRuleInvoker()
    {
    }
}
