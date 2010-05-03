/*******************************************************************************
 * Copyright (c) 2000, 2009 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.swt.widgets;

import org.eclipse.swt.internal.symbian.*;
import org.eclipse.swt.*;
import org.eclipse.swt.graphics.*;
import org.eclipse.swt.events.*;

/**
 * Instances of this class represent a selectable
 * user interface object that displays a text with
 * links.
 * <p>
 * <dl>
 * <dt><b>Styles:</b></dt>
 * <dd>(none)</dd>
 * <dt><b>Events:</b></dt>
 * <dd>Selection</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 *
 * @since 3.1, eRCP 1.3
*/
public class Link extends org.eclipse.swt.widgets.Control
{
    private int linkHandle;
    private String text;

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
     * @param parent a composite control which will be the parent of the new instance (cannot be null)
     * @param style the style of control to construct
     *
     * @exception IllegalArgumentException <ul>
     * <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     * </ul>
     * @exception SWTException <ul>
     * <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the parent</li>
     * <li>ERROR_INVALID_SUBCLASS - if this class is not an allowed subclass</li>
     * </ul>
     *
     * @see Widget#checkSubclass
     * @see Widget#getStyle
     */
    public Link(Composite parent, int style)
    {
        super(parent, style);
        super.checkOrientation(parent);
    }

    void checkOrientation(Widget parent)
    {
        style &= ~SWT.MIRRORED;
        // If orientation is not set explicitly, it will be set on native side.
        if ((style & (SWT.LEFT_TO_RIGHT | SWT.RIGHT_TO_LEFT)) != 0)
        {
            style = internal_checkBits(style, SWT.LEFT_TO_RIGHT, SWT.RIGHT_TO_LEFT, 0, 0, 0, 0);
        }
    }
    
    final void internal_setLinkHandle(int linkHandle)
    {
        this.linkHandle = linkHandle;
        handle = OS.Link_ControlHandle(linkHandle);
    }

    protected void internal_createHandle()
    {
        int checkedStyle = internal_checkStyle(internal_getStyle());
        internal_setLinkHandle(
            OS.Link_New(
                this,
                getParent().internal_getCompositeHandle(),
                checkedStyle));
    }

    protected static int internal_checkStyle(int style)
    {
        return internal_checkBits(style, SWT.LEFT, SWT.CENTER, SWT.RIGHT, 0, 0, 0);
    }

