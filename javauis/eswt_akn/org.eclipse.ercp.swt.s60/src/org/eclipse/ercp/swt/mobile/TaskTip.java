/*******************************************************************************
 * Copyright (c) 2006 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ControlListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Widget;
import org.eclipse.ercp.swt.mobile.internal.OS;


/**
 *
 * This class provides feedback to the user about the state of a long-running
 * task.
 *
 * <p>
 * A TaskTip may contain text and an optional progress bar or another object to
 * illustrate current task state. Like a MessageBox, the look and feel of
 * TaskTip is platform-dependent, which means that there is no API level access
 * to the UI components inside a TaskTip. Unlike a MessageBox, the TaskTip is a
 * non-focusable window and will not change the current window's activation or
 * focus.
 * </p>
 * <p>
 * When constructed without a style constant, a TaskTip displays plain text to
 * indicate task progress.
 * </p>
 * <p>
 * A TaskTip becomes visible by calling <code>setVisible(true)</code> and
 * remains visible until the application calls
 * <code>setVisible(false)</code>. When a new TaskTip is created before
 * hiding or disposing of a prior TaskTip, the newest becomes the top-most one
 * and obscures the prior ones, if any.
 * </p>
 * <p>
 * A TaskTip can not be positioned programmatically or by the user. The position
 * is implementation-dependent.
 * </p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>SMOOTH: displays a visual indicator of what portion of the progress is
 * left to go</dd>
 * <dd>INDETERMINATE: displays a visual indicator that a long-running process
 * is progressing</dd>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 *
 * <p>
 * Note: The methods <code>setMaximum</code>, <code>setMinimum</code>,
 * and <code>setSelection</code>, are silently ignored if the style is not
 * SWT.SMOOTH.
 *
 * @see SWT#SMOOTH
 * @see SWT#INDETERMINATE
  * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class TaskTip extends Widget
{
    private Shell tasktipShell = null;
    private Label tasktipLabel = null;
    private ProgressBar tasktipBar = null;

    private int minimum = 0;
    private int maximum = 100;
    private int selection = 0;

    private Shell topShell = null;
    private ControlListener topShellListener = null;

    /**
     *
     * Constructs a TaskTip with plain text style.
     *
     * The default style displays plain text to indicate progress. The TaskTip
     * is disposed if the parent Shell is disposed.
     *
     * @param shell
     *            The Shell the TaskTip is associated with
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the shell value is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the display</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     */
    public TaskTip(Shell shell)
    {
        this(shell, SWT.NONE);
    }

    /**
     *
     * Constructs a TaskTip with a given style value. The TaskTip
     * is disposed if the parent Shell is disposed.
     *
     * @param shell
     *            The Shell the TaskTip is associated with
     * @param style
     *            style of TaskTip: NONE, SMOOTH or INDETERMINATE.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the shell value is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the display</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * @see SWT#SMOOTH
     * @see SWT#INDETERMINATE
     */
    public TaskTip(Shell shell, int style)
    {
        super(shell, checkStyle(style));

        if (shell == null)
        {
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        }

        tasktipShell = new Shell(shell, SWT.NO_FOCUS|SWT.ON_TOP|SWT.NO_TRIM);
        GridLayout layout = new GridLayout();
        layout.marginBottom = 0;
        layout.marginTop = 0;
        layout.marginLeft = 0;
        layout.marginRight = 0;
        layout.marginHeight = 0;
        layout.marginWidth = 0;
        layout.verticalSpacing = 1;
        layout.horizontalSpacing = 0;
        tasktipShell.setLayout(layout);
        tasktipShell.internal_setTaskTip();
        if (getStyle() != SWT.NONE)
        {
            tasktipBar = new ProgressBar(tasktipShell, getStyle());
            tasktipBar.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
        }

        // If the parent shell is disposed task tip is disposed also
        shell.addDisposeListener(new DisposeListener()
        {
            public void widgetDisposed(DisposeEvent e)
            {
                dispose();
            }
        });

        // Find top shell and listen to resize events (means actually screen rotation)
        topShellListener = new ControlListener()
        {
            public void controlMoved(ControlEvent ev)
            {
            }
            public void controlResized(ControlEvent ev)
            {
                rebound();
            }
        };
        Shell topShellTemp = shell;
        while (topShellTemp.getParent() != null && topShellTemp.getParent().getShell() != null)
        {
            topShellTemp = topShellTemp.getParent().getShell();
        }
        topShell = topShellTemp;
        topShell.addControlListener(topShellListener);

        rebound();
    }

    /**
     * Sets the maximum value that the TaskTip will allow.
     * If the new maximum is applied, the TaskTip's selection value is
     * adjusted if necessary to fall within the new range.
     * <p>
     * This method works only for {@link SWT#SMOOTH SWT.SMOOTH} style. This method
     * is silently ignored for other styles.
     * </p>
     *
     * @param value
     *            the new maximum, which must be positive and greater than or
     *            equal to the current minimum
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the value is negative or
     *                less than the current minimum</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setMaximum(int value)
    {
        checkWidget();
        if (getStyle() != SWT.SMOOTH)
        {
            return;
        }
        if (value < 0 || value < minimum)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            return;
        }
        maximum = value;
        if (selection > maximum)
        {
            selection = maximum;
        }
        tasktipBar.setMaximum(maximum);
    }

    /**
     * Sets the minimum value that the TaskTip will allow.
     * If the new minimum is applied, the TaskTip's selection
     * value is adjusted if necessary to fall within the new range.
     *
     * <p>
     * This method works only for {@link SWT#SMOOTH SWT.SMOOTH} style. This method
     * is silently ignored for other styles.
     * </p>
     *
     * @param value
     *            the new minimum, which must be positive and less than or
     *            equal to the current maximum
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the value is negative or
     *                greater than the current maximum</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setMinimum(int value)
    {
        checkWidget();
        if (getStyle() != SWT.SMOOTH)
        {
            return;
        }
        if (value < 0 || value > maximum)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            return;
        }
        minimum = value;
        if (selection < minimum)
        {
            selection = minimum;
        }
        tasktipBar.setMinimum(minimum);
    }

    /**
     *
     * Sets the position of the TaskTip's indicator to the provided value.
     * The value must fall in the range defined by setMinimum() and setMaximum().
     *
     * <p>
     * This method works only for {@link SWT#SMOOTH SWT.SMOOTH} style. This method
     * is silently ignored for other styles.
     * </p>
     *
     * @param value
     *            the new selection (must be zero or greater).
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the value is not in
     *                between the current minimum and maximum</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>IllegalArgumentException - if the vlaue falls out of
     *                the range defined by setMinimum() and setMaximum()</li>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     */
    public void setSelection(int value)
    {
        checkWidget();
        if (getStyle() != SWT.SMOOTH)
        {
            return;
        }
        if (value < minimum || value > maximum)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
            return;
        }
        selection = value;
        tasktipBar.setSelection(selection);
    }

    /**
     * Sets the label text.
     * <p>
     * This method sets a TaskTip's label . The label can be null. The label's
     * position and the visibility of label text is platform-dependent.
     * </p>
     *
     * @param string
     *            the label text
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     *
     */
    public void setText(String string)
    {
        checkWidget();
        boolean prevLabel = tasktipLabel != null;
        if (string == null)
        {
            if (tasktipLabel != null)
            {
                tasktipLabel.dispose();
                tasktipLabel = null;
            }
        }
        else
        {
            if (tasktipLabel == null)
            {
                tasktipLabel = new Label(tasktipShell, SWT.NONE);
                tasktipLabel.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, true));
            }
            tasktipLabel.setText(removeLine(string));
        }
        if (prevLabel != (tasktipLabel != null))
        {
            rebound();
        }
    }

    /**
     * Makes the TaskTip visible and brings it to the front of the display.
     *
     * <p>
     * This method will return immediately after showing or hiding the TaskTip.
     * </p>
     * <p>
     * Despite setting a TaskTip visible, it may not actually be displayed if the
     * device's main screen is not active.
     * </p>
     *
     * @param visible
     *            the value to show or hide the TaskTip. The method will not
     *            return an error if the value matches the current visibility
     *            state.
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the TaskTip has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setVisible(boolean visible)
    {
        checkWidget();
        if (tasktipShell.isVisible() == visible)
        {
            return;
        }
        tasktipShell.setVisible(visible);
    }

    protected void internal_releaseResources()
    {
        if (tasktipShell != null)
        {
            topShell.removeControlListener(topShellListener);
            topShellListener = null;
            tasktipShell.dispose();
            tasktipShell = null;
            tasktipLabel = null;
            tasktipBar = null;
        }
        super.internal_releaseResources();
    }

    private void rebound()
    {
        Rectangle shellRect = OS.TaskTip_DefaultBounds();
        
        Point idealSize = tasktipShell.computeSize(shellRect.width, SWT.DEFAULT);
        if (shellRect.height < idealSize.y)
            shellRect.height = idealSize.y;
        
        tasktipShell.setBounds(shellRect);
        tasktipShell.layout();
    }

    private String removeLine(String aString)
    {
        String newString = "";
        if (aString.indexOf("\n") > 0 || aString.indexOf("\r") > 0)
        {
            char[] ch=aString.toCharArray();
            for (int i=0; i<ch.length; i++)
            {
                if (!(ch[i]=='\n' || ch[i]=='\r'))
                {
                    newString = newString+ch[i];
                }
            }
        }
        else
        {
            newString = aString;
        }
        return newString;
    }

    static private int checkStyle(int style)
    {
        // Only one of NONE, INDETERMINATE or SMOOTH allowed (in this given order). Default is NONE.
        return internal_checkBits(style, SWT.NONE, SWT.INDETERMINATE, SWT.SMOOTH, 0, 0, 0);
    }
}
