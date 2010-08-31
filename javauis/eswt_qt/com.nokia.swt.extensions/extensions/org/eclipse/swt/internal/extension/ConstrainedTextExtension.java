package org.eclipse.swt.internal.extension;

import org.eclipse.ercp.swt.mobile.ConstrainedText;
import org.eclipse.ercp.swt.mobile.Internal_MobilePackageSupport;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.internal.qt.OS;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Listener;

/**
 * An extension class to ConstrainedText.
 * 
 * <p>
 * To listen to caret movement or selection change events, create listeners
 * of type <code>SWTExtension.CaretMoved</code> or 
 * <code>SWTExtension.ContentSelected</code>. 
 * </p><p>
 * Here is an example of using a <code>ConstrainedTextExtension</code> 
 * with the extended listeners:
 * <code><pre>
 * ConstrainedTextExtension text = new ConstrainedTextExtension(shell, SWT.LEFT, ConstrainedText.DECIMAL);
 * text.setText("123456789");
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
public class ConstrainedTextExtension extends ConstrainedText {
    
public ConstrainedTextExtension(Composite parent, int style, int constrainedStyle) {
    super(parent, style, constrainedStyle, true);
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

/**
 * Returns the number of lines.
 *
 * @return the number of lines in the widget
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getLineCount() {
    checkWidget();
    return 1;
}

/**
 * Returns the height of a line.
 *
 * @return the height of a row of text
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getLineHeight () {
    checkWidget();
    return OS.QWidget_QFontMetrics_lineSpacing(Internal_PackageSupport.handle(this));
}

/**
 * Gets the selected text, or an empty string if there is no current selection.
 *
 * @return the selected text
 * 
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public String getSelectionText () {
    return Internal_MobilePackageSupport.getSelectionText(this); 
}

/**
 * Returns the zero-relative index of the line which is currently
 * at the top of the receiver.
 * <p>
 * This index can change when lines are scrolled or new lines are added or removed.
 * </p>
 *
 * @return the index of the top line
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getTopIndex () {
    checkWidget ();
    return 0;
}

/**
 * Returns the top pixel.
 * <p>
 * The top pixel is the pixel position of the line
 * that is currently at the top of the widget.  On
 * some platforms, a text widget can be scrolled by
 * pixels instead of lines so that a partial line
 * is displayed at the top of the widget.
 * </p><p>
 * The top pixel changes when the widget is scrolled.
 * The top pixel does not include the widget trimming.
 * </p>
 *
 * @return the pixel position of the top line
 *
 * @exception SWTException <ul>
 *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
 *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
 * </ul>
 */
public int getTopPixel () {
    checkWidget ();
    return 0;
}
}
