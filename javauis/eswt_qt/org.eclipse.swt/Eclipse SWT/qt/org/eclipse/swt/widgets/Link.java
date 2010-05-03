/*******************************************************************************
 * Copyright (c) 2000, 2007 IBM Corporation and others.
 * Portion Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.widgets.TypedListener;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.WidgetState;

/**
 * 
 * Instances of this class represent a selectable user interface object that
 * launchs other applications when activated by the end-user.
 * 
 * <p>
 * This class represents several types of hyperlinks associated with certain
 * functionalities. The end-user can activate the associated program determined
 * by the style. The concrete visual effect and activation behaviour vary from
 * platform to platform.
 * </p>
 * <p>
 * A HyperLink instance accepts general characters as other controls do, but the
 * appearance is implementation and locale dependent, for example, a HyperLink
 * object with the PHONE style might display as follows: <br>
 * (416) 123-4567 <br>
 * but the actual contents of the object visible to the application through the
 * APIs, e.g. <code>getText()</code>, can be the string "4161234567".
 * </p>
 * <p>
 * <b>Example </b>
 * 
 * <pre>
 * HyperLink email = new HyperLink(composite, SWT.NONE, HyperLink.EMAIL);
 * email.setText(&quot;firstname.lastname@foo.com&quot;);
 * HyperLink dialler = new HyperLink(composite, SWT.BORDER | SWT.LEFT,
 *         HyperLink.PHONE);
 * dialler.setText(&quot;3581234567&quot;);
 * </pre>
 * 
 * </p>
 * <dl>
 * <dt><b>Styles: </b></dt>
 * <dd>BORDER, CENTER, LEFT, RIGHT</dd>
 * <dt><b>Format Styles: </b></dt>
 * <dd>URL: launches a platform specific web browser when activated</dd>
 * <dd>EMAIL: opens the platform specific e-mail client when activated</dd>
 * <dd>PHONE: shows a platform specific dialler interface when activated</dt>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * Note: Since the style provides hint for the implementation, there is no event
 * that applications need to listen to.
 * </p>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class Link extends org.eclipse.swt.widgets.Control {
    
    String text;

    /*
     * This flag is set to true when the control gains focus because of 
     * keyboard event. In such a case user needs to press tab again to 
     * get the link highlighted, so we cannot traverse.
     */
    boolean keyFocusIn;
    
    static final String LINK_START = "<a>";
    static final String LINK_END = "</a>";
    
    /**
     * Constructs a new instance of this class given its parent, a style value
     * describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * 
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * 
     * @see SWT#BORDER
     * @see SWT#CENTER
     * @see SWT#LEFT
     * @see SWT#RIGHT
     */
    public Link(Composite parent, int style) {
        this(parent, style, 0, null);
    }

    /**
     * <p>
     * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
     * public API. It should never be referenced from application code. 
     * </p>
     */
    protected Link(Composite parent, int style, int format, Object packageProxy) {
        super(parent,
              checkBits(style, SWT.LEFT, SWT.CENTER, SWT.RIGHT, 0, 0, 0),
              format, packageProxy, false);
        setAlignment(style);
        OS.QLabel_setTextInteractionFlags(handle, OS.QT_TEXTBROWSERINTERACTION);
        OS.QLabel_setOpenExternalLinks(handle, false);
        text = "";
    }
    
    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control is selected by the user, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * <code>widgetSelected</code> is called when the control is selected.
     * <code>widgetDefaultSelected</code> is not called.
     * </p>
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see SelectionEvent
     */
    public void addSelectionListener (SelectionListener listener) {
        checkWidget ();
        if (listener == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener (listener);
        addListener(SWT.Selection,typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    void createHandle_pp(int index) {
        frameHandle = topHandle = handle = OS.QLabel_new();
        state |= WidgetState.HANDLE;
    }

    /**
     * Returns the receiver's text, which will be an empty string if it has
     * never been set.
     * 
     * @return the receiver's text
     * 
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see #setText(String)
     */
    public String getText() {
        checkWidget();
        return text;
    }

    
    void hookEvents_pp() {
        super.hookEvents_pp();
        int signalProxy = OS.SignalHandler_new(handle, display,
                OS.QSIGNAL_LINKACTIVATED);
        OS.QObject_connectOrThrow(handle, "linkActivated(const QString&)",
                signalProxy, "widgetSignal(const QString&)",
                OS.QT_AUTOCONNECTION);
    }
    
    boolean qt_event_focusIn(int reason) {
        
        if (reason != OS.QT_MOUSEFOCUSREASON) {
            keyFocusIn = true;
        }
        
        return super.qt_event_focusIn(reason);
    }

    boolean qt_event_focusOut(int reason) {
        keyFocusIn = false;
        return super.qt_event_focusOut(reason);
    }

    void qt_signal_linkActivated_pp(String link) {
        Event event = new Event ();
        event.text = link;
        sendEvent (SWT.Selection, event);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the control is selected by the user.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener (SelectionListener listener) {
        checkWidget();
        if (listener == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
        removeListener(SWT.Selection, listener);
        removeListener(SWT.DefaultSelection, listener);    
    }

    void setAlignment(int style) {
        int qAlign = OS.QT_ALIGNVCENTER;
        if ((style & SWT.LEFT) != 0) {
            qAlign |= OS.QT_ALIGNLEFT;
        } else if ((style & SWT.CENTER) != 0) {
            qAlign |= OS.QT_ALIGNHCENTER;
        } else if ((style & SWT.RIGHT) != 0) {
            qAlign |= OS.QT_ALIGNRIGHT;
        }
        OS.QLabel_setAlignment(handle, qAlign);
    }

    public void setEnabled(boolean enabled) {
        
        /**
         * To disable/enable user interaction for a QLabel,
         * we need to set the text interaction flags.
         */
        if (enabled != getEnabled()) {
            OS.QLabel_setTextInteractionFlags(handle, 
                    enabled ? OS.QT_TEXTBROWSERINTERACTION : 
                              OS.QT_NOTEXTINTERACTION);
        }
        
        super.setEnabled(enabled);
    }

    /**
     * Sets the receiver's text.
     * <p>
     * This method sets the link text.
     * </p>
     * 
     * @param string
     *            the new text
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     * @see #getText()
     */
    public void setText(String string) {
        checkWidget();
        if (string == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        text = string;
        OS.QLabel_setText(handle, adjustTags(text));
    }
    
    /*
     * QLabel does not render a link as a link if it is created like this:
     *   Text and <a>link</a> and another <a>link</a>
     * QLabel requires the href part to be there also. This function changes
     * the string above to this:
     *   Text and <a href="link">link</a> and another <a href="link">link</a>
     */
    static String adjustTags(String str) {
        int pos = str.indexOf(LINK_START);

        if (pos == -1) {
            return str;
        }
        
        int start = 0;
        int endPos = pos;
        StringBuffer sb = new StringBuffer();
        String clean = "";
        
        while (pos != -1) {
            sb.append(str.substring(start, pos)).append("<a href=\"");
            start = pos + LINK_START.length();
            endPos = str.indexOf(LINK_END, start);           
            if (endPos != -1) {
                clean = str.substring(start, endPos);
                sb.append(clean);
            } else {
                // If the link format is invalid in any way, just use 
                // original string and try not to be too clever.
                return str;
            }
            
            sb.append("\">"+clean+LINK_END);
            start = endPos + LINK_END.length();     
            pos = str.indexOf(LINK_START, start);            
        }
        
        sb.append(str.substring(endPos));
        
        return sb.toString();
    }
    
    void setTraversalFlags_pp(int type, int key, int modifier,
            int character) {
        if (type == SWT.TRAVERSE_RETURN) {
            // use RETURN key to activate link,
            // which should be delivered to QLabel instance
            traverseDoit = false;
            traverseCancel = false;
            return;
        }
        
        super.setTraversalFlags_pp(type, key, modifier, character);
        
        switch (key) {
            case OS.QT_KEY_ENTER:  // By default, enter key is used for 
            case OS.QT_KEY_RETURN: // default selection, not traversing.
                traverseDoit = false;
                traverseCancel = false;
                break;
            case OS.QT_KEY_TAB:
                if (isFocusControl() && keyFocusIn) {
                    traverseDoit = false;
                    traverseCancel = false;
                    keyFocusIn = false;
                }
                break;
            default:
                break;
        }
    }
}