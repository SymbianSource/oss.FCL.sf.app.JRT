/*******************************************************************************
 * Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials 
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * 
 * Contributors:
 *     Nokia Corporation - initial implementation 
 *******************************************************************************/
package org.eclipse.ercp.swt.mobile;

import org.eclipse.swt.widgets.Control;

/**
 * <p>
 * <b>IMPORTANT:</b> This class is <em>not</em> part of the SWT public API. It
 * must never be accessed from application code.
 * </p>
 */
public class Internal_MobilePackageSupport {
/*
 * From the class ConstrainedText
 */
public static String getSelectionText(ConstrainedText ct) {
    return ct.getSelectionText(); 
}

public static void qt_signal_screen_changed(MobileDevice mobileDevice, int aCount){
	mobileDevice.qt_signal_screen_changed(aCount);
}

public static void qt_swt_event_mobiledevice_orientationchanged(MobileDevice mobileDevice){
	mobileDevice.qt_swt_event_mobiledevice_orientationchanged();
}

public static void qt_swt_event_mobiledevice_changed(MobileDevice mobileDevice, boolean aStatus){
	mobileDevice.qt_swt_event_mobiledevice_changed(aStatus);
}

public static void qt_swt_event_mobiledevice_screen_activate(MobileDevice mobileDevice, boolean aStatus){
	mobileDevice.qt_swt_event_mobiledevice_screen_activate(aStatus);
}

public static void mobiledevice_dispose(MobileDevice mobileDevice){
	mobileDevice.internal_dispose();
}

public static Control control(Command command){
    return command.control();
}

public static int type(Command command){
    return command.type();
}
}
