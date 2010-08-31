/*******************************************************************************
 * Copyright (c) 2000, 2008 IBM Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     IBM Corporation - initial API and implementation
 *******************************************************************************/
package org.eclipse.swt.animation;

import org.eclipse.swt.*;
import org.eclipse.swt.animation.internal.qt.OS;
import org.eclipse.swt.internal.qt.QObjectDeleteWrapper;
import org.eclipse.swt.widgets.*;

public abstract class Animation {
	
	/**
	 * Set the loop count to repeat forever until stop is called.
	 */
	public static final int REPEAT_FOREVER = -1;
	
	
	Display display;
	int handle;
	Widget widget;
	int repeatCount = 1;
	
	
	
	void init(Display display){
		this.display = display;
		create();
	}
	
	/*
	 * CompositeAnimations adds the child animations
	 */
	void addChildren() {
	
	}
	
	void updateAnimation () {
	}
	
	
	void setWidget(Widget widget) {
		this.widget = widget;
		hookEvents();
	}
	
	void updateRepeatCount(){
		OS.QAbstractAnimation_setLoopCount(handle, repeatCount);
	}
	
	/*
	 * Create the native animation
	 */
	void create() {	
	}

    void hookEvents() {
        int signalProxy = org.eclipse.swt.internal.qt.OS.SignalHandler_new(handle, display, org.eclipse.swt.internal.qt.OS.QSIGNAL_RETURN_PRESSED);
        org.eclipse.swt.internal.qt.OS.QObject_connectOrThrow(handle, "finished()", signalProxy, "widgetSignal()", org.eclipse.swt.internal.qt.OS.QT_AUTOCONNECTION);
    }
	void checkAnimation() {		
		if (handle==0) SWT.error(SWT.ERROR_WIDGET_DISPOSED);
		if (widget != null && widget.isDisposed()) SWT.error(SWT.ERROR_WIDGET_DISPOSED);
	}

	
	void release() {
		if (handle != 0){
			 QObjectDeleteWrapper.deleteSafely( handle );
			 handle = 0;
		}

	}
	
	public void start(Widget widget) {
		if( widget== null ) SWT.error(SWT.ERROR_NULL_ARGUMENT);
		checkAnimation();
		setWidget(widget);
		addChildren();
		updateAnimation();
		updateRepeatCount();
		OS.QAbstractAnimation_start(handle);
	}

	public void setRepeatCount(int repeatCount) {
		checkAnimation();
		if (repeatCount == 0) SWT.error(SWT.ERROR_INVALID_ARGUMENT);
		this.repeatCount = repeatCount;
	}

	
	public void stop() {
		checkAnimation();
		if (widget != null) OS.QAbstractAnimation_stop(handle);
	}	
	
	public void dispose() {
		if (handle==0) return;
		release();
	}
	
	public void addAnimationListener(AnimationListener listener){
		
	}
	
	public void removeAnimationListener(AnimationListener listener){
		
	}
	
}
