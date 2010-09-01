/*******************************************************************************
 * Copyright (c) 2004 IBM Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Mark Rogalski (IBM Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/

package org.eclipse.ercp.swt.mobile;

import org.eclipse.ercp.swt.mobile.internal.OS;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;

/**
 * Instances of this class represent key based input devices.
 *
 * <dl>
 * <dt><b>Input device types: </b></dt>
 * <dd>SOFTKEYS</dd>
 * <dd>KEYPAD: ITU-T keypad</dd>
 * <dd>LIMITED_KEYBOARD</dd>
 * <dd>FULL_KEYBOARD</dd>
 * </dl>
 *
 * @see MobileDevice
 * @see MobileDeviceListener
 */
public class Input
{

    private int         inputHandle;
    private boolean     isDisposed;

    /**
     * input device has one or more hardware keys whose meaning can be
     * configured
     */
    public static final int SOFTKEYS = 1;

    /**
     * input device has hardware keys labeled 0 - 9, *, and #
     */
    public static final int KEYPAD = 2;

    /**
     * input device has more hardware keys than a keypad, but fewer than a full
     * keyboard. For instance, number keys may overlap a row of letter keys.
     */
    public static final int LIMITED_KEYBOARD = 3;

    /**
     * input device has hardware keys typical in a full keyboard. For instance,
     * individual keys for all numbers and Latin letters.
     */
    public static final int FULL_KEYBOARD = 4;

    /**
     * Constructs a new instance of this class. Non-public to prevent
     * instantiation outside of this package.
     *
     * @see MobileDevice#getLocalInput
     */
    private Input This;

    /**
     * Screen id.
     */
    private int id;

    Input(int aInputId)
    {
        id = aInputId;
        isDisposed = false;
        This = this;
        (Display.getDefault()).syncExec(new Runnable()
        {
            public void run()
            {
                This.inputHandle = OS.Input_New(This, id);
            }
        });
    }

    private void internal_checkInput()
    {
        if (isDisposed)
        {
            SWT.error(SWT.ERROR_WIDGET_DISPOSED);
        }
    }

    /**
     * Returns the location of the input device.
     *
     * <ul>
     * <dt>Can be one of:</dt>
     * <dd>{@link MobileDevice#LOCAL LOCAL}</dd>
     * <dd>{@link MobileDevice#INTERNAL INTERNAL}</dd>
     * <dd>{@link MobileDevice#EXTERNAL EXTERNAL}</dd>
     * <dd>{@link MobileDevice#REMOTE REMOTE}</dd>
     * </ul>
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     *
     * @see MobileDevice#LOCAL
     * @see MobileDevice#INTERNAL
     * @see MobileDevice#EXTERNAL
     * @see MobileDevice#REMOTE
     */
    private int location;
    public int getLocation()
    {
        internal_checkInput();
        try
        {
            (Display.getDefault()).syncExec(new Runnable()
            {
                public void run()
                {
                    location = OS.Input_GetLocation(inputHandle);
                }
            });
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);
        }
        return location;
    }

    /**
     * Returns the type of input device.
     *
     * <ul>
     * <dt>Can be one of:</dt>
     * <dd>SOFTKEYS</dd>
     * <dd>KEYPAD</dd>
     * <dd>LIMITED_KEYBOARD</dd>
     * <dd>FULL_KEYBOARD</dd>
     * </ul>
     *
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                </ul>
     * @exception SWTError
     *                <ul>
     *                <li>ERROR_CANNOT_GET_SELECTION - if the operation fails
     *                because of an operating system failure</li>
     *                </ul>
     *
     * @see Input#SOFTKEYS
     * @see Input#KEYPAD
     * @see Input#LIMITED_KEYBOARD
     * @see Input#FULL_KEYBOARD
     */
    private int type;
    public int getType()
    {
        internal_checkInput();
        try
        {
            (Display.getDefault()).syncExec(new Runnable()
            {
                public void run()
                {
                    type = OS.Input_GetType(inputHandle);
                }
            });
        }
        catch (Throwable e)
        {
            SWT.error(SWT.ERROR_CANNOT_GET_SELECTION);
        }

        return type;
    }

    /**
     * Call to dispose the Input object.
     *
     */
    void internal_dispose()
    {
        isDisposed  = true;
        OS.Input_Dispose(inputHandle);
        inputHandle     = 0;
    }
}
