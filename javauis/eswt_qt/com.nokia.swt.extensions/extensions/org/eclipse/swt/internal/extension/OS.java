/*******************************************************************************
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - initial implementation
 *******************************************************************************/
package org.eclipse.swt.internal.extension;

public final class OS {
	public static final int QSIGNAL_NETWORKCONFIGURATIONCHANGED = 1;
	public static final int QSIGNAL_CALLINFORMATIONCHANGED = 2;
	
	public static final int QNETWORKCONFIGURATION_ACTIVE = 0x000000e;
	
	//
	// QNetworkConfigurationManager
	//
    public static final native int QNetworkConfigurationManager_new(int parent);
    public static final native int[] QNetworkConfigurationManager_allConfigurations(int handle, int filter);
    
    //
    // QNetworkConfiguration
    //
    public static final native String QNetworkConfiguration_bearerName(int handle);
    public static final native void QNetworkConfiguration_delete(int handle);
    
    //
    // XQCallInfo
    //
    public static final native int XQCallInfo_create();
    public static final native boolean XQCallInfo_swt_hasCalls(int handle);
}
