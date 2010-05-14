/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

import org.eclipse.swt.graphics.Point;

/**
 * Class representing the Gauge item.
 */
public class Gauge extends Item
{

    /**
     * Indefinite constant.
     */
    public static final int INDEFINITE = -1;

    /**
     * Continuous idle constant.
     */
    public static final int CONTINUOUS_IDLE = 0;

    /**
     * Incremental idle constant.
     */
    public static final int INCREMENTAL_IDLE = 1;

    /**
     * Continuous running constant.
     */
    public static final int CONTINUOUS_RUNNING = 2;

    /**
     * Incremental updating constant.
     */
    public static final int INCREMENTAL_UPDATING = 3;

    private int maxValue;
    private int value;
    private boolean interactive;

    /**
     * Constructor.
     *
     * @param name the label.
     * @param interactive if its interactive.
     * @param maxVal the maximum value.
     * @param initVal the initial value.
     */
    public Gauge(String name, boolean interactive, int maxVal, int initVal)
    {
        setLabel(name);
        this.interactive = interactive;
        setMaxValue(maxVal);
        setValue(initVal);
    }

    /**
     * Check value validity.
     *
     * @param value the value.
     * @param maxVal the maximum value.
     * @return validated value.
     */
    private static int checkValue(int value, int maxVal)
    {
        if(maxVal == INDEFINITE)
        {
            switch(value)
            {
            case CONTINUOUS_IDLE:
            case INCREMENTAL_IDLE:
            case CONTINUOUS_RUNNING:
            case INCREMENTAL_UPDATING:
                break;
            default:
                throw new IllegalArgumentException(
                    MsgRepository.GAUGE_EXCEPTION_INVALID_VALUE);
            }
            return value;
        }
        else
        {
            // make sure the value is in [0, maxValue] range
            value = (value < 0 ? 0 : value);
            value = (value > maxVal ? maxVal : value);
            return value;
        }
    }

    /**
     * Check maximum value validity.
     *
     * @param maxVal the maximum value.
     * @param interactive is interactive.
     * @return validated maximum value.
     */
    private static int checkMaxValue(int maxVal, boolean interactive)
    {
        if(!interactive)
        {
            if(maxVal == INDEFINITE)
            {
                return maxVal;
            }
        }
        if(maxVal <= 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.GAUGE_EXCEPTION_INVALID_MAXVALUE);
        }
        return maxVal;
    }

    /**
     * Sets value of this gauge.
     *
     * @param value New value. Must be between zero and maxvalue inclusive.
     */
    public void setValue(int value)
    {
        // TODO: eSWT support required
        /*
         * if Gauge state is incremental-updating and it is non-interactive and
         * indefinite, we should update Gauge when this method is called, but
         * that requires eSWT extension.
         */
        this.value = checkValue(value, this.maxValue);
        updateParent(UPDATE_CONTENT);
    }

    /**
     * Get value.
     *
     * @return the value.
     */
    public int getValue()
    {
        return value;
    }

    /**
     * Set the maximum value.
     *
     * @param maxValue the maximum value.
     */
    public void setMaxValue(int maxValue)
    {
        this.maxValue = checkMaxValue(maxValue, interactive);
        this.value = checkValue(getValue(), this.maxValue);
        updateParent(UPDATE_CONTENT);
    }

    /**
     * Get maximum value.
     *
     * @return the maximum value.
     */
    public int getMaxValue()
    {
        return maxValue;
    }

    /**
     * Checks if gauge is interactive.
     *
     * @return true if the Gauge is interactive.
     */
    public boolean isInteractive()
    {
        return interactive;
    }

    /**
     * Updates Form or Alert.
     *
     * @see javax.microedition.lcdui.Item#updateParent(int)
     */
    void updateParent(int updateReason)
    {
        if(isContainedInAlert())
        {
            ((Alert) getParent()).updateIndicator();
        }
        else
        {
            super.updateParent(updateReason);
        }
    }

    /**
     * Calculates minimum size of this item.
     *
     * @return Minimum size.
     */
    Point calculateMinimumSize()
    {
        return GaugeLayouter.calculateMinimumBounds(this);
    }

    /**
     * Calculates preferred size of this item.
     *
     * @return Preferred size.
     */
    Point calculatePreferredSize()
    {
        return GaugeLayouter.calculatePreferredBounds(this);
    }

    /**
     * Called by widget listeners to update Item value.
     */
    void internalSetValue(int newValue)
    {
        this.value = checkValue(newValue, this.maxValue);
        // notify item state listener
        notifyStateChanged();
    }

    /**
     * Return layout with optional custom flags.
     *
     * @return layout directive
     */
    int internalGetLayout()
    {
        return super.internalGetLayout() | Item.LAYOUT_NEWLINE_BEFORE;
    }

    /**
     * @return if the Gauge is indefinite.
     */
    boolean isIndefinite()
    {
        return (maxValue == INDEFINITE);
    }

    /**
     * Returns if this indicator meets the restrictions for its use in an Alert.
     */
    boolean isSuitableForAlert()
    {
        return (!isInteractive()
                && getParent() == null
                && !hasLabel()
                && getLayout() == Item.LAYOUT_DEFAULT
                && !isSizeLocked()
                && getNumCommands() == 0
                && getItemCommandListener() == null);
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Item#isFocusable()
     */
    boolean isFocusable()
    {
        return (isInteractive() || (getNumCommands() > 0));
    }

}
