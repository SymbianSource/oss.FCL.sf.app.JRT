/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.sensor;


/**
 *  Please refer JSR 256 for more details.
 *
 */

public final class ObjectCondition implements Condition
{
    private final Object limit;

    /**
     * Please refer JSR 256 for more details.
     */
    public ObjectCondition(Object limit)
    {
        if (limit == null)
            throw new NullPointerException("The ObjectCondition constructor was called with a null limit");
        this.limit = limit;
    }


    /**
     * Please refer JSR 256 for more details.
     */
    public final Object getLimit()
    {
        return limit;
    }



    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(double value)
    {

        return false;
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(Object value)
    {
        if (value == null)
        {
            return false;
        }
        return limit.equals(value);
    }



}


