/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

import javax.microedition.midlet.MIDlet;
import java.io.IOException;

import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;

import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.mj.impl.rt.legacy.LegacyRtPort;

import com.nokia.mid.ui.impl.DeviceControlImpl;

public class Display
{
    public static final int LIST_ELEMENT = 1;
    public static final int CHOICE_GROUP_ELEMENT = 2;
    public static final int ALERT = 3;
    //
    public static final int COLOR_BACKGROUND = 0;
    public static final int COLOR_FOREGROUND = 1;
    public static final int COLOR_HIGHLIGHTED_BACKGROUND = 2;
    public static final int COLOR_HIGHLIGHTED_FOREGROUND = 3;
    public static final int COLOR_BORDER = 4;
    public static final int COLOR_HIGHLIGHTED_BORDER = 5;

    private static final int REQUEST_NONE       = 0;
    private static final int REQUEST_SWITCH     = 1;
    private static final int REQUEST_BACKGROUND = 2;
    private static final int REQUEST_FOREGROUND = 3;

    /**
     *
     */
    static Hashtable sDisplayMap = new Hashtable();

    private MIDlet  iMIDlet;
    private Toolkit iToolkit;
    final Object    iCallbackLock;

    //
    private Displayable iCurrent;
    private Displayable iPending;
    private Displayable iLastDisplayable;
    private Displayable iUnderCurrent;
    private int         iRequest;

    private Device      iDevice;
    private boolean     iForeground;
    private Vector      iRunQueue;

    Display(MIDlet aMIDlet)
    {
        LegacyRtPort.registerToolkit(this.getClass().getName());

        iMIDlet       = aMIDlet;
        iToolkit      = Toolkit.getToolkit();
        iCallbackLock = iToolkit.getCallbackLock();
        iDevice       = iToolkit.getDevice();
        iRunQueue     = new Vector();
        iToolkit.start(this);
    }

    /**
     * API
     */
    public static Display getDisplay(MIDlet aMIDlet)
    {
        synchronized (sDisplayMap)
        {
            if (aMIDlet == null)
            {
                throw new NullPointerException();
            }
            Display display = (Display)sDisplayMap.get(aMIDlet);
            if (null == display)
            {
                display = new Display(aMIDlet);
                sDisplayMap.put(aMIDlet, display);
            }
            return display;
        }
    }

    void dispose()
    {
        synchronized (sDisplayMap)
        {
            LegacyRtPort.unRegisterToolkit(this.getClass().getName());
            sDisplayMap.remove(iMIDlet);
        }
    }

    /**
     * API
     */
    public boolean isColor()
    {
        return iDevice.isColor();
    }

    /**
     * API
     */
    public int numColors()
    {
        return iDevice.numColors();
    }

    /**
     * API
     */
    public Displayable getCurrent()
    {
        return iCurrent;
    }

    /**
     * API
     */
    public void setCurrent(Displayable aDisplayable)
    {
        synchronized (iToolkit)
        {
            if (aDisplayable instanceof Alert  && (aDisplayable != iCurrent))   // dialog
            {
                Alert alert = (Alert)aDisplayable;
                alert.SetReturnScreen(iCurrent);
            }
            DoSetCurrent(aDisplayable);
        }
    }

    /**
     * API
     */
    public void setCurrent(Alert aAlert, Displayable aReturnScreen)
    {
        if ((aAlert == null) || (aReturnScreen == null))
        {
            throw new NullPointerException();
        }

        synchronized (iToolkit)
        {
            CheckToolkit(aReturnScreen);
            aAlert.SetReturnScreen(aReturnScreen);
            DoSetCurrent(aAlert);
        }
    }

    /**
     * Implementation method checks switch arguments are valid and posts
     * switch event to schedule screen switch on event thread.
     *
     * Toolkit lock must be held by calling thread.
     */
    final void DoSetCurrent(Displayable aPending)
    {
        CheckToolkit(aPending);

        if (null == aPending)
        {
            iRequest = REQUEST_BACKGROUND;
        }
        else if (iCurrent == aPending)
        {
            iRequest = REQUEST_FOREGROUND;
        }
        else if (iLastDisplayable == aPending)
        {
            // To avoid problems when midlet set current displayable twice
            // in a row and java event wasn't handled in time before
            // next setCurrent was called
            return;
        }
        else
        {
            iRequest = REQUEST_SWITCH;
        }

        iLastDisplayable = aPending;
        iPending = aPending;

        iToolkit.postEvent(Toolkit.EVENT_SET_CURRENT);

        com.nokia.mj.impl.utils.StartUpTrace.doTrace("UI ready");
    }

