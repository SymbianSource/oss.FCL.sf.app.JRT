/*******************************************************************************
 * Copyright (c) 2003, 2004 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Lynne Kues (IBM Corp) - modified to reflect eSWT API subset
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.browser;

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.browser.ProgressListener;
import org.eclipse.swt.browser.StatusTextListener;
import org.eclipse.swt.browser.TitleListener;
import org.eclipse.swt.graphics.Point;
import org.eclipse.ercp.swt.expanded.internal.OS;

/**
 * Instances of this class implement the browser user interface
 * metaphor.  It allows the user to visualize and navigate through
 * HTML documents.
 * <p>
 * Note that although this class is a subclass of <code>Composite</code>,
 * it does not make sense to set a layout on it.
 * </p><p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 *
 */
public class Browser  extends Composite
{
    private int   browserHandle;

    LocationListener[] locationListeners = new LocationListener[0];
    ProgressListener[] progressListeners = new ProgressListener[0];
    StatusTextListener[] statusTextListeners = new StatusTextListener[0];
    TitleListener[] titleListeners = new TitleListener[0];


    /**
     * Constructs a new instance of this class given its parent
     * and a style value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in
     * class <code>SWT</code> which is applicable to instances of this
     * class, or must be built by <em>bitwise OR</em>'ing together
     * (that is, using the <code>int</code> "|" operator) two or more
     * of those <code>SWT</code> style constants. The class description
     * lists the style constants that are applicable to the class.
     * Style bits are also inherited from superclasses.
     * </p>
     *
     * @param parent a widget which will be the parent of the new instance (cannot be null)
     * @param style the style of widget to construct
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     * </ul>
     * @exception SWTError <ul>
     *    <li>ERROR_NO_HANDLES if a handle could not be obtained for browser creation</li>
     * </ul>
     *
     * @see #getStyle
     *
     */

    public Browser(Composite parent, int style)
    {
        //The scrollbars of native browser control is not accessible. Reset scrollbar bits in order to cut off the relation to native scrollbar.
        super(parent,  style);
    }

    final void setBrowserHandle(int browserhandle)
    {
        this.browserHandle = browserhandle;
        internal_setCompositeHandle(OS.Browser_CompositeHandle(browserHandle));
    }

    protected void internal_createHandle(int style)
    {
        if (OS.Browser_ExistsInstanceAlready())
        {
            SWT.error(SWT.ERROR_UNSPECIFIED, null, "There can be only one Browser instance at a given time");
        }

        setBrowserHandle(OS.Browser_New(this, getParent().internal_getCompositeHandle(),style));
    }

    protected void internal_createHandle()
    {
    }

    public Point computeSize(int wHint, int hHint, boolean changed)
    {
        // We must redefine this method to override Composite implementation.
        checkWidget();
        return org.eclipse.swt.internal.symbian.OS.Control_ComputeSize(handle, wHint, hHint);
    }


    /**
     * Adds the listener to receive events.
     * <p>
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void addLocationListener(LocationListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        LocationListener[] newLocationListeners = new LocationListener[locationListeners.length + 1];
        System.arraycopy(locationListeners, 0, newLocationListeners, 0, locationListeners.length);
        locationListeners = newLocationListeners;
        locationListeners[locationListeners.length - 1] = listener;
    }

    /**
     * Adds the listener to receive events.
     * <p>
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void addProgressListener(ProgressListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        ProgressListener[] newProgressListeners = new ProgressListener[progressListeners.length + 1];
        System.arraycopy(progressListeners, 0, newProgressListeners, 0, progressListeners.length);
        progressListeners = newProgressListeners;
        progressListeners[progressListeners.length - 1] = listener;
    }

    /**
     * Adds the listener to receive events.
     * <p>
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void addStatusTextListener(StatusTextListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        StatusTextListener[] newStatusTextListeners = new StatusTextListener[statusTextListeners.length + 1];
        System.arraycopy(statusTextListeners, 0, newStatusTextListeners, 0, statusTextListeners.length);
        statusTextListeners = newStatusTextListeners;
        statusTextListeners[statusTextListeners.length - 1] = listener;
    }

    /**
     * Adds the listener to receive events.
     * <p>
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void addTitleListener(TitleListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        TitleListener[] newTitleListeners = new TitleListener[titleListeners.length + 1];
        System.arraycopy(titleListeners, 0, newTitleListeners, 0, titleListeners.length);
        titleListeners = newTitleListeners;
        titleListeners[titleListeners.length - 1] = listener;
    }

    /**
     * Navigate to the previous session history item.
     *
     * @return <code>true</code> if the operation was successful and <code>false</code> otherwise
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     * @see #forward
     *
     */
    public boolean back()
    {
        checkWidget();
        if (isBackEnabled())
        {
            return OS.Browser_Back(browserHandle);
        }
        return false;

    }

