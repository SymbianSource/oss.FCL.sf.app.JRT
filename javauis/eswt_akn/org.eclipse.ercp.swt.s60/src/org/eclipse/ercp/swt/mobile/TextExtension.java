/*******************************************************************************
 * Copyright (c) 2004, 2007 Nokia Corporation.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Yu You (Nokia Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.ercp.swt.mobile.internal.*;


/**
 * This class contains methods for extending the functionality of the Text
 * control. The functionality is specific to non-full keyboard devices.
 * Applications can request certain text input modes when a TextExtension
 * control gets focused. The effective input mode takes into account two style
 * aspects. One aspect is based on the content semantics. The other is based on
 * the content character set.
 * </p>
 * <p>
 * The editing behavior and appearance are otherwise identical to the
 * <code>Text</code> control. The differences in TextExtension are to ease the
 * possible switch of initial input modes, such as to enable/disable predictive
 * input, changing initial casing styles and particular input modes of some
 * languages. The initial input mode does not persist if changed by the end-user
 * during editing. Whether the mode will be persist during the application
 * life-cycle is implementation-dependent.
 * </p>
 *
 * <dl>
 * <dt><b>TextExtension Styles: </b></dt>
 * <dl>
 * <dt><b>Content Styles (exclusive): </b></dt>
 * <dd>EMAILADDRESS: an e-mail address</dd>
 * <dd>URL: a web address</dd>
 * <p>
 * These are different semantic styles but basically all characters are allowed
 * in each style. The implementation may use the content style to implement a
 * device-dependent UI for assisting user input, for example, providing a UI to
 * access the device's PIM or browser bookmarks, if available.
 * </p>
 * <dt><b>Input Modifier Styles: </b></dt>
 * <dd>NON_PREDICTIVE: hint for turning off possible predictive text input. By
 * default any predictive input facilities should be turned on if available
 * </dd>
 * <dd>LATIN_INPUT_ONLY: force that locale specific input modes should not be
 * available. This is used in some situations when only Latin characters are
 * allowed, for example, password text field</dd>
 * </dl>
 * </dl>
 * <dl>
 * <dt><b>Casing Modifiers (exclusive): </b> Casing modifiers define which case modes
 * are available for input. Only one casing modifier may be specified.</dt>
 * <dd>UPPERCASE: The capital letters of a typeface</dd>
 * <dd>LOWERCASE: The small letters of a typeface, as opposed to the capital
 * letters, or uppercase letters</dd>
 * <dd>TEXTCASE: The first word of a sentence is capitalized. Note:
 * Determination of where a sentence ends is implementation-dependent.</dd>
 * <dd>TITLECASE: Every word is capitalized.</dd>
 * </dl>
 * <dl>
 * <dt><b>Events: </b></dt>
 * <dd>(none)</dd>
 * </dl>
 * <p>
 * IMPORTANT: This class is <em>not</em> intended to be subclassed.
 * </p>
 */
public class TextExtension extends Text
{

    /**
     * E-mail address.
     */
    public static final int EMAILADDRESS = 1 << 1;

    /**
     * Uniform Resource Locator.
     */
    public static final int URL = 1 << 2;

    /**
     * Hint for turning off predictive text input. By default any predictive
     * input facilities should be turned on if available.
     */
    public static final int NON_PREDICTIVE = 1 << 3;

    /**
     * Force that locale specific input modes should not be available.
     */
    public static final int LATIN_INPUT_ONLY = 1 << 4;

    /**
     * The capital letters of a typeface.
     */
    public static final int UPPERCASE = 1;

    /**
     * The lower case letters of a typeface.
     */
    public static final int LOWERCASE = 2;

    /**
     * The first word of each sentence is capitalized.
     */
    public static final int TEXTCASE = 3;

    /**
     * Every word is capitalized.
     */
    public static final int TITLECASE = 4;

    private int textExtensionHandle;
    private int extensionStyle;
    private int casingModifier = 0;

    /**
     * Constructs a new instance of <code>Text</code> class when specific
     * styles are not used.
     * <p>
     * The <code>style</code> value is either one of the style constants defined in class
     * <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. Style bits are also inherited from superclasses.
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
     * @see SWT#SINGLE
     * @see SWT#MULTI
     * @see SWT#READ_ONLY
     * @see SWT#WRAP
     * @see org.eclipse.swt.widgets.Text#Text(Composite, int)
     */
    public TextExtension(Composite parent, int style)
    {
        this(parent, style, 0);
    }

