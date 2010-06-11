/*******************************************************************************
 * Copyright (c) 2004, 2008 Nokia Corporation and/or its subsidiary(-ies)..
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Yu You (Nokia Corp.) - initial API specification
 *     Nokia Corporation - S60 implementation
 *     Nokia Corporation - QT implementation
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.*;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.PackageProxy;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.internal.qt.SWTExtension;
import org.eclipse.swt.internal.qt.TextUtils;
import org.eclipse.swt.internal.qt.WidgetState;
import org.eclipse.swt.widgets.*;


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
public class TextExtension extends Text {

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



/*
 * Input mode.
 */
static final String UCB_BASIC_LATIN = "UCB_BASIC_LATIN";
static final String UCB_GREEK = "UCB_GREEK";
static final String UCB_CYRILLIC = "UCB_CYRILLIC";
static final String UCB_ARMENIAN = "UCB_ARMENIAN";
static final String UCB_HEBREW = "UCB_HEBREW";
static final String UCB_ARABIC = "UCB_ARABIC";
static final String UCB_DEVANAGARI = "UCB_DEVANAGARI";
static final String UCB_BENGALI = "UCB_BENGALI";
static final String UCB_THAI = "UCB_THAI";
static final String UCB_HIRAGANA = "UCB_HIRAGANA";
static final String UCB_KATAKANA = "UCB_KATAKANA";
static final String UCB_HANGUL_SYLLABLES = "UCB_HANGUL_SYLLABLES";

static final String IS_FULLWIDTH_DIGITS = "IS_FULLWIDTH_DIGITS";
static final String IS_FULLWIDTH_LATIN = "IS_FULLWIDTH_LATIN";
static final String IS_HALFWIDTH_KATAKANA = "HALFWIDTH_KATAKANA";
static final String IS_HANJA = "IS_HANJA";
static final String IS_KANJI = "IS_KANJI";
static final String IS_LATIN = "IS_LATIN";
static final String IS_LATIN_DIGITS = "IS_LATIN_DIGITS";
static final String IS_SIMPLIFIED_HANZI = "IS_SIMPLIFIED_HANZI";
static final String IS_TRADITIONAL_HANZI = "IS_TRADITIONAL_HANZI"; 

static final String emailScheme = "mailto:";
int inputContext; 
private int inputFlags;

private boolean serviceDone;
private String[] returnedData;


/*
 * Used to store the position and length of selected text
 */
Point lastSelection;

