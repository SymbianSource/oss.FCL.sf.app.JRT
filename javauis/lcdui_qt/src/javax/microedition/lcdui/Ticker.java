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

import java.util.*;

import org.eclipse.swt.internal.extension.StringUtil;
import org.eclipse.swt.widgets.Label;

/**
 * Implementation of LCDUI <code>Ticker</code> class.
 */
public class Ticker
{

    /**
     * How many pixels the ticker will move after every delay.
     */
    private float stepSize = 1;

    private int screenWidth;
    private float tickerX = Integer.MIN_VALUE;
    private String text;
    private boolean isRunning;

    /**
     * If true, text is scrolling from left to right. Otherwise direction
     * is from right to left.
     */
    private boolean leftToRight;

    /**
     * eSWT labels associated with this Ticker. It is displayable's
     * responsibility to add Label to the ticker.
     */
    private Vector labels = new Vector();

    /**
     * Timer which runs the tickers. There's only one timer for all
     * ticker instances but every ticker has its own TimerTask.
     */
    private static Timer timer;
    private TimerTask timerTask;

    /**
     * Constructor.
     *
     * @param txt Displayed string. If null, throws NullPointerException.
     */
    public Ticker(String txt)
    {
        setString(txt);
    }

    /**
     * Set the displayed string.
     *
     * @param txt Displayed string. If null, throws NullPointerException.
     */
    public void setString(String txt)
    {
        if(txt == null)
        {
            throw new NullPointerException(
                MsgRepository.TICKER_EXCEPTION_NULL_STRING);
        }
        text = txt;
        updateDirection();
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                for(int i = 0; i < labels.size(); i++)
                {
                    Label label = ((Label) labels.elementAt(i));
                    if(!label.isDisposed())
                    {
                        label.setText(getFormattedString());
                        label.pack();
                    }
                }
            }
        });
    }

    /**
     * Get the displayed string.
     *
     * @return the displayed string.
     */
    public String getString()
    {
        return text;
    }

    /**
     * Gets the displayed string where new line characters are replaced
     * with spaces. When setting text to eSWT Label one should use this
     * method instead of getString().
     *
     * @return String where newline characters are replaced with spaces.
     */
    String getFormattedString()
    {
        StringBuffer formattedText = new StringBuffer(text.length());
        for(int i = 0; i < text.length(); i++)
        {
            if(text.charAt(i) == '\r')
            {
                continue;
            }
            else if((text.charAt(i) == '\n')
                    || (text.charAt(i) == '\u2028'))
            {
                formattedText.append(" ");
            }
            else
            {
                formattedText.append(text.charAt(i));
            }
        }
        return formattedText.toString();
    }

    /**
     * Adds new eSWT Label to this ticker. That's the label which is then
     * updated when ticker is running. Note that there might be more than
     * one label in one Ticker, because same Ticker may exist in many
     * Displayables.
     *
     * @param label Label to add.
     */
    void addLabel(final Label label)
    {
        if(label != null)
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    labels.addElement(label);
                }
            });
        }
    }

    /**
     * Removes eSWT Label from this ticker. If ticker is removed from
     * displayable, displayable must call this.
     *
     * @param label Label to be removed. If null, nothing happens.
     */
    void removeLabel(final Label label)
    {
        if(label != null)
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    labels.removeElement(label);
                }
            });
        }
    }

    /**
     * Updates the speed of the Ticker depending of the Displayable's width.
     * This should be called when the width of the displayable changes.
     *
     * Ticker speed is adjusted so that it takes about 7 seconds
     * for every character to run across the screen.
     */
    void updateSpeed()
    {
        if(labels.size() > 0)
        {
            ESWTUIThreadRunner.syncExec(new Runnable()
            {
                public void run()
                {
                    screenWidth = ((Label) labels.elementAt(0)).getParent()
                                  .getBounds().width;
                }
            });
            stepSize = (screenWidth * Config.TICKER_MOVEMENT_DELAY) / Config.TICKER_DISPLAY_TIME;
        }
    }

    /**
     * Start to run a ticker. If same ticker is added to more than one
     * displayable and then current displayable is changed, the ticker
     * should continue running from same position as it was in previous
     * displayable. That's why when once started, there's no way to stop
     * the ticker (except by removing it from all displayables).
     *
     * Note that there must be at least one label in this ticker when started.
     *
     * It is ok to call this method even if Ticker is already running.
     * In that case the method will just return.
     */
    void start()
    {
        if(isRunning)
        {
            // Ticker already running.
            return;
        }
        isRunning = true;
        updateDirection();
        updateSpeed();



        if(timer == null)
        {
            timer = new Timer();
        }

        timerTask = new TimerTask()
        {
            public void run()
            {
                if(isRunning)
                {
                    updateLocation();
                }
                else
                {
                    timerTask.cancel();
                }
            }
        };

        timer.schedule(timerTask, 0, Config.TICKER_MOVEMENT_DELAY);
    }

    private void updateDirection()
    {
        leftToRight = StringUtil.isRightToLeftText(text);
        if(leftToRight)
        {
            tickerX = Integer.MAX_VALUE;
        }
        else
        {
            tickerX = Integer.MIN_VALUE;
        }
    }

    /**
     * Updates ticker location.
     */
    private void updateLocation()
    {
        ESWTUIThreadRunner.syncExec(new Runnable()
        {
            public void run()
            {
                if(labels.size() <= 0)
                {
                    // Ticker is removed from all displayables
                    // so it is ok to stop it.
                    isRunning = false;
                    return;
                }

                for(int i = 0; i < labels.size(); i++)
                {
                    Label label = ((Label) labels.elementAt(i));
                    if(!label.isDisposed())
                    {
                        label.setLocation((int) tickerX, 0);
                    }
                }

                if(((Label) labels.elementAt(0)).isDisposed())
                {
                    // Label is disposed. This may happen only
                    // when MIDlet is closing and in that case
                    // this method would throw exception without
                    // this check.
                    isRunning = false;
                    return;
                }

                int labelWidth = ((Label) labels.elementAt(0)).
                                 getBounds().width;

                if(leftToRight)
                {
                    // Scrolling from left to right:
                    if(tickerX > screenWidth)
                    {
                        tickerX = -labelWidth;
                    }
                    else
                    {
                        tickerX += stepSize;
                    }
                }
                else
                {
                    // Scrolling from right to left:
                    if(tickerX < -labelWidth)
                    {
                        tickerX = screenWidth;
                    }
                    else
                    {
                        tickerX -= stepSize;
                    }
                }
            }
        });
    }
}
