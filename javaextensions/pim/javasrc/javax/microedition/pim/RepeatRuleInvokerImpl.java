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
* Description:  RepeatRule invoker for com.nokia.microedition.pim package.
 *
*/


// PACKAGE
package javax.microedition.pim;

// IMPORTS
import com.nokia.mj.impl.pim.RepeatRuleInvoker;

// CLASS DEFINITION
/**
 * Implements the RepeatRuleInvoker in the javax.microedition.pim package,
 * providing a way to access the package private methods of RepeatRule from the
 * com.nokia.mj.impl.pim package. Not part of the PIM API interface and not
 * publicly accessible.
 *
 * \b Note that this is strictly package private class.
 */
class RepeatRuleInvokerImpl extends RepeatRuleInvoker
{
    /**
     * Sets the RepeatRuleInvoker. Initialization is tied to the creation of a
     * PIM object. Creation of the PIM singleton instance is forced in the
     * construction of a RepeatRule.
     */
    static void initialize()
    {
        setInstance(new RepeatRuleInvokerImpl());
    }

    /**
     * Direct construction is prohibited.
     */
    protected RepeatRuleInvokerImpl()
    {
    }

    protected int doGetHandle(RepeatRule aRepeatRule)
    {
        return aRepeatRule.nativeHandle();
    }

    protected void doSetNativePeer(RepeatRule aRepeatRule, int aRepeatRuleHandle)
    {
        aRepeatRule.setNativePeer(aRepeatRuleHandle);
    }

    protected RepeatRule doCreateRepeatRule(

        int aRepeatRulehandle)
    {
        return new RepeatRule(aRepeatRulehandle);
    }
}