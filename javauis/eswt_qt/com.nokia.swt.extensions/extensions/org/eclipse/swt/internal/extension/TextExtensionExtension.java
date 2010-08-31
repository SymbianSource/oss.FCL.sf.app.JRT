package org.eclipse.swt.internal.extension;

import org.eclipse.ercp.swt.mobile.TextExtension;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Listener;

/**
 * An extension class to TextExtension.
 * 
 * <p>
 * of type <code>SWTExtension.CaretMoved</code> or 
 * <code>SWTExtension.ContentSelected</code>. 
 * </p><p>
 * Here is an example of using a <code>TextExtensionExtension</code> 
 * with the extended listeners:
 * <code><pre>
 * TextExtensionExtension text = new TextExtensionExtension(shell, SWT.LEFT, TextExtension.EMAILADDRESS);
 * text.setText("address@site.com");
 *      
 * text.addListener(SWTExtension.CaretMoved, new Listener() {
 *     public void handleEvent(Event event) {
 *         System.out.println("Caret moved, new position "+event.x);
 *     }});
 *      
 * text.addListener(SWTExtension.ContentSelected, new Listener() {
 *     public void handleEvent(Event event) {
 *         System.out.println("Selection changed, pos "+event.x+", len "+event.count);
 *     }});
 * </pre></code></p>
 */
public class TextExtensionExtension extends TextExtension {
    
	// Bit set to make sure API extension styles don't overlap with the official API
	private static final int TEXTEXTENSIONEXTENSIONSTYLEBIT = 0x80000000;
	
	private int validator;
	
	/**
	 * NUMERIC, DECIMAL, PHONENUMBER values can be used in the extensionStyle
	 * constructor parameter to set the corresponding constraints. In the
	 * eSWT API these constraints are only supported by ConstrainedText which
	 * is a single line editor. For a multi-line editor with these constraints
	 * this API extension can be used. 
	 */
	public static final int NUMERIC = 1|TEXTEXTENSIONEXTENSIONSTYLEBIT;
	public static final int DECIMAL = 2|TEXTEXTENSIONEXTENSIONSTYLEBIT;
	public static final int PHONENUMBER = 3|TEXTEXTENSIONEXTENSIONSTYLEBIT;
	
    public TextExtensionExtension(Composite parent, int style) {
        this(parent, style, 0);
    }

    public TextExtensionExtension(Composite parent, int style, int extensionStyle) {
        super(parent, style, checkExtensionStyle(extensionStyle), true);
        int topHandle = Internal_PackageSupport.topHandle(this);
    	int newHints, oldHints;
    	newHints = oldHints = OS.QWidget_inputMethodHints(topHandle);
        switch(extensionStyle) {
        case TextExtensionExtension.NUMERIC:
        	newHints |= OS.QT_IMHDIGITSONLY;
        	// Intermediate restriction: anything that builds up to a valid integer is allowed
            // I.E.: You can have + alone, since it is the beginning of a positive integer
            validator = OS.QRegExpValidator_new(topHandle, "[\\+\\-]?\\d*");
        	break;
        case TextExtensionExtension.DECIMAL:
        	newHints |= OS.QT_IMHFORMATTEDNUMBERSONLY;
        	// Intermediate restriction: anything that builds up to a valid real is allowed
            // I.E.: You can have '-.' alone, since it is the beginning of a negative real but
            // cannot have 'E' since a exponential notation needs a valid real before E 
            validator = OS.QRegExpValidator_new(topHandle, "[\\+\\-]?\\d*(\\.?(\\d+([eE][\\+\\-]?\\d*)?)?)?");
        	break;
        case TextExtensionExtension.PHONENUMBER:
        	newHints |= OS.QT_IMHDIALABLECHARACTERSONLY;
        	// Very loose restriction: any of the chars in the set allowed in any order and amount
            validator = OS.QRegExpValidator_new(topHandle, "[\\+\\d\\sABCD,TP!W@;>IiGg\\*\\#\\.\\(\\)\\/\\-]*");
        	break;
    	default:
       		break;
        }
        if(oldHints != newHints) {
        	OS.QWidget_setInputMethodHints(topHandle, newHints);
        }
    }
    