static final class TextExtensionPackageProxy extends PackageProxy {
public boolean qt_event_keypress( int widgetHandle, int key, int modifier, int character, int nativeScanCode ) {
    return ((TextExtension)w).qt_event_keypress(widgetHandle, key, modifier, character, nativeScanCode);
}
public boolean qt_event_keyrelease(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    return ((TextExtension)w).qt_event_keyrelease(widgetHandle, key, modifier, character, nativeScanCode);
}
public void createHandle (int index) {
    ((TextExtension)w).createHandle(index);
}
public void qt_signal_requestCompleted( int value ) {
    ((TextExtension)w).qt_signal_requestCompleted(value);
}
public void qt_signal_text_cursorPositionChanged(int oldPos, int newPos) {
    ((TextExtension)w).qt_signal_text_cursorPositionChanged(oldPos, newPos);
}
public void qt_signal_text_selectionChanged() {
    ((TextExtension)w).qt_signal_text_selectionChanged();
}
public void hookEvents() {
    ((TextExtension)w).hookEvents();
}
public String getPhoneNumber() {
    return ((TextExtension)w).getPhoneNumber();
}

}
    
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
public TextExtension(Composite parent, int style) {
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
public TextExtension(Composite parent, int style, int extensionStyle) {
    this(parent, style, extensionStyle, false);
}

/**
 * <p>
 * <b>IMPORTANT:</b> This constructor is <em>not</em> part of the SWT
 * public API. It should never be referenced from application code. 
 * </p>
 */
protected TextExtension(Composite parent, int style, int extensionStyle, boolean isExtended) {
    super(parent, checkStyle(style, extensionStyle), extensionStyle, 
            new TextExtensionPackageProxy(), isExtended);
    if ((extraStyle() & LATIN_INPUT_ONLY) != 0) {
            // create a new latin input context
            // because the default input context can be something else
            int newInputContext = OS.QInputContextFactory_create(
                    OS.QINPUTCONTEXT_COEFEP, topHandle());
            OS.QWidget_setInputContext(topHandle(), newInputContext);
            inputContext = newInputContext;
        } else {
            int handle = Internal_PackageSupport.handle(this);
            OS.QWidget_setAttribute(handle, OS.QT_WA_INPUTMETHODENABLED, true);
        }
   
    if ((extraStyle() & NON_PREDICTIVE) != 0) {
    	inputFlags |= OS.QT_IMHNOPREDICTIVETEXT;
        }

    String cmdTxt = null;
    if ((extraStyle() & EMAILADDRESS) != 0) {
        cmdTxt = SWT.getMessage("fetch_email");
        inputFlags |= OS.QT_IMHEMAILCHARACTERSONLY;
        addCommand(cmdTxt);
    } else if ((extraStyle() & URL) != 0) {
        cmdTxt = SWT.getMessage("fetch_url");
        inputFlags |= OS.QT_IMHURLCHARACTERSONLY;
        addCommand(cmdTxt);
    } 
    
    if ((extraStyle() & ( NON_PREDICTIVE | URL | EMAILADDRESS)) != 0) {
    	inputFlags |= OS.QT_IMHNOPREDICTIVETEXT;
    	OS.QWidget_setInputMethodHints(topHandle(), inputFlags);
    }
}

void addCommand(String str) {
    Command cmd = new Command(this, Command.GENERAL, 0);
    cmd.setText(str);
    cmd.addListener(SWT.Selection, new Listener() {
        public void handleEvent(Event event) {
            if ((extraStyle() & EMAILADDRESS) != 0) {
                String email = fetchEmail();
                if (email != null) {
                    setText(email);
                }
            } else if ((extraStyle() & URL) != 0) {
                fetchUrl();
            }
        }
    });
}

String fetchEmail() {

    int serviceRequest = OS.XQServiceRequest_new(
            "com.nokia.services.phonebookservices.Fetch",
            "fetch(QString,QString,QString)", false);
    if (serviceRequest <= 0) {
        return null;
    }
    OS.XQServiceRequest_swt_setArgumentsForFetchEmail(serviceRequest,
                    SWT.getMessage("fetch_email"), OS.CNT_ACTIONALL,
                    OS.CNT_DISPLAYALL);
    Display display = Internal_PackageSupport.display(this);
    int handler = OS.SignalHandler_new(topHandle(), 
            OS.QSIGNAL_REQUESTCOMPLETED);

    OS.QObject_connectOrThrow(serviceRequest, "requestCompleted(QVariant)",
            handler, "widgetSignal(QVariant)", OS.QT_AUTOCONNECTION);
    boolean success = OS.XQServiceRequest_send(serviceRequest);
    if (success) {
        serviceDone = false;
        while (!serviceDone) {
            if (display.readAndDispatch() == false) {
                display.sleep();
            }
        }
        if (returnedData != null) {
            String email = returnedData[2];
            returnedData = null;
            QObjectDeleteWrapper.deleteSafely(handler);
            QObjectDeleteWrapper.deleteSafely(serviceRequest);
            return email;
        }
    }
    QObjectDeleteWrapper.deleteSafely(handler);
    QObjectDeleteWrapper.deleteSafely(serviceRequest);
    return null;
}

String getPhoneNumber() {

    int serviceRequest = OS.XQServiceRequest_new(
            "com.nokia.services.phonebookservices.Fetch",
            "fetch(QString,QString,QString)", false);
    if (serviceRequest <= 0) {
        return null;
    }
    OS.XQServiceRequest_swt_setArgumentsForFetchEmail(serviceRequest,
                    SWT.getMessage("fetch_phonenumber"), OS.CNT_ACTIONALL,
                    OS.CNT_DISPLAYALL);
    Display display = Internal_PackageSupport.display(this);
    int handler = OS.SignalHandler_new(topHandle(), 
            OS.QSIGNAL_REQUESTCOMPLETED);

    OS.QObject_connectOrThrow(serviceRequest, "requestCompleted(QVariant)",
            handler, "widgetSignal(QVariant)", OS.QT_AUTOCONNECTION);
    boolean success = OS.XQServiceRequest_send(serviceRequest);
    if (success) {
        serviceDone = false;
        while (!serviceDone) {
            if (display.readAndDispatch() == false) {
                display.sleep();
            }
        }
        if (returnedData != null) {
            String email = returnedData[1];
            returnedData = null;
            QObjectDeleteWrapper.deleteSafely(handler);
            QObjectDeleteWrapper.deleteSafely(serviceRequest);
            return email;
        }
    }
    QObjectDeleteWrapper.deleteSafely(handler);
    QObjectDeleteWrapper.deleteSafely(serviceRequest);
    return null;
}


private void fetchUrl(){
// TODO: implement fetching bookmarks when relevant service is available.
}

private static int checkStyle(int style, int extensionStyle) {
    if ((style & SWT.READ_ONLY) != 0) {
        // ignore extension styles
        extensionStyle = 0;
    }

    // Check extension style is only made of valid bits
    if ((extensionStyle & ~(EMAILADDRESS | URL | NON_PREDICTIVE | LATIN_INPUT_ONLY)) != 0) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    // Cannot have EMAILADDRESS and URL at the same time
    if ((extensionStyle & (EMAILADDRESS | URL)) == (EMAILADDRESS | URL)) {
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
    }
    // EMAILADDRES or URL force single line
    if ((extensionStyle & (EMAILADDRESS | URL)) != 0) {
        style |= SWT.SINGLE;
    }

    return style;
}

void createHandle (int index) {
    if(isExtended()) {
        lastSelection = new Point(0,0);
    }
    
    // QT's single line editor is not capable of handling char formatting
    int variant = (getStyle() & SWT.PASSWORD) != 0 ? TextUtils.LINE_EDIT : TextUtils.TEXT_EDIT;
    Internal_PackageSupport.setVariant(this, variant);
    
    Internal_PackageSupport.createHandle(this, index);
}

void hookEvents () {
    Internal_PackageSupport.hookEvents(this);
    if(isExtended()) {
        Display display = Internal_PackageSupport.display(this);
        int handler1 = OS.SignalHandler_new(topHandle(), OS.QSIGNAL_TEXT_CURSORPOSITIONCHANGED);
        String s;
        if(Internal_PackageSupport.variant(this) == TextUtils.LINE_EDIT) {
        	s = "cursorPositionChanged(int, int)";         	
        } else {
        	s = "cursorPositionChanged()"; 
        }
        OS.QObject_connectOrThrow(topHandle(), s,
            handler1, "widgetSignal()", OS.QT_AUTOCONNECTION);
        int handler2 = OS.SignalHandler_new(topHandle(), OS.QSIGNAL_TEXT_SELECTIONCHANGED);
        OS.QObject_connectOrThrow(topHandle(), "selectionChanged()",
            handler2, "widgetSignal()", OS.QT_AUTOCONNECTION);
    }
}

String inputContextKey(String mode) {
    if (mode == null || mode.length() < 1)
        return null;
    String strMode = mode.trim().toUpperCase();
    String key = null;
    if (strMode.equals(UCB_BASIC_LATIN)) {
        key = OS.QINPUTCONTEXT_COEFEP;
    } else if (mode.equals(IS_FULLWIDTH_DIGITS)) {
        key = OS.QINPUTCONTEXT_COEFEP;
    } else if (mode.equals(IS_FULLWIDTH_LATIN)) {
        key = OS.QINPUTCONTEXT_COEFEP;
    } else if (strMode.equals(IS_LATIN)) {
        key = OS.QINPUTCONTEXT_COEFEP;
    } else if (mode.equals(IS_LATIN_DIGITS)) {
        key = OS.QINPUTCONTEXT_COEFEP;
    } else {
        String lang = language(strMode);
        if (lang != null) {
            key = OS.QInputContextFactory_swt_key(lang);
        }
    }
    return key;
}

private final boolean isExtended() {
    return ((Internal_PackageSupport.state(this) & WidgetState.IS_EXTENDED) != 0);
}

String language(String mode) {
    String lang = null;
    if (mode.equals(UCB_GREEK)) {
        lang = "el";
    } else if (mode.equals(UCB_CYRILLIC)) {
        lang = "sr";
    } else if (mode.equals(UCB_ARMENIAN)) {
        lang = "hy";
    } else if (mode.equals(UCB_HEBREW)) {
        lang = "he";
    } else if (mode.equals(UCB_ARABIC)) {
        lang = "ar";
    } else if (mode.equals(UCB_DEVANAGARI)) {
        lang = "hi";
    } else if (mode.equals(UCB_BENGALI)) {
        lang = "bn";
    } else if (mode.equals(UCB_THAI)) {
        lang = "th";
    } else if (mode.equals(UCB_HIRAGANA)) {
        lang = "ja";
    } else if (mode.equals(UCB_KATAKANA)) {
        lang = "ja";
    } else if (mode.equals(UCB_HANGUL_SYLLABLES)) {
        lang = "ko";
    } else if (mode.equals(IS_HALFWIDTH_KATAKANA)) {
        lang = "ja";
    } else if (mode.equals(IS_HANJA)) {
        lang = "ko";
    } else if (mode.equals(IS_KANJI)) {
        lang = "ja";
    } else if (mode.equals(IS_SIMPLIFIED_HANZI)) {
        lang = "zh-CN;zh-SG";
    } else if (mode.equals(IS_TRADITIONAL_HANZI)) {
        lang = "zh-TW;zh-HK";
    }
    return lang;
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
public void setInitialInputMode(int casingModifier, String mode) {
    checkWidget();
    if ((Internal_PackageSupport.style(this) & SWT.READ_ONLY) != 0)
        return;

    if ((extraStyle() & LATIN_INPUT_ONLY) == 0) {
        String key = inputContextKey(mode);
        int newInputContext = 0;
        if (key != null) {
            newInputContext = OS.QInputContextFactory_create(key,
                    topHandle());
        }
        if (newInputContext != 0 || inputContext != 0) {
            // OS.QWidget_setInputContext also deletes input context
            // which was set previously
            OS.QWidget_setInputContext(topHandle(),
                    newInputContext);
        }
        inputContext = newInputContext;
    }

    if (variant() == TextUtils.LINE_EDIT) {
        return;
    }

    switch (casingModifier) {
    case LOWERCASE:
        inputFlags |= OS.QT_IMHPREFERLOWERCASE;
        break;
    case UPPERCASE:
        inputFlags |= OS.QT_IMHPREFERUPPERCASE;
        break;
    case TEXTCASE:
//        nativeCasing = OS.QFONT_MIXEDCASE;
        break;
    case TITLECASE:
//        inputFlags |= OS.QT_IMHNOAUTOUPPERCASE;
        break;
    default:
        SWT.error(SWT.ERROR_INVALID_ARGUMENT);
        break;
    }
    
    OS.QWidget_setInputMethodHints(topHandle(), inputFlags);
}

boolean qt_event_keypress(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    return Internal_PackageSupport.qt_event_keypress(this, widgetHandle, key, modifier, character, nativeScanCode);
}

boolean qt_event_keyrelease(int widgetHandle, int key, int modifier, int character, int nativeScanCode) {
    if ((getStyle() & SWT.SINGLE) != 0 && (key == OS.QT_KEY_RETURN || key == OS.QT_KEY_ENTER)) {
        Internal_PackageSupport.sendEvent(this, SWT.DefaultSelection);
    }
    return Internal_PackageSupport.qt_event_keyrelease(this, widgetHandle, key, modifier, character, nativeScanCode);
}


void qt_signal_requestCompleted( int value ) {
    
    if (value < 1) {
        serviceDone = true;
        return;
    }
    returnedData = OS.CntServicesContactList_swt_contacts(value);
    serviceDone = true;
}

/*
 * Extended TextExtension sends CaretMoved events
 */ 
void qt_signal_text_cursorPositionChanged(int oldPos, int newPos) {
    if(isExtended()) {
        // For QTextEdit the signature for cursorPositionChanged is 
        // cursorPositionChanged(), so we cannot rely on the information in the
        // signal
        Event event = new Event();
        event.x = TextUtils.getCaretPosition(variant(), topHandle());
        Internal_PackageSupport.sendEvent(this, SWTExtension.CaretMoved, event);
    }
    Internal_PackageSupport.qt_signal_text_cursorPositionChanged(this, oldPos, newPos);
}

/*
 * Extended TextExtension sends ContentSelected events
 */ 
void qt_signal_text_selectionChanged() {
    if(isExtended()) {
        Point selection = TextUtils.getSelection(variant(), topHandle());
        int length = selection.y - selection.x;
        
        // Qt sends selectionChanged signals when user drags on the text, even
        // if there the selection content does not change. Filter out excessive 
        // events here.
        if (lastSelection.y - lastSelection.x != length) {
            lastSelection = selection;
    
            Event event = new Event();
            event.x = selection.x;
            event.count = length;
            Internal_PackageSupport.sendEvent(this, SWTExtension.ContentSelected, event);
        }
    }
    Internal_PackageSupport.qt_signal_text_selectionChanged(this);
}


private final int topHandle() {
    return Internal_PackageSupport.topHandle(this);
}

private final int extraStyle() {
    return Internal_PackageSupport.extraStyle(this);    
}

private final int variant() {
    return Internal_PackageSupport.variant(this);
}
}