    /**
     * API
     *
     * Call a runnable serialized with event stream.
     *
     */
    public void callSerially(Runnable aRunnable)
    {
        synchronized (iToolkit)
        {
            iRunQueue.addElement(aRunnable);
            iToolkit.postEvent(Toolkit.EVENT_SERIAL);
        }
    }

    /**
     * Handles, one by one, call serially events received from native side
     * caused by <code>Display#callSerially</code> method calls.
     */
    void processSerialEvents()
    {
        Runnable runnable = null;
        synchronized (iToolkit)
        {
            try
            {
                runnable = (Runnable)iRunQueue.firstElement();
                iRunQueue.removeElement(runnable);
            }
            catch (Exception e)
            {
                runnable = null;
            }
        }
        if (runnable != null)
        {
            synchronized (iCallbackLock)
            {
                runnable.run();
            }
        }
    }

    /**
     * Implementation.
     *
     * Request MIDlet be brought to foreground (aForeground is true)
     * or sent to background (aForeground is false).
     */
    final void requestForeground(boolean aForeground)
    {
        iToolkit.setForeground(aForeground);
    }

    /**
     * Implementation
     * Responds to foreground notification from UI thread.
     * These occur when the MIDLet UI is foregrounded either by the user
     * or by the application makeing a foreground request.
     *
     *@see requestForeground
     */
    final void handleForeground(boolean aForeground)
    {
        synchronized (iCallbackLock)
        {
            final boolean foreground;
            final Displayable current;

            synchronized (iToolkit)
            {
                foreground=iForeground;
                current=iCurrent;
            }

            if (aForeground == foreground)
            {
                //
                // No change, ignore event.
                //
                return;
            }

            if (aForeground && (null != current))
            {
                //
                // We are gaining foreground, showNotify must be
                // called before isShown() returns true. That
                // occures when the displayable is current and
                // the display is foreground.
                //
                current.showNotifyEvent(this);
            }

            // Take the lock so other internal lcdui methods don't find
            // foreground state changing under them unexpectedly.
            synchronized (iToolkit)
            {
                iForeground = aForeground;
            }

            if (null != current)
            {
                if (aForeground)
                {
                    current.update();
                }
                else
                {
                    current.hideNotifyEvent(this);
                }
            }
        }
    }

    /**
     * Implementation.
     *
     *@see Displayable.isShown()
     */
    final boolean IsShown(Displayable aDisplayable)
    {
        return (aDisplayable == iCurrent) && IsForeground();
    }

    /**
     * Implementation
     */
    final boolean IsForeground()
    {
        return iForeground;
    }


    /**
     * Implementation.
     *
     * Reject displayables created by other MIDlets in the same suite. These
     * would be associated with native windows from a different UI thread
     * and so would panic the UI thread if passed native.
     */
    private void CheckToolkit(Displayable aDisplayable)
    {
        if ((null != aDisplayable) && aDisplayable.iToolkit != iToolkit)
        {
            final String msg = "Displayable in setCurrent() belongs to the wrong toolkit";
            throw new IllegalArgumentException(msg);
        }
    }

