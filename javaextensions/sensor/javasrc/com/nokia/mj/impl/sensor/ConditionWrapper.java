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
* Description:  Condition wrapper
*
*/

package com.nokia.mj.impl.sensor;

import javax.microedition.sensor.Condition;

/**
 * Wrapper class for Condition to hold additional information
 *
 * Note: when comparing to Condition, remember to either unwrap the wrapper or
 * wrap the plain Condition. ConditionWrapper.equals() uses the equals() of the
 * wrapped Condition and can take either plain or wrapped Condition as parameter.
 * But Condition.equals() doesn't understand ConditionWrapper.
 * This has some pitfalls, i.e.
 *
 * INSTEAD OF
 * wrappedConditionsVector.contains( plainCondition )
 *
 * USE
 * wrappedConditionsVector.contains( new ConditionWrapper( plainCondition ) )
 */
public class ConditionWrapper implements Condition
{
    private Condition iCondition = null;
    private int iNativeId = -1;

    /**
     * Constructor
     * @param aCondition condition instance to be wrapped
     */
    public ConditionWrapper(Condition aCondition)
    {
        super();
        iCondition = aCondition;
    }

    /**
     * Returns the native Id of the condition
     * @return native id or -1 if condition is not on native side
     */
    protected int getNativeId()
    {
        return iNativeId;
    }

    /**
     * Sets the native ID
     * @param aId
     */
    protected void setNativeId(int aId)
    {
        iNativeId = aId;
    }

    /**
     * Unwraps the Condition from wrapper
     * @return Condition object contained in wrapper
     */
    protected Condition unwrap()
    {
        return iCondition;
    }
    /*
     * (non-Javadoc)
     * @see javax.microedition.sensor.Condition#isMet(double)
     */
    public boolean isMet(double doubleValue)
    {
        return iCondition.isMet(doubleValue);
    }

    /*
     * (non-Javadoc)
     * @see javax.microedition.sensor.Condition#isMet(java.lang.Object)
     */
    public boolean isMet(Object value)
    {
        return iCondition.isMet(value);
    }

    /*
     * (non-Javadoc)
     * @see java.lang.Object#equals(java.lang.Object)
     */
    public boolean equals(Object aObj)
    {
        if (aObj instanceof ConditionWrapper)
        {
            ConditionWrapper other = (ConditionWrapper) aObj;
            return iCondition.equals(other.unwrap());
        }
        if (aObj instanceof Condition)
        {
            return iCondition.equals(aObj);
        }
        return false;
    }
}