    /**
     * Navigate to the next session history item.
     *
     * @return <code>true</code> if the operation was successful and <code>false</code> otherwise
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     * @see #back
     *
     */
    public boolean forward()
    {
        checkWidget();
        if (isForwardEnabled())
        {
            return OS.Browser_Forward(browserHandle);
        }
        return false;
    }

    /**
     * Returns <code>true</code> if the receiver can navigate to the
     * previous session history item, and <code>false</code> otherwise.
     *
     * @return the receiver's back command enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #back
     */
    public boolean isBackEnabled()
    {
        checkWidget();
        return OS.Browser_IsBackEnabled(browserHandle);
    }

    /**
     * Returns <code>true</code> if the receiver can navigate to the
     * next session history item, and <code>false</code> otherwise.
     *
     * @return the receiver's forward command enabled state
     *
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see #forward
     */
    public boolean isForwardEnabled()
    {
        checkWidget();
        return OS.Browser_IsForwardEnabled(browserHandle);
    }

    /**
     * Returns the current URL.
     *
     * @return the current URL or an empty <code>String</code> if there is no current URL
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     * @see #setUrl
     *
     */
    public String getUrl()
    {
        checkWidget();
        return OS.Browser_GetUrl(browserHandle);
    }

    /**
     * Refresh the current page.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
      */
    public void refresh()
    {
        checkWidget();
        OS.Browser_Refresh(browserHandle);
    }

    /**
     * Removes the listener.
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void removeLocationListener(LocationListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (locationListeners.length == 0) return;
        int index = -1;
        for (int i = 0; i < locationListeners.length; i++)
        {
            if (listener == locationListeners[i])
            {
                index = i;
                break;
            }
        }
        if (index == -1) return;
        if (locationListeners.length == 1)
        {
            locationListeners = new LocationListener[0];
            return;
        }
        LocationListener[] newLocationListeners = new LocationListener[locationListeners.length - 1];
        System.arraycopy(locationListeners, 0, newLocationListeners, 0, index);
        System.arraycopy(locationListeners, index + 1, newLocationListeners, index, locationListeners.length - index - 1);
        locationListeners = newLocationListeners;
    }

    /**
     * Removes the listener.
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void removeProgressListener(ProgressListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (progressListeners.length == 0) return;
        int index = -1;
        for (int i = 0; i < progressListeners.length; i++)
        {
            if (listener == progressListeners[i])
            {
                index = i;
                break;
            }
        }
        if (index == -1) return;
        if (progressListeners.length == 1)
        {
            progressListeners = new ProgressListener[0];
            return;
        }
        ProgressListener[] newProgressListeners = new ProgressListener[progressListeners.length - 1];
        System.arraycopy(progressListeners, 0, newProgressListeners, 0, index);
        System.arraycopy(progressListeners, index + 1, newProgressListeners, index, progressListeners.length - index - 1);
        progressListeners = newProgressListeners;
    }

    /**
     * Removes the listener.
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void removeStatusTextListener(StatusTextListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (statusTextListeners.length == 0) return;
        int index = -1;
        for (int i = 0; i < statusTextListeners.length; i++)
        {
            if (listener == statusTextListeners[i])
            {
                index = i;
                break;
            }
        }
        if (index == -1) return;
        if (statusTextListeners.length == 1)
        {
            statusTextListeners = new StatusTextListener[0];
            return;
        }
        StatusTextListener[] newStatusTextListeners = new StatusTextListener[statusTextListeners.length - 1];
        System.arraycopy(statusTextListeners, 0, newStatusTextListeners, 0, index);
        System.arraycopy(statusTextListeners, index + 1, newStatusTextListeners, index, statusTextListeners.length - index - 1);
        statusTextListeners = newStatusTextListeners;
    }

    /**
     * Removes the listener.
     *
     * @param listener the listener
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     */
    public void removeTitleListener(TitleListener listener)
    {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        if (titleListeners.length == 0) return;
        int index = -1;
        for (int i = 0; i < titleListeners.length; i++)
        {
            if (listener == titleListeners[i])
            {
                index = i;
                break;
            }
        }
        if (index == -1) return;
        if (titleListeners.length == 1)
        {
            titleListeners = new TitleListener[0];
            return;
        }
        TitleListener[] newTitleListeners = new TitleListener[titleListeners.length - 1];
        System.arraycopy(titleListeners, 0, newTitleListeners, 0, index);
        System.arraycopy(titleListeners, index + 1, newTitleListeners, index, titleListeners.length - index - 1);
        titleListeners = newTitleListeners;
    }

