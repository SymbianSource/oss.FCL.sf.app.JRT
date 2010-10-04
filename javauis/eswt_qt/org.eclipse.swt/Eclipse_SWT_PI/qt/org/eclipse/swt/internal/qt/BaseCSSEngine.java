package org.eclipse.swt.internal.qt;

import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTException;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Widget;

/**
 * Base CSS Engine for eSWT. This engine recognizes only Qt
 * <a href="http://qt.nokia.com/doc/4.6/stylesheet.html">stylesheet</a> syntax.
 *
 * CSS id selectors are supported by setting an id to the widget as follows.
 * <pre>
 *	Button myButton = new Button(composite, SWT.PUSH);
 *	myButton.setData(WidgetConstant.CSS_ID,"myButtonId");
 * </pre>
 *
 * <p><b>NOTE:</b> This class is intended for internal use only.</p>
 */
public final class BaseCSSEngine {
	private static final String FILE_SCHEME = "file:///";
	private Display parent;

	/**
	 * Creates an engine instance.
	 *
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if the parent is null</li>
	 *                <li>ERROR_INVALID_ARGUMENT - if the parent is disposed</li>
	 *                </ul>
	 * @exception SWTException
	 *                <ul>
	 *                <li>ERROR_THREAD_INVALID_ACCESS - if not called from the
	 *                thread that created the parent</li>
	 *                </ul>
	 */
	public BaseCSSEngine(Display display){
		if (display == null) SWT.error (SWT.ERROR_NULL_ARGUMENT);
		if (display.isDisposed ()) SWT.error (SWT.ERROR_INVALID_ARGUMENT);
		if (display.getThread() != Thread.currentThread()) SWT.error (SWT.ERROR_THREAD_INVALID_ACCESS);
		this.parent = display;
	}

	/**
	 * Applies the given string containing valid CSS to all the widgets created for the Display.
	 * CSS used in this method can not have any relative urls (for example for imports and images).
	 * Relative urls will not be resolved.
	 *
	 *<p><b>NOTE:</b> This is intended for internal use.</p>
	 *
	 * @param style
	 * @exception IllegalArgumentException
	 * <ul>
 	 *    <li>ERROR_NULL_ARGUMENT - if style is null</li>
 	 *    <li>ERROR_INVALID_ARGUMENT - if style is not a valid style string</li>
 	 * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
	 */
	public void applyCSS( String style ){
		checkEngine();
		if( style == null ) SWT.error (SWT.ERROR_NULL_ARGUMENT);
		if( style.startsWith(FILE_SCHEME) ) SWT.error (SWT.ERROR_INVALID_ARGUMENT);
		OS.QApplication_setStyleSheet(style);
	}

	/**
	 * Applies the given string containing valid CSS to the widget.
	 * CSS used in this method can not have any relative urls (for example for imports and images).
	 * Relative urls will not be resolved.
	 *
	 * <p><b>NOTE:</b> This is intended for internal use.</p>
	 *
	 * @param style
	 * @exception IllegalArgumentException
	 * <ul>
 	 *    <li>ERROR_NULL_ARGUMENT - if style is null</li>
 	 *    <li>ERROR_INVALID_ARGUMENT - if style is not a valid style string</li>
 	 * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver or the widget has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
	 */
	public void applyCSS(Widget widget, String style ){
		checkEngine();
		if( style == null ) SWT.error (SWT.ERROR_NULL_ARGUMENT);
		if( widget == null ) SWT.error (SWT.ERROR_NULL_ARGUMENT);
		if( widget.isDisposed() ) SWT.error( SWT.ERROR_WIDGET_DISPOSED);
		if( style.startsWith(FILE_SCHEME) ) SWT.error (SWT.ERROR_INVALID_ARGUMENT);
		OS.QWidget_setStyleSheet(Internal_PackageSupport.handle(widget), style);
	}
	/**
	 * Loads a file containing valid CSS and applies to all the widgets for Display.
	 * This function bypasses java security checks for accessing the file.
	 *
	 * <p><b>NOTE:</b> This is intended for internal use.</p>
	 *
	 * @param style
	 * @exception IllegalArgumentException <ul>
 	 *    <li>ERROR_NULL_ARGUMENT - if the file name is null</li>
 	 * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
	 */
	public void loadCSS( String filename ){
		checkEngine();
		if (filename == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
		OS.QApplication_setStyleSheet(FILE_SCHEME+filename);
	}

	/**
	 * Loads a file containing valid CSS and applies to the widget.
	 * This function bypasses Java security checks for accessing the file.
	 *
	 * <p><b>NOTE:</b> This is intended for internal use.</p>
	 *
	 * @param style
	 * @exception IllegalArgumentException
	 * <ul>
 	 *    <li>ERROR_NULL_ARGUMENT - if the file name is null</li>
 	 * </ul>
     * @exception SWTException <ul>
     *    <li>ERROR_WIDGET_DISPOSED - if the receiver or the widget has been disposed</li>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
	 */
	public void loadCSS( Widget widget, String filename ){
		if (filename == null) SWT.error(SWT.ERROR_NULL_ARGUMENT);
		if( widget == null ) SWT.error (SWT.ERROR_NULL_ARGUMENT);
		if( widget.isDisposed() ) SWT.error( SWT.ERROR_WIDGET_DISPOSED);
		OS.QWidget_setStyleSheet(Internal_PackageSupport.handle(widget), FILE_SCHEME+filename);
	}

	/**
	 * Dispose the engine.
	 * @exception SWTException <ul>
     *    <li>ERROR_THREAD_INVALID_ACCESS - if not called from the thread that created the receiver</li>
     * </ul>
	 *
	 */
	public void dispose(){
		if(parent != null) {
			if (parent.getThread() != Thread.currentThread()) SWT.error (SWT.ERROR_THREAD_INVALID_ACCESS);
			parent = null;
		}
	}

	private void checkEngine(){
		if ( parent == null || parent.isDisposed() ) SWT.error( SWT.ERROR_WIDGET_DISPOSED);
	    if ( parent.getThread() != Thread.currentThread() ) SWT.error (SWT.ERROR_THREAD_INVALID_ACCESS);
	}
}
