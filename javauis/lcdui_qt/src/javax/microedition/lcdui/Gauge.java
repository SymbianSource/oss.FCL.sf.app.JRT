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

    /**
     * If Gauge is changed, reasons for Re-layouting.
     */
	static final int UPDATE_MAXVALUE = UPDATE_ITEM_MAX << 1;
	static final int UPDATE_VALUE = UPDATE_ITEM_MAX << 2;

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
    public Gauge(String aName, boolean aInteractive, int aMaxVal, int aInitVal)
    {
        maxValue = validateMaxValue(aMaxVal, aInteractive);
		value = validateValue(aInitVal, maxValue);
		interactive = aInteractive;
        setLabel(aName);
    }

    /**
     * Sets value of this gauge.
     *
     * @param value New value. Must be between zero and maxvalue inclusive.
     */
    public void setValue(int aValue)
    {
        // TODO: eSWT support required
        /*
         * if Gauge state is incremental-updating and it is non-interactive and
         * indefinite, we should update Gauge when this method is called, but
         * that requires eSWT extension.
         */
        value = validateValue(aValue, maxValue);
        updateParent(UPDATE_VALUE);
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
    public void setMaxValue(int aMaxValue)
    {
        int oldMaxValue = maxValue;
		maxValue = validateMaxValue(aMaxValue, interactive);
        value = validateValue(getValue(), maxValue, oldMaxValue);
        updateParent(UPDATE_MAXVALUE);
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
     * Check maximum value validity.
     *
     * @param aMaxVal the maximum value.
     * @param aInteractive is interactive.
     * @return validated maximum value.
     */
    private int validateMaxValue(int aMaxVal, boolean aInteractive)
    {
        if((!aInteractive) && (aMaxVal == INDEFINITE))
        {
			return aMaxVal;
        }

        if(aMaxVal <= 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.GAUGE_EXCEPTION_INVALID_MAXVALUE);
        }

        return aMaxVal;
    }

    /**
     * Check value validity.
     *
     * @param aValue the value.
     * @param aMaxVal the maximum value.
     * @return validated value.
     */
	private int validateValue(int aValue, int aMaxVal)
	{
		if (aMaxVal == INDEFINITE)
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
			return clampValue(aValue, aMaxVal);
		}
	}

    /**
     * Check value validity.
     *
     * @param aValue the value.
     * @param aNewMaxVal the new maximum value.
     * @param aOlddMaxVal the old maximum value.
     * @return validated value.
     */
	private int validateValue(int aValue, int aNewMaxVal, int aOlddMaxVal)
	{
		if (aNewMaxVal == INDEFINITE)
		{
			return CONTINUOUS_IDLE;
		}
		else if (aOlddMaxVal == INDEFINITE)
		{
			return 0;
		}
		else
		{
			return clampValue(aValue, aNewMaxVal);
		}
	}

	  /**
	 * Validates the value against the range.
	 *
	 * @param aValue the value.
	 * @param aMaxVal the maximum value.
	 * @return validated value.
	 */
    private static int clampValue(int aValue, int aMaxVal)
    {
        aValue = Math.min(aValue, aMaxVal);
        aValue = Math.max(aValue, 0);
        return aValue;
    }

    /**
     * @return if the Gauge is indefinite.
     */
    boolean isIndefinite()
    {
        return (maxValue == INDEFINITE);
    }

    /* (non-Javadoc)
     * @see javax.microedition.lcdui.Item#isFocusable()
     */
    boolean isFocusable()
    {
        return (isInteractive() || (getNumCommands() > 0));
    }

    /**
     * Returns if this indicator meets the restrictions for its use in an Alert.
     */
    boolean isSuitableForAlert()
    {
        return (!isInteractive()
                && getParent() == null
                && getLabel() == null
                && getLayout() == Item.LAYOUT_DEFAULT
                && !isSizeLocked()
                && getNumCommands() == 0
                && getItemCommandListener() == null);
    }

    /**
     * Called by widget listeners to update Item value.
     */
    void internalSetValue(int newValue)
    {
        value = validateValue(newValue, maxValue);
		updateParent(UPDATE_VALUE);
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

}

