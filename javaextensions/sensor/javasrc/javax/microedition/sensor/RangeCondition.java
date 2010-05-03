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

public final class RangeCondition implements Condition
{


    private final double lowerLimit, upperLimit;
    private final String lowerOp, upperOp;
    private final IsMetStrategy strategy;

    /**
     * Please refer JSR 256 for more details.
     */
    public RangeCondition(double lowerLimit, String lowerOp, double upperLimit, String upperOp)
    {
        if (upperLimit < lowerLimit)
            throw new IllegalArgumentException(upperLimit + "<" + lowerLimit);
        if (lowerLimit == upperLimit && !(lowerOp == Condition.OP_GREATER_THAN_OR_EQUALS &&
                                          upperOp == Condition.OP_LESS_THAN_OR_EQUALS))
            throw new IllegalArgumentException(lowerOp + " == " + upperOp);
        if (!(lowerOp == Condition.OP_GREATER_THAN | lowerOp == Condition.OP_GREATER_THAN_OR_EQUALS))
            throw new IllegalArgumentException("lowerOp is invalid:"+lowerOp);
        if (!(upperOp == Condition.OP_LESS_THAN | upperOp == Condition.OP_LESS_THAN_OR_EQUALS))
            throw new IllegalArgumentException("upperOp is invalid:"+upperOp);
        this.lowerLimit = lowerLimit;
        this.upperLimit = upperLimit;
        this.upperOp = upperOp;
        this.lowerOp = lowerOp;
        strategy = IsMetStrategy.getStrategy(lowerOp, upperOp);
    }



    /**
     * Please refer JSR 256 for more details.
     */
    public final double getLowerLimit()
    {
        return lowerLimit;
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public final String getLowerOp()
    {
        return lowerOp;
    }


    /**
     * Please refer JSR 256 for more details.
     */
    public final double getUpperLimit()
    {
        return upperLimit;
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public final String getUpperOp()
    {
        return upperOp;
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(double doubleValue)
    {
        return strategy.isMet(lowerLimit, doubleValue, upperLimit);
    }

    /**
     * Please refer JSR 256 for more details.
     */
    public boolean isMet(Object value)
    {
        return false;
    }



    private static abstract class IsMetStrategy
    {

        static IsMetStrategy getStrategy(String lowerOp, String upperOp)
        {
            if (lowerOp == Condition.OP_GREATER_THAN &&
                    upperOp == Condition.OP_LESS_THAN)
                return new OpenRangeStrategy();
            if (lowerOp == Condition.OP_GREATER_THAN_OR_EQUALS &&
                    upperOp == Condition.OP_LESS_THAN_OR_EQUALS)
                return new ClosedRangeStrategy();
            if (lowerOp == Condition.OP_GREATER_THAN &&
                    upperOp == Condition.OP_LESS_THAN_OR_EQUALS)
                return new HalfClosedRangeStrategy();
            if (lowerOp == Condition.OP_GREATER_THAN_OR_EQUALS &&
                    upperOp == Condition.OP_LESS_THAN)
                return new HalfOpenRangeStrategy();
            return null;
        }

        abstract boolean isMet(double lowerLimit, double value, double upperLimit);
    }

    private static class ClosedRangeStrategy extends IsMetStrategy
    {
        public boolean isMet(double lowerLimit, double value, double upperLimit)
        {
            return (lowerLimit<=value) && (value<=upperLimit);
        }
    }
    private static class OpenRangeStrategy extends IsMetStrategy
    {
        public boolean isMet(double lowerLimit, double value, double upperLimit)
        {
            return (lowerLimit<value) && (value<upperLimit);
        }
    }
    private static class HalfOpenRangeStrategy extends IsMetStrategy
    {
        public boolean isMet(double lowerLimit, double value, double upperLimit)
        {
            return (lowerLimit<=value) && (value<upperLimit);
        }
    }
    private static class HalfClosedRangeStrategy extends IsMetStrategy
    {
        public boolean isMet(double lowerLimit, double value, double upperLimit)
        {
            return (lowerLimit<value) && (value<=upperLimit);
        }
    }

}


