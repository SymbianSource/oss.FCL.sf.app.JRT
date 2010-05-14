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

import javax.microedition.lcdui.EventDispatcher.LCDUIEvent;
import javax.microedition.midlet.MIDlet;
import org.eclipse.ercp.swt.mobile.MobileDevice;
import org.eclipse.ercp.swt.mobile.Screen;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.internal.extension.DisplayExtension;
import org.eclipse.swt.widgets.Event;

/**
 * Display - represents device's display.
 */
public class Display
{

    /**
     * Type of image for List element.
     *
     * @value for LIST_ELEMENT is 1.
     */
    public static final int LIST_ELEMENT = 1;

    /**
     * Type of image for ChoiceGroup element.
     *
     * @value for CHOICE_GROUP_ELEMENT is 2.
     */
    public static final int CHOICE_GROUP_ELEMENT = 2;

    /**
     * Type of image for Alert.
     *
     * @value for ALERT is 3.
     */
    public static final int ALERT = 3;

    /**
     * A color constant for usage in getColor(). COLOR_BACKGROUND defines
     * background color of the screen. It is always in contrast with
     * COLOR_FOREGROUND.
     *
     * @value for COLOR_BACKGROUND is 0.
     */
    public static final int COLOR_BACKGROUND = 0;

    /**
     * A color constant for usage in getColor(). COLOR_BACKGROUND defines
     * foreground color of the screen. It is always in contrast with
     * COLOR_BACKGROUND.
     *
     * @value for COLOR_FOREGROUND is 1.
     */
    public static final int COLOR_FOREGROUND = 1;

    /**
     * A color constant for usage in getColor(). COLOR_HIGHLIGHTED_BACKGROUND
     * defines background color for focused or highlighted items. It is always
     * in contrast with COLOR_HIGHLIGHTED_FOREGROUND.
     *
     * @value for COLOR_HIGHLIGHTED_BACKGROUND is 2.
     */
    public static final int COLOR_HIGHLIGHTED_BACKGROUND = 2;

    /**
     * A color constant for usage in getColor(). COLOR_HIGHLIGHTED_FOREGROUND
     * defines foreground color for focused or highlighted items.
     *
     * @value for COLOR_HIGHLIGHTED_FOREGROUND is 3.
     */
    public static final int COLOR_HIGHLIGHTED_FOREGROUND = 3;

    /**
     * A color constant for usage in getColor(). COLOR_BORDER defines color of
     * border of items in Display.
     *
     * @value for COLOR_BORDER is 4.
     */
    public static final int COLOR_BORDER = 4;

    /**
     * A color constant for usage in getColor(). COLOR_HIGHLIGHTED_BORDER
     * defines color of border of highlighted or focused items in Display.
     *
     * @value for COLOR_HIGHLIGHTED_BORDER is 5.
     */
    public static final int COLOR_HIGHLIGHTED_BORDER = 5;

    private static Display instance = new Display();
    // private static Form emptyForm;

    private Displayable currentDisplayable;
    private Displayable getcurrent;
    private int numColors;
    private RGB eswtTempRgb;

    /**
     * Constructor.
     */
    private Display()
    {
    }

    /**
     * Returns the Display unique for specific midlet.
     *
     * @param midlet - MIDlet
     * @return display - unique display used by the midlet. note: current
     *         implementation supports 1 Display.
     */
    public static Display getDisplay(MIDlet midlet)
    {
        if(midlet == null)
        {
            throw new NullPointerException(
                MsgRepository.DISPLAY_EXCEPTION_MIDLET_IS_NULL);
        }
        // following code was removed to improve performance
        /*if (emptyForm == null) {
            emptyForm = new Form(midlet.getAppProperty("MIDlet-Name"));
            ESWTUIThreadRunner.syncExec(new Runnable() {
                public void run() {
                    emptyForm.eswtHandleShowCurrentEvent();
                    emptyForm.eswtHandleHideCurrentEvent();
                    emptyForm.getContentComp().getVerticalBar().setVisible(
                            false);
                }
            });
        }*/
        return instance;
    }

    /**
     * Returns the Display instance.
     */
    static Display getDisplay()
    {
        return instance;
    }

    /**
     * Propagate event to current displayable.
     */
    void eswtHandleEvent(Event e)
    {
        if(currentDisplayable != null)
        {
            currentDisplayable.eswtHandleEvent(e);
        }
    }

    /**
     * Detect support for colors.
     *
     * @return true if Display supports colors. false if black and white only
     *         supported.
     */
    public boolean isColor()
    {
        return true;
    }

