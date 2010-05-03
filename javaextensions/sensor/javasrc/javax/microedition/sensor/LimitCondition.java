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
 * Please refer JSR 256 for more details.
 */

public final class LimitCondition implements Condition
{
    private final double limit;
    private final int intOp;
    private final String op;

    private static final int INT_EQUALS = 1;
    private static final int INT_GREATER_THAN = 16;
    private static final int INT_GREATER_THAN_OR_EQUALS = 32;
    private static final int INT_LESS_THAN = 2;
    private static final int INT_LESS_THAN_OR_EQUALS = 4;

    private static final int INT_ALL_EQUALS = INT_EQUALS | INT_GREATER_THAN_OR_EQUALS | INT_LESS_THAN_OR_EQUALS;
    private static final int INT_ALL_GREATER_THAN = INT_GREATER_THAN | INT_GREATER_THAN_OR_EQUALS;
    private static final int INT_ALL_LESS_THAN = INT_LESS_THAN | INT_LESS_THAN_OR_EQUALS;



    /**
     * Please refer JSR 256 for more details.
     */
    public LimitCondition(double limit, String operator)
    {
        if (operator == null) throw new NullPointerException();
        intOp = getIntOp(operator);
        if (intOp == 0) throw new IllegalArgumentException();

        this.limit = limit;
        this.op = operator;

    }

    /**
     * Please refer JSR 256 for more details.
     */
    public final double getLimit()
    {
        return limit;
    }




    /**
     * Please refer JSR 256 for more details.
     */
    public final String getOperator()
    {
        return op;
    }



    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(double value)
    {

        if ((intOp & INT_ALL_EQUALS) > 0)
        {
            if (Double.doubleToLongBits(limit) == Double.doubleToLongBits(value))
            {
                return true;
            }
        }

        if ((intOp & INT_ALL_GREATER_THAN) > 0)
        {
            if (value>limit)
            {
                return true;
            }
        }

        if ((intOp & INT_ALL_LESS_THAN) > 0)
            if (value<limit)
            {
                return true;
            }
        return false;
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(Object value)
    {
        return false;
    }



    private int getIntOp(String op)
    {
        if (op.equals(OP_EQUALS)) return INT_EQUALS;
        if (op.equals(OP_LESS_THAN)) return INT_LESS_THAN;
        if (op.equals(OP_LESS_THAN_OR_EQUALS)) return INT_LESS_THAN_OR_EQUALS;
        if (op.equals(OP_GREATER_THAN)) return INT_GREATER_THAN;
        if (op.equals(OP_GREATER_THAN_OR_EQUALS)) return INT_GREATER_THAN_OR_EQUALS;
        return 0;
    }

}


