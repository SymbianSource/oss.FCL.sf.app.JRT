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
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;

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
public class HyperLink extends org.eclipse.swt.widgets.Link {

    /**
     * launches a implementation-dependent web browser when activated.
     */
    public static final int URL = 0x01;

    /**
     * opens the implementation-dependent e-mail client when activated.
     */
    public static final int EMAIL = URL << 1;

    /**
     * shows the implementation-dependent dialler interface when activated.
     */
    public static final int PHONE = URL << 2;

    static final String emailScheme = "mailto:";

    static final class HyperLinkPackageProxy extends PackageProxy {
        public void qt_signal_linkActivated(String link) {
            ((HyperLink)w).qt_signal_linkActivated(link);
        }
    }
    
    /**
     * Constructs a new instance of this class given its parent, a style value
     * and a format value describing its behavior and appearance.
     * <p>
     * The style value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or must
     * be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. The class description lists the style constants that are
     * applicable to the class. Style bits are also inherited from superclasses.
     * </p>
     * <p>
     * The format value must be one of URL, EMAIL and PHONE.
     * </p>
     * 
     * @param parent
     *            a composite control which will be the parent of the new
     *            instance (cannot be null)
     * @param style
     *            the style of control to construct
     * @param format
     *            the format value.
     * 
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if the format is invalid</li>
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
     * @see #URL
     * @see #EMAIL
     * @see #PHONE
     */
    public HyperLink(Composite parent, int style, int format) {
        super(parent, style, format, new HyperLinkPackageProxy());
        int extraStyle = Internal_PackageSupport.checkBits(checkFormat(format), URL, EMAIL, PHONE, 0, 0, 0);
        Internal_PackageSupport.setExtraStyle(this, extraStyle);
    }

    static int checkFormat(int format) {
        if (format != URL && format != EMAIL && format != PHONE) {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }
        return format;
    }

    void qt_signal_linkActivated(String link) {

        Event event = new Event();
        event.widget = this;
        event.doit = true;
        event.text = getText();
        Internal_PackageSupport.sendEvent(this, SWT.Selection, event);
        
        if (event.doit == true) {    
            if (extraStyle() == PHONE) {
                if (OS.windowServer == OS.WS_SYMBIAN_S60) {
                    // TODO: Launch S60 phone application
                }
            }
            String url = getText();
            if ((extraStyle() == EMAIL) && (!getText().trim().toLowerCase().startsWith(emailScheme)))
                url = emailScheme + url;
            OS.QDesktopServices_openUrl(url);
        }
    }

    public void setText(String string) {
        checkWidget();
        if (string == null)
            SWT.error(SWT.ERROR_NULL_ARGUMENT);
        super.setText(string);
        String extraStr = "";
        if (extraStyle() == EMAIL) {
            if (!getText().trim().toLowerCase().startsWith(emailScheme))
                extraStr = emailScheme;

        }
        
        String link = "<a href=" + extraStr + getText() + ">" + getText() + "</a>";
        OS.QLabel_setText(Internal_PackageSupport.handle(this), link);
    }
    
    private final int extraStyle() {
        return Internal_PackageSupport.extraStyle(this);    
    }
}