    /**
     * Constructs a new instance of this class given its parent, a style value
     * describing behaviour and appearance, and an additional style specifying
     * the text types defined above.
     * <p>
     * The <code>style</code> value is either one of the style constants defined
     * in class <code>SWT</code> which is applicable to instances of this class, or
     * must be built by <em>bitwise OR</em> 'ing together (that is, using the
     * <code>int</code> "|" operator) two or more of those <code>SWT</code>
     * style constants. Style bits are also inherited from superclasses.
     * </p>
     * <p>
     * The <code>extensionStyle</code> value is a <em>bitwise OR</em> 'ing of
     * EMAILADDRESS, URL, NON_PREDICTIVE, and LATIN_INPUT_ONLY. EMAILADDRESS and
     * URL may not be both specified. The <code>extensionStyle</code> value is ignored
     * when <code>SWT.READ_ONLY</code> is specified in the <code>style</code> value.
     * </p>
     *
     * @param parent
     *            a composite control which is the parent of the new instance
     *            (cannot be null)
     * @param style
     *            the style of control to construct
     * @param extensionStyle
     *            the TextExtension specific style.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
     *                <li>ERROR_INVALID_ARGUMENT - if either the style or the
     *                extensionStyle is invalid</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the parent</li>
     *                <li>ERROR_INVALID_SUBCLASS - if this class is not an
     *                allowed subclass</li>
     *                </ul>
     * @see SWT#SINGLE
     * @see SWT#MULTI
     * @see SWT#READ_ONLY
     * @see SWT#WRAP
     * @see org.eclipse.swt.widgets.Text#Text(Composite, int)
     */
    public TextExtension(Composite parent, int style, int extensionStyle)
    {
        super(parent, checkStyle(style, extensionStyle));
        this.extensionStyle = extensionStyle;
        OS.TextExtension_SetTypeStyle(textExtensionHandle, extensionStyle);
        if ((extensionStyle & (EMAILADDRESS)) != 0)
        {
            setTextLimit(150);
        }
        else if ((extensionStyle & (URL)) != 0)
        {
            setTextLimit(1000);
        }
    }

    private static int checkStyle(int style, int extensionStyle)
    {
        // Check extension style is only made of valid bits
        if ((extensionStyle & ~(EMAILADDRESS | URL | NON_PREDICTIVE | LATIN_INPUT_ONLY)) != 0)
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        // Cannot have EMAILADDRESS and URL at the same time
        if ((extensionStyle & (EMAILADDRESS | URL)) == (EMAILADDRESS | URL))
        {
            SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        }

        // EMAILADDRES or URL force single line
        if ((extensionStyle & (EMAILADDRESS|URL)) != 0)
        {
            style |= SWT.SINGLE;
        }

        return style;
    }

    protected void internal_createHandle()
    {
        textExtensionHandle = OS.TextExtension_New(this, getParent().internal_getCompositeHandle(), internal_getStyle());
        internal_setTextHandle(OS.TextExtension_TextHandle(textExtensionHandle));
    }

    /**
     * Hints to the implementation as to the input mode that should be used when
     * the user initiates editing of this control.
     *
     * <h3>Input mode string format</h3>
     * <p>
     * The initial input mode is specified as a string and can be null. A
     * "Unicode character block" may be specified by adding the prefix "UCB_" to
     * the names of Unicode character blocks as defined in the J2SE class
     * <code>java.lang.Character.UnicodeBlock</code>. Any Unicode character
     * block can be named in this fashion. For convenience, the most common
     * Unicode character blocks are listed below.
     * </p>
     * <blockquote><code>
     * UCB_BASIC_LATIN<br>UCB_GREEK<br>UCB_CYRILLIC<br>UCB_ARMENIAN<br>UCB_HEBREW<br>
     * UCB_ARABIC<br>UCB_DEVANAGARI<br>UCB_BENGALI<br>UCB_THAI<br>UCB_HIRAGANA<br>UCB_KATAKANA<br>
     * UCB_HANGUL_SYLLABLES<br></code> </blockquote>
     *
     * <p>
     * An "Input subset" may be specified by adding the prefix "IS_" to the name
     * of input subsets as defined by the J2SE class
     * <code>java.awt.im.InputSubset</code>. Any defined input subset can be
     * used. For convenience, the names of the currently defined input subsets
     * are listed below.
     * </p>
     * <blockquote><code>
     * IS_FULLWIDTH_DIGITS<BR>IS_FULLWIDTH_LATIN<BR>IS_HALFWIDTH_KATAKANA<BR>IS_HANJA<br>
     * IS_KANJI<BR>IS_LATIN<BR>IS_LATIN_DIGITS<BR>IS_SIMPLIFIED_HANZI<BR>IS_TRADITIONAL_HANZI
     * </code> </blockquote>
     * <p>
     * Note: Implementations need not compile in support for all the strings
     * listed above. Instead, they need only to compile in the strings that name
     * Unicode character subsets that they support. If the subset name passed by
     * the application is not known by the implementation, the request is
     * gracefully ignored, and a default input mode based on current locale
     * configuration is used.
     * </p>
     *
     * @param mode
     *            a string naming a Unicode character block, input subset, or
     *            null
     *
     * @param casingModifier
     *            an int value. Must be one of UPPERCASE, LOWERCASE, TEXTCASE or
     *            TITLECASE.
     *
     * @exception IllegalArgumentException
     *                <ul>
     *                <li>ERROR_INVALID_ARGUMENT - if the casingModifer is not
     *                one of UPPERCASE, LOWERCASE, TEXTCASE or TITLECASE</li>
     *                </ul>
     * @exception SWTException
     *                <ul>
     *                <li>ERROR_WIDGET_DISPOSED - if the receiver has been
     *                disposed</li>
     *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
     *                thread that created the receiver</li>
     *                </ul>
     */
    public void setInitialInputMode(int casingModifier, String mode)
    {
        checkWidget();
        switch (casingModifier)
        {
        case LOWERCASE:
        case UPPERCASE:
        case TEXTCASE:
        case TITLECASE:
            this.casingModifier = casingModifier;
            break;
        default:
            internal_error(SWT.ERROR_INVALID_ARGUMENT);
            break;
        }

        OS.TextExtension_SetInitialInputMode(textExtensionHandle, this.casingModifier, mode);
    }
}