    /**
     * Adds the listener to the collection of listeners who will
     * be notified when the control is selected by the user, by sending
     * it one of the messages defined in the <code>SelectionListener</code>
     * interface.
     * <p>
     * <code>widgetSelected</code> is called when the control is selected by the user.
     * <code>widgetDefaultSelected</code> is not called.
     * </p>
     *
     * @param listener the listener which should be notified
     *
     * @exception IllegalArgumentException <ul>
     * <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     * <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     * <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SelectionListener
     * @see #removeSelectionListener
     * @see SelectionEvent
     */
    public void addSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        TypedListener typedListener = new TypedListener(listener);
        addListener(SWT.Selection, typedListener);
        addListener(SWT.DefaultSelection, typedListener);
    }

    /**
     * Removes the listener from the collection of listeners who will
     * be notified when the control is selected by the user.
     *
     * @param listener the listener which should no longer be notified
     *
     * @exception IllegalArgumentException <ul>
     * <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     * <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     * <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see SelectionListener
     * @see #addSelectionListener
     */
    public void removeSelectionListener(SelectionListener listener)
    {
        checkWidget();
        if (listener == null) error(SWT.ERROR_NULL_ARGUMENT);
        if (eventTable == null) return;
        eventTable.unhook(SWT.Selection, listener);
        eventTable.unhook(SWT.DefaultSelection, listener);
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
    public String getText()
    {
        checkWidget();
        return text;
    }

    /**
     * Sets the receiver's text.
     * <p>
     * The string can contain both regular text and hyperlinks. A hyperlink
     * is delimited by an anchor tag, &lt;A&gt; and &lt;/A&gt;. Within an
     * anchor, a single HREF attribute is supported. When a hyperlink is
     * selected, the text field of the selection event contains either the
     * text of the hyperlink or the value of its HREF, if one was specified.
     * In the rare case of identical hyperlinks within the same string, the
     * HREF tag can be used to distinguish between them. The string may
     * include the mnemonic character and line delimiters.
     * </p>
     *
     * @param string the new text
     *
     * @exception IllegalArgumentException <ul>
     * <li>ERROR_NULL_ARGUMENT - if the text is null</li>
     * </ul>
     * @exception SWTException <ul>
     * <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     * <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     */
    public void setText(String string)
    {
        checkWidget();
        if (string == null) internal_error(SWT.ERROR_NULL_ARGUMENT);

        text = string;

        String[] textFragments = { "" };
        String[] linkTargets  = { null };
        if (text.length() > 0)
        {
            // For "<a>x</a>" length is 8, so maxFragmentCount = 2 + 1;
            // + 1 is for plain text shorter than 4 characters
            final int maxFragmentCount = text.length() / 4 + 1;
            String[] parsedTextFragments = new String[maxFragmentCount];
            String[] parsedLinkTargets = new String[maxFragmentCount];
            int fragmentsCount = parse(parsedTextFragments, parsedLinkTargets);
            if (fragmentsCount > 0)
            {
                textFragments = new String[fragmentsCount];
                System.arraycopy(parsedTextFragments, 0, textFragments, 0, fragmentsCount);
                linkTargets = new String[fragmentsCount];
                System.arraycopy(parsedLinkTargets, 0, linkTargets, 0, fragmentsCount);
            }
        }

        OS.Link_SetText(linkHandle, textFragments, linkTargets);
    }

    int parse(String[] textFragments, String[] linkTargets)
    {
        int length = text.length();
        char [] buffer = new char [length];
        text.getChars(0, text.length(), buffer, 0);
        int index = 0, state = 0, fragmentIndex = 0;
        int start = 0, tagStart = 0, linkStart = 0, endtagStart = 0, refStart = 0;
        while (index < length)
        {
            char c = Character.toLowerCase(buffer [index]);
            switch (state)
            {
            case 0:
                if (c == '<')
                {
                    tagStart = index;
                    state++;
                }
                break;
            case 1:
                if (c == 'a') state++;
                break;
            case 2:
                switch (c)
                {
                case 'h':
                    state = 7;
                    break;
                case '>':
                    linkStart = index  + 1;
                    state++;
                    break;
                default:
                    if (c == ' ' || c == '\t' || c=='\n' || c == '\r') break;
                    else state = 13;
                }
                break;
            case 3:
                if (c == '<')
                {
                    endtagStart = index;
                    state++;
                }
                break;
            case 4:
                state = c == '/' ? state + 1 : 3;
                break;
            case 5:
                state = c == 'a' ? state + 1 : 3;
                break;
            case 6:
                if (c == '>')
                {
                    textFragments[fragmentIndex] = new String(buffer, start, tagStart - start);
                    linkTargets[fragmentIndex] = null;
                    fragmentIndex++;

                    textFragments[fragmentIndex] = new String(buffer, linkStart, endtagStart - linkStart);
                    if (linkTargets[fragmentIndex] == null)
                    {
                        linkTargets[fragmentIndex] = new String(buffer, linkStart, endtagStart - linkStart);;
                    }
                    fragmentIndex++;
                    start = tagStart = linkStart = endtagStart = refStart = index + 1;
                    state = 0;
                }
                else
                {
                    state = 3;
                }
                break;
            case 7:
                state = c == 'r' ? state + 1 : 0;
                break;
            case 8:
                state = c == 'e' ? state + 1 : 0;
                break;
            case 9:
                state = c == 'f' ? state + 1 : 0;
                break;
            case 10:
                state = c == '=' ? state + 1 : 0;
                break;
            case 11:
                if (c == '"')
                {
                    state++;
                    refStart = index + 1;
                }
                else
                {
                    state = 0;
                }
                break;
            case 12:
                if (c == '"')
                {
                    linkTargets[fragmentIndex + 1] = new String(buffer, refStart, index - refStart);
                    state = 2;
                }
                break;
            case 13:
                if (c == ' ' || c == '\t' || c=='\n' || c == '\r')
                {
                    state = 0;
                }
                else if (c == '=')
                {
                    state++;
                }
                break;
            case 14:
                state = c == '"' ? state + 1 : 0;
                break;
            case 15:
                if (c == '"') state = 2;
                break;
            default:
                state = 0;
                break;
            }
            index++;
        }
        if (start < length)
        {
            textFragments[fragmentIndex] = new String(buffer, start, length - start);
            linkTargets[fragmentIndex] = null;
            fragmentIndex++;
        }

        return fragmentIndex;
    }
}