    /**
     * Adds the listener to the collection of listeners who will
     * be notified when an event of the given type occurs. When the
     * event does occur in the widget, the listener is notified by
     * sending it the <code>handleEvent()</code> message. The event
     * type is one of the event constants defined in class <code>SWT</code>.
     * 
     * To listen to caret moved event, add a listener of type 
     * <code>SWTExtension.CaretMoved</code>. New position of the caret is placed
     * in <code>event.x</code>.
     *
     * To listen to content selection event, add a listener of type
     * <code>SWTExtension.ContentSelected</code>. Index of the first selected 
     * character is placed in <code>event.x</code>, number of selected characters 
     * is placed in <code>event.count</code>.
     *
     * @param eventType the type of event to listen for
     * @param listener the listener which should be notified when the event occurs
     *
     * @exception IllegalArgumentException <ul>
     *    <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
     * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
     *
     * @see Listener
     * @see SWTExtension
     * @see Event
     * @see #removeListener(int, Listener)
     * @see #notifyListeners
     */
    public void addListener(int eventType, Listener listener) {
        super.addListener(eventType, listener);
    }
    
    private static final int checkExtensionStyle(int style) {
    	style &= ~(NUMERIC|DECIMAL|PHONENUMBER);
    	return style;
    }
    
    public String fetchPhoneNumber() {
        return Internal_PackageSupport.getPhoneNumber(this);
    }
    
    public void launchDialer(String phoneNumber) {
        int serviceRequest = OS.XQServiceRequest_new(
                "com.nokia.services.serviceapp.Dialer", "dial(QString, bool)",
                false);
        if (serviceRequest < 1) {
            return;
        }
        OS.XQServiceRequest_swt_setArgumentsForDial(serviceRequest, phoneNumber,
                false);
        OS.XQServiceRequest_send(serviceRequest);
        QObjectDeleteWrapper.deleteSafely(serviceRequest);
     }    

	/**
	 * Doesn't do anything if one of NUMERIC, DECIMAL, PHONENUMBER was
	 * specified. Otherwise calls TextExtension.setInitialInputMode. 
	 */
	public void setInitialInputMode(int casingModifier, String mode) {
		int hints = OS.QWidget_inputMethodHints(Internal_PackageSupport.topHandle(this));
		if((hints & (OS.QT_IMHDIGITSONLY|
				     OS.QT_IMHFORMATTEDNUMBERSONLY|
				     OS.QT_IMHDIALABLECHARACTERSONLY)) == 0) {
			super.setInitialInputMode(casingModifier, mode);
		}
	}
	
	public void setText (String string) {
		checkWidget();
		if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.PHONENUMBER|TextExtensionExtension.DECIMAL|
				 TextExtensionExtension.NUMERIC)) != 0) {
			 boolean valid = OS.QValidator_validate(validator, string);
			 if (valid) {
				 super.setText(string);
			 }
			 else if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.DECIMAL|
						 TextExtensionExtension.NUMERIC)) != 0) {
					 SWT.error(SWT.ERROR_INVALID_ARGUMENT);
			 }
		 } else{
			 super.setText(string);
		 }
	}

	public void insert(String string) {
	    checkWidget();
	    if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.PHONENUMBER|
	    		TextExtensionExtension.DECIMAL|TextExtensionExtension.NUMERIC)) != 0) {
			 boolean valid = OS.QValidator_validate(validator, string);
			 if (valid) {
				 super.insert(string);
			 }
			 else if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.DECIMAL|
					 TextExtensionExtension.NUMERIC)) != 0) {
					 SWT.error(SWT.ERROR_INVALID_ARGUMENT);
			 }
		 } else{
			 super.insert(string);
		 }
	}

	public void append(String string) {
	    checkWidget();
	    if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.PHONENUMBER|
	    		TextExtensionExtension.DECIMAL|TextExtensionExtension.NUMERIC)) != 0) {
			 boolean valid = OS.QValidator_validate(validator, string);
			 if (valid) {
				 super.append(string);
			 }
			 else if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.DECIMAL|
						 TextExtensionExtension.NUMERIC)) != 0) {
				 SWT.error(SWT.ERROR_INVALID_ARGUMENT);
			 }
		 } else {
			 super.append(string);
		 }
	}

	public void paste() {
	    checkWidget();
	    if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.PHONENUMBER|
	    		TextExtensionExtension.DECIMAL|TextExtensionExtension.NUMERIC)) != 0) {
			 boolean valid = OS.QValidator_validate(validator, OS.QClipboard_text());
			 if (valid) {
				 super.paste();
			 }
			 else if ((Internal_PackageSupport.extraStyle(this) & (TextExtensionExtension.DECIMAL|
						 TextExtensionExtension.NUMERIC)) != 0) {
				 SWT.error(SWT.ERROR_INVALID_ARGUMENT);
			 }
		 }else {
			 super.paste();
		 }
	}
	
}