    /**
     * Renders HTML.
     * <p>
     * The html parameter is Unicode encoded since it is a java <code>String</code>.
     * As a result, the HTML meta tag charset should not be set. The charset is implied
     * by the <code>String</code> itself.
     *
     * @param html the HTML content to be rendered
     *
     * @return true if the operation was successful and false otherwise.
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the html is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     * @see #setUrl
     *
     */
    public boolean setText(String html)
    {

        checkWidget();
        if (html == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return OS.Browser_SetText(browserHandle, html);
    }

    /**
     * Loads a URL.
     *
     * @param url the URL to be loaded
     *
     * @return true if the operation was successful and false otherwise.
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the url is null</li>
     * </ul>
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     * @see #getUrl
     *
      */
    public boolean setUrl(String url)
    {
        checkWidget();
        if (url == null) internal_error(SWT.ERROR_NULL_ARGUMENT);
        return OS.Browser_SetUrl(browserHandle, url);
    }

    /**
     * Stop any loading and rendering activity.
     *
     * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS when called from the wrong thread</li>
     *    <li>ERROR_WIDGET_DISPOSED when the widget has been disposed</li>
     * </ul>
     *
     * @since 3.0
     */
    public void stop()
    {
        checkWidget();
        OS.Browser_Stop(browserHandle);
    }

    private boolean internal_sendLocationChangingEvent(boolean doIt, boolean top, String location)
    {
        LocationEvent event = new LocationEvent(this);
        boolean finalDoIt=true;
        for (int i = 0; i < locationListeners.length; i++)
        {
            event.doit = doIt;
            event.location = location;
            event.top = top;
            locationListeners[i].changing(event);
            if (finalDoIt == true)
            {
                if (event.doit == false)
                {
                    finalDoIt = false;
                }
            }
        }
        return finalDoIt;
    }

    private void internal_sendLocationChangedEvent(boolean doIt, boolean top, String location)
    {
        LocationEvent event = new LocationEvent(this);
        for (int i = 0; i < locationListeners.length; i++)
        {
            event.doit = doIt;
            event.location = location;
            event.top = top;
            locationListeners[i].changed(event);
        }

    }

    private void internal_sendProgressEvent(int current,  int total)
    {
        ProgressEvent event = new ProgressEvent(this);
        for (int i = 0; i < progressListeners.length; i++)
        {
            event.current = current;
            event.total = total;
            progressListeners[i].changed(event);
        }
    }

    private void internal_sendProgressCompletedEvent(int current,  int total)
    {
        ProgressEvent event = new ProgressEvent(this);
        for (int i = 0; i < progressListeners.length; i++)
        {
            event.current = current;
            event.total = total;
            progressListeners[i].completed(event);
        }
    }

    private void internal_sendStatusTextEvent(String status)
    {
        StatusTextEvent event = new StatusTextEvent(this);
        for (int i = 0; i < statusTextListeners.length; i++)
        {
            event.text = status;
            statusTextListeners[i].changed(event);
        }
    }

    private void internal_sendTitleEvent(String title)
    {
        TitleEvent event = new TitleEvent(this);
        for (int i = 0; i < titleListeners.length; i++)
        {
            event.title = title;
            titleListeners[i].changed(event);
        }
    }
}