    /**
     * Get number of colors supported by device.
     *
     * @return number of Colors supported by device.
     */
    public int numColors()
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                numColors = eswtNumColors();
            }
        });
        return numColors;
    }

    /**
     * eSWT specific call to implement Display.numColors() functionality.
     *
     * @return numColors supported by device.
     */
    public int eswtNumColors()
    {

        final int MAX_SHIFT = 30;

        Screen[] screens = MobileDevice.getMobileDevice().getScreens();
        Screen activeScreen = null;
        int depth = 0;
        if(screens != null)
        {
            for(int i = 0; i < screens.length; i++)
            {
                if(screens[i].isActive())
                {
                    activeScreen = screens[i];
                }
            }
            depth = activeScreen.getColorDepth();
            if(depth > MAX_SHIFT)
            {
                numColors = Integer.MAX_VALUE;
            }
            else
            {
                numColors = (1 << depth);
            }
        }
        else
        {
            numColors = 0;
        }

        numColors = 2 << 16;
        return numColors;
    }

    /**
     * Returns current Displayable for Display.
     *
     * @return current Displayable
     */
    public Displayable getCurrent()
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                getcurrent = eswtGetCurrent();
            }
        });
        return getcurrent;
    }

    /**
     * eSWT specific call to implement Display.getCurrent() functionality
     *
     * @return current Displayable object which is active for Display
     */
    Displayable eswtGetCurrent()
    {
        if(ESWTUIThreadRunner.getInstance().getDisplay().getActiveShell()
                != null)
        {
            return currentDisplayable;
        }
        return null;
    }

    /**
     * Set the currentDisplayable to be active in the midlet.
     *
     * @param displayable new Displayable to be set active in midlet
     */
    public void setCurrent(Displayable displayable)
    {
        if(displayable != null)
        {
            if(displayable instanceof Alert)
            {
                Alert alert = (Alert) displayable;
                if(currentDisplayable == null)
                {
                    // if Alert is the first Displayable
                    alert.setNextDisplayable(null);
                    doSetCurrent(alert);
                }
                else
                {
                    setCurrent(alert, currentDisplayable);
                }
            }
            else
            {
                doSetCurrent(displayable);
            }
        }
        else
        {
            /*
             * If parameter is null, nothing is required to happen, optionally
             * bringing Midlet to background/foreground.
             */

            if(currentDisplayable != null && currentDisplayable instanceof Alert)
            {
                // Restarts Alert timer if any
                Alert alert = (Alert) currentDisplayable;
                alert.resetTimerTask(true);
            }
        }
    }

    /**
     * Set an Alarm to be active in the midlet.
     *
     * @param alert Alert to be set active in midlet
     * @param nextDisplayable next Displayable to be set active after Alert
     *
     * @throws NullPointerException if aAlert or aNextDisplayable is null
     * @throws IllegalArgumentException if aNextDisplayable is an Alert
     */
    public void setCurrent(Alert alert, Displayable nextDisplayable)
    {
        if(alert == null)
        {
            throw new NullPointerException(
                MsgRepository.DISPLAY_EXCEPTION_ALERT_IS_NULL);
        }
        else if(nextDisplayable == null)
        {
            throw new NullPointerException(
                MsgRepository.DISPLAY_EXCEPTION_NEXTDISP_IS_NULL);
        }
        else if(nextDisplayable instanceof Alert)
        {
            throw new IllegalArgumentException(
                MsgRepository.DISPLAY_EXCEPTION_NEXTDISP_IS_ALERT);
        }
        alert.setNextDisplayable(nextDisplayable);
        doSetCurrent(alert);
    }

    /**
     * Sets Displayable which contains specified item to be current and scrolls
     * Displayable so that the item is visible. In addition of that, if item is
     * focusable the focus is set to the item.
     *
     * @param item Item to be displayed.
     * @throws IllegalStateException If Item is not owned by a container.
     * @throws IllegalStateException If The Item is owned by an Alert.
     * @throws NullPointerException If the Item is null.
     */
    public void setCurrentItem(Item item)
    {
        if(item == null)
        {
            throw new NullPointerException(
                MsgRepository.DISPLAY_EXCEPTION_ITEM_IS_NULL);
        }
        if(item.isContainedInAlert())
        {
            throw new IllegalStateException(
                MsgRepository.ITEM_EXCEPTION_OWNED_BY_ALERT);
        }
        if(item.getParent() == null)
        {
            throw new IllegalStateException(
                MsgRepository.DISPLAY_EXCEPTION_ITEM_HAS_NO_PARENT);
        }
        Logger.method(this, "setCurrentItem", item);
        ((Form) item.getParent()).setCurrentItem(item);
        setCurrent((item.getParent()));
    }

    /**
     * Changes the current Displayable
     *
     * @param displayable the Displayable to be set active
     */
    private void doSetCurrent(final Displayable displayable)
    {
        if(currentDisplayable != displayable)
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    eswtDoSetCurrent(displayable);
                }
            });
        }
    }

    /**
     * eSWT callback for doSetCurrent
     */
    private void eswtDoSetCurrent(Displayable displayable)
    {
        Logger.info("Display :: " + currentDisplayable + " -> " + displayable);
        // hide old Displayable
        if(currentDisplayable != null)
        {
            currentDisplayable.eswtHandleHideCurrentEvent();
        }
        // change the current reference
        currentDisplayable = displayable;
        // show new Displayable
        if(currentDisplayable != null)
        {
            currentDisplayable.eswtHandleShowCurrentEvent();
        }
    }

    /**
     * Calls Run of Runnable parameter synchronized with event thread.
     *
     * @param runnable to serialize.
     */
    public void callSerially(Runnable runnable)
    {
        eswtCallSerially(runnable);
    }

    /**
     * eSWT specific calls to implement callSerially(Runnable runnable).
     *
     * @param runnable to serialize.
     */
    void eswtCallSerially(Runnable runnable)
    {
        EventDispatcher eventDispatcher = EventDispatcher.instance();
        LCDUIEvent event = eventDispatcher.newEvent(LCDUIEvent.RUNNABLE_CALLSERIALLY, this);
        event.runnable = runnable;
        eventDispatcher.postEvent(event);
    }

    /**
     * Flashes devices backlight.
     *
     * @param duration Duration in milliseconds.
     * @return false if no flashing happened because MIDlet was in background or
     *         feature is not supported.
     * @throws IllegalArgumentException if duration is negative.
     */
    public boolean flashBacklight(int duration)
    {
        if(duration < 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.DISPLAY_EXCEPTION_NEGATIVE_DURATION);
        }
       return DisplayExtension.flashLights(duration);
    }

    /**
     * Vibrates device.
     *
     * @param duration Duration in milliseconds.
     * @return false if no vibration happened because MIDlet was in background
     *         or feature is not supported.
     * @throws IllegalArgumentException if duration is negative.
     */
    public boolean vibrate(int duration)
    {
    	if(duration < 0)
        {
            throw new IllegalArgumentException(
                MsgRepository.DISPLAY_EXCEPTION_NEGATIVE_DURATION);
        }
        return DisplayExtension.startVibra(duration);
    }

    /**
     * Defines color in RGB based on specifier.
     *
     * @param specifier one of the defined color specifiers.
     * @return color in RGB. currently only 2 constants supported:
     *         COLOR_BACKGROUND and COLOR_FOREGROUND
     */
    public int getColor(int specifier)
    {
        // TODO: eSWT support required - for other color constants in LCDUI
        int constant = 0;
        switch(specifier)
        {
        case COLOR_BACKGROUND:
            constant = SWT.COLOR_WIDGET_BACKGROUND;
            break;
        case COLOR_FOREGROUND:
            constant = SWT.COLOR_WIDGET_FOREGROUND;
            break;
        case COLOR_HIGHLIGHTED_BACKGROUND:
            break;
        case COLOR_HIGHLIGHTED_FOREGROUND:
            break;
        case COLOR_BORDER:
            break;
        case COLOR_HIGHLIGHTED_BORDER:
            break;
        default:
            throw new IllegalArgumentException(
                MsgRepository.DISPLAY_EXCEPTION_INVALID_COLOR_IDENTIFIER);
        }

        final int finalConstant = constant;
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                eswtTempRgb = ESWTUIThreadRunner.getInstance().getDisplay()
                              .getSystemColor(finalConstant).getRGB();
            }
        });

        int color = eswtTempRgb.red;
        color <<= Config.DISPLAY_BITS_PER_PIXEL;
        color += eswtTempRgb.green;
        color <<= Config.DISPLAY_BITS_PER_PIXEL;
        color += eswtTempRgb.blue;
        return color;
    }

    /**
     * Returns border style of a UI component.
     *
     * @param isHighlighted true if component is highlighted
     * @return Border style of a UI component.
     */
    public int getBorderStyle(boolean isHighlighted)
    {
        return Graphics.SOLID;
    }

    /**
     * Get number of alpha levels supported.
     *
     * @return number of supported alpha levels
     */
    public int numAlphaLevels()
    {
        // Number of alpha levels is always 256, this implementation does
        // not support monochrome or 2-bit transparency.
        return 256;
    }

    /**
     * Get best image width.
     *
     * @param imgType Image type.
     * @return Best image width in pixels.
     */
    public int getBestImageWidth(int imgType)
    {
        return DisplayExtension.getBestImageWidth(imgType);
    }

    /**
     * Get best image height.
     *
     * @param imgType Image type.
     * @return Best image height in pixels.
     */
    public int getBestImageHeight(int imgType)
    {
        return DisplayExtension.getBestImageHeight(imgType);
    }

    /*
     * Dispatcher thread calls.
     */
    void doCallback(LCDUIEvent event)
    {
        switch(event.type)
        {
        case LCDUIEvent.RUNNABLE_CALLSERIALLY:
            event.runnable.run();
            break;
        }
    }
}
