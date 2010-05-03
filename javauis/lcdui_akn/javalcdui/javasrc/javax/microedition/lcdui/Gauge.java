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


package javax.microedition.lcdui;

import com.nokia.mj.impl.rt.legacy.NativeError;

public class Gauge extends Item
{
    public static final int INDEFINITE = -1;
    //
    public static final int CONTINUOUS_IDLE = 0;        // == ~INDEFINITE
    public static final int INCREMENTAL_IDLE = 1;
    public static final int CONTINUOUS_RUNNING = 2;
    public static final int INCREMENTAL_UPDATING = 3;

    //
    // A strictly positive integer or the special value INDEFINITE.
    //
    private int iMaxValue;

    //
    // If iMaxValue is a positive integer, then iValue ranges over the
    // non-negative integers (including zero).
    //
    // If iMaxValue is the special value INDEFINITE, then iValue takes
    // one value from the set (CONINTUOUS_IDLE, INCREMENTAL_IDLE,
    // CONTINUOUS_RUNNING, INCREMENTAL_UPDATING).
    //
    private int iValue;

    //
    // Indicates that the user can change iValue.
    //
    private boolean iInteractive;

    public Gauge(String aLabel, boolean aInteractive, int aMaxValue, int aValue)
    {
        super();
        synchronized (iToolkit)
        {
            aMaxValue = ValidateMaxValue(aMaxValue, aInteractive);
            aValue    = ValidateValue(aValue, aMaxValue);

            final int    toolkit = iToolkit.getHandle();
            final String label   = aLabel == null?"":aLabel;

            iHandle = Toolkit.checkHandle(_create(toolkit, this, label, aInteractive, aMaxValue, aValue));
            iMaxValue = aMaxValue;
            iValue = aValue;
            iLabel = aLabel;
            iInteractive = aInteractive;
        }
    }

    public void setValue(int aValue)
    {
        synchronized (iToolkit)
        {
            aValue = ValidateValue(aValue, iMaxValue);
            NativeError.check(_setValue(iToolkit.getHandle(), getHandle(), aValue));
            iValue = aValue;
        }
    }

    public int getValue()
    {
        synchronized (iToolkit)
        {
            if (iInteractive)
            {
                //
                // User may have modified value, must check peer and
                // update iValue to match.
                //
                iValue = _getValue(iToolkit.getHandle(), getHandle());
            }
            else
            {
                // No point in going native since the user cannot have
                // modified the value.
                // Thus, nothing to do.
            }

            return iValue;
        }
    }

    public void setMaxValue(int aMaxValue)
    {
        synchronized (iToolkit)
        {
            if (iMaxValue == aMaxValue)
            {
                return;
            }
            int maxValue = ValidateMaxValue(aMaxValue, iInteractive);
            int value    = ValidateValue(getValue(), maxValue, iMaxValue);
            NativeError.check(_setMaxValue(iToolkit.getHandle(), getHandle(), maxValue, value));
            iMaxValue = maxValue;
            iValue = value;
        }
    }

    public int getMaxValue()
    {
        return iMaxValue;
    }

    public boolean isInteractive()
    {
        return iInteractive;
    }

    private boolean IsIndefinite()
    {
        final boolean indefinite = (iMaxValue == INDEFINITE);
        return indefinite;
    }

    private static int ValidateMaxValue(int aMaxValue, boolean aInteractive)
    {
        //
        // Non-interactive gauges may have the special max value of
        // INDEFINITE
        //
        if (!aInteractive && (aMaxValue == INDEFINITE))
        {
            return aMaxValue;
        }

        //
        // All remaining cases max value must be a positive integer.
        //
        if (aMaxValue <= 0)
        {
            throw new IllegalArgumentException("invalid max value");
        }

        return aMaxValue;
    }

    private static int ValidateRange(int aValue, int aMaxValue)
    {
        aValue = Math.min(aValue, aMaxValue);   // clamp to max
        aValue = Math.max(aValue, 0);           // clamp to min (also maps INDEFINITE to CONTINUOUS_IDLE)
        return aValue;
    }

    private static int ValidateValue(int aValue, int aNewRange)
    {
        if (aNewRange == INDEFINITE)
        {
            switch (aValue)
            {
            case CONTINUOUS_IDLE:
            case INCREMENTAL_IDLE:
            case CONTINUOUS_RUNNING:
            case INCREMENTAL_UPDATING:
                break;
            default:
                throw new IllegalArgumentException();
            }
            return aValue;
        }
        else
        {
            return ValidateRange(aValue, aNewRange);
        }
    }

    private static int ValidateValue(int aValue, int aNewRange, int aOldRange)
    {
        if (aNewRange == INDEFINITE)
        {
            return CONTINUOUS_IDLE;
        }
        else if (aOldRange == INDEFINITE)
        {
            return 0;
        }
        else
        {
            return ValidateRange(aValue, aNewRange);
        }
    }

    boolean isAlertGauge()
    {
        boolean alertGauge=false;
        synchronized (iToolkit)
        {
            alertGauge = (!iInteractive) && (null==iListener) && (null==iLabel) && (null == iScreen)
                         && (Item.LAYOUT_DEFAULT==iLayout)&&(!iSizeLocked)&&(iCommands.count()==0);
        }
        return alertGauge;
    }

    private native int _create(int aToolkit,Gauge aGauge,String aLabel,boolean aInteractive,int aMaxValue,int aInitialValue);
    private native int _setValue(int aToolkit,int aItem,int aValue);
    private native int _getValue(int aToolkit,int aItem);
    private native int _setMaxValue(int aToolkit,int aItem,int aMaxValue, int aValue);
}