    /**
     * Implementation.
     *
     * Called by toolkit in response to SET_CURRENT event posted when the
     * native activation occurs.
     */
    void switchCurrent()
    {
        synchronized (iCallbackLock)
        {
            /**
            Since we cannot hold the toolkit lock during a showNotify()/hideNotify()
            callback, we have to guarantee that iForeground is only ever modified with
            the event sequence lock (callback lock) held.
            Similarly if iCurrent is only updated in this method, then we can read both
            iForeground and iCurrent without requiring any additional synchronization
            (over the event sequence lock). Writing is another matter of course....
            */

            final Displayable pending;
            final Displayable current;
            final boolean foreground;
            final int request;
            int err = 0;    // KErrNone

            synchronized (iToolkit)
            {
                current = iCurrent;
                pending = iPending;
                foreground = iForeground;
                request = iRequest;
                iPending = null;
                iRequest = REQUEST_NONE;
            }

            if (request == REQUEST_NONE)
            {
                //
                // Nothing to do, last request must
                // have aleady been processed.
                //
                return;
            }

            if (request == REQUEST_BACKGROUND)
            {
                //
                // Last request was background.
                //
                requestForeground(false);
                return;
            }

            if (request == REQUEST_SWITCH)
            {
                if (foreground)
                {
                    pending.showNotifyEvent(this);
                }

                synchronized (iToolkit)
                {
                    err = iToolkit._setCurrent(iToolkit.getHandle(), pending.getContainerHandle());
                    if (0 == err)
                    {
                        iCurrent = pending;
                        if (!(pending instanceof Alert))
                        {
                            // We've switched to a full-screen displayable, so clear
                            iUnderCurrent = null;
                        }
                        else if (current != null && !(current instanceof Alert))
                        {
                            // We've switched to an alert, so update if we replaced
                            // a full-screen displayable
                            iUnderCurrent = current;
                        }
                    }
                }

                if (foreground)
                {
                    if (0 == err)
                    {
                        //
                        // Successful switch
                        //
                        if (null != current)
                        {
                            // hide previous
                            current.hideNotifyEvent(this);
                        }
                        // repaint new current
                        pending.update();
                    }
                    else
                    {
                        //
                        // Switch failed - hide the pending displayable
                        //
                        pending.hideNotifyEvent(this);
                    }
                }

            }

            //
            // Always request foreground.
            //
            requestForeground(true);

            /**
             If the native control failed to activate correctly we throw an
             exception here. The application will not see it - but the default
             exception handler will, and will hopefully display an error.
             */
            NativeError.check(err);
        }
    }

    /**
     * API
     */
    public boolean flashBacklight(int aDuration)
    {
        if (aDuration < 0)
        {
            throw new IllegalArgumentException();
        }
        if (iForeground)
        {
            return DeviceControlImpl.flashLights(aDuration);
        }
        return false;
    }

    /**
     * API
     */
    public boolean vibrate(int aDuration)
    {
        if (aDuration < 0)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            return _vibrate(iToolkit.getHandle(),aDuration);
        }
    }

    /**
     * API
     */
    public int getColor(int aColorSpecifier)
    {
        if (aColorSpecifier < COLOR_BACKGROUND || aColorSpecifier >  COLOR_HIGHLIGHTED_BORDER)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            return NativeError.check(_getColor(iToolkit.getHandle(),aColorSpecifier));
        }
    }

    /**
     * API
     */
    public int getBorderStyle(boolean aHighlighted)
    {
        synchronized (iToolkit)
        {
            return NativeError.check(_getBorderStyle(iToolkit.getHandle(),aHighlighted));
        }
    }

    /**
     * API
     *
     * For images that have alpha channels we use EGray256 displaymode
     * masks. For images that only have binary transparency we use
     * EGray2 masks.
     *
     * Here we return the maximum we support. Consider querying
     * lcdgr.dll for actuall maximum if some specialized displaymode is
     * used.
     */
    public int numAlphaLevels()
    {
        return iDevice.numAlphas();
    }

    /**
     * API
     *
     * This method always returns immediately, without waiting for the switching of the
     * Displayable, the scrolling, and the assignment of input focus to take place.
     */
    public void setCurrentItem(Item aItem)
    {
        final Form form = aItem.getForm();
        if (form == null)
        {
            throw new IllegalStateException();
        }
        form.setCurrentItem(aItem);
        setCurrent(form);
    }

    /**
     * API
     */
    public int getBestImageWidth(int aImageType)
    {
        if ((aImageType < LIST_ELEMENT) || (aImageType > ALERT))
            throw new IllegalArgumentException();
        return NativeError.check(_getBestImageWidth(iToolkit.getHandle(),aImageType));
    }

    /**
     * API
     */
    public int getBestImageHeight(int aImageType)
    {
        if ((aImageType < LIST_ELEMENT) || (aImageType > ALERT))
            throw new IllegalArgumentException();
        return NativeError.check(_getBestImageHeight(iToolkit.getHandle(),aImageType));
    }


    private native boolean _flashBacklight(int aToolkit,int aDuration);
    private native boolean _vibrate(int aToolkit,int aDuration);
    private native int _getColor(int aToolkit,int aColorSpecifier);
    private native int _getBorderStyle(int aToolkit,boolean aHighlighted);
    private native int _getBestImageWidth(int aToolkit,int aImageType);
    private native int _getBestImageHeight(int aToolkit,int aImageType);
}
