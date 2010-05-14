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

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Internal_PackageSupport;
import org.eclipse.swt.widgets.Listener;

/**
 * This class provides notifications that can be used to determine which type of
 * network connections are active at any given moment. 
 */
public final class NetworkStatus {

	/**
	 * A notification state flag that is raised when there are any active
	 * Ethernet data connections.
	 */
	public static final int DATA_ETHERNET = 0x00000001;

	/**
	 * A notification state flag that is raised when there are any active WLAN
	 * data connections.
	 */
	public static final int DATA_WLAN = 0x00000002;

	/**
	 * A notification state flag that is raised when there are any active CSD,
	 * GPRS, HSCSD, EDGE or cdmaOne data connections.
	 */
	public static final int DATA_2G = 0x00000004; 
	
	/**
	 * A notification state flag that is raised when there are any active CDMA
	 * data connections.
	 */
	public static final int DATA_CDMA2000 = 0x00000008;
	
	/**
	 * A notification state flag that is raised when there are any active
	 * W-CDMA/UMTS data connections.
	 */
	public static final int DATA_WCDMA = 0x00000010;
	
	/**
	 * A notification state flag that is raised when there are any active High
	 * Speed Packet Access data connections.
	 */
	public static final int DATA_HSPA = 0x00000020;

	/**
	 * A notification state flag that is raised when there are any active
	 * Bluetooth data connections.
	 */
	public static final int DATA_BLUETOOTH = 0x00000040;
	
	/**
	 * A notification state flag that is raised when there are any active WiMAX
	 * data connections.
	 */
	public static final int DATA_WIMAX = 0x00000080;
	
	/**
	 * A notification state flag that is raised when there are any active voice
	 * calls.
	 */
	public static final int VOICE_CALL = 0x00000100;

	// This flag is set for any other active types than the ones above. 
	// No events are sent for this type of active connections. 
	private static final int UNKNOWN = 0x80000000;
	
	// The notified states of the connection types
	private static int notifiedStates;
	
	// Singleton instance
	private static NetworkStatus instance;
	
	// References to the listeners of the clients
	private static NetworkStatusListener[] listeners;

	// QNetworkConfigurationManager and XQCallInfo handles. 
	// Can be 0 if required native parts not compiled in. 
	private static int qNetworkConfigurationManagerHandle;
	private static int xqCallInfoHandle;
	
	// QNetworkConfiguration objects for active connections
	private static int activeConfigHandles[];
	
	// The dispose listener that is added to Display
	private static Listener disposeListener;
	
	private NetworkStatus() {
		xqCallInfoHandle = OS.XQCallInfo_create();
		qNetworkConfigurationManagerHandle = OS.QNetworkConfigurationManager_new(0);
		hookEvents();
		addDisposeListener();
		handleNetworkConfigurationChange();
		handleCallInformationChanged();
	}
	
	private static void addDisposeListener() {
		Display display = Internal_PackageSupport.getDisplayInstance();
		disposeListener = new Listener() {
			public void handleEvent(Event event) {
				destroy();
			}
		};
		display.addListener(SWT.Dispose, disposeListener);
	}
	
	private static void removeDisposeListener() {
		if(disposeListener != null) {
			Display display = Internal_PackageSupport.getDisplayInstance();
			if(display != null && !display.isDisposed()) {
				display.removeListener(SWT.Dispose, disposeListener);
				disposeListener = null;
			}
		}
	}
	
	private static void checkThread() {
		Display display = Internal_PackageSupport.getDisplayInstance();
		if(display == null) {
			throw new RuntimeException("Display doesn't exist");
		}
		if(!display.getThread().equals(Thread.currentThread())) {
			throw new RuntimeException("Not the UI thread");
		}
	}
	
	private static NetworkStatus instance() {
		if(instance == null) {
			instance = new NetworkStatus();
		}
		return instance;
	}

	private static boolean hasListeners() {
		if(listeners == null) return false;
		for(int i = 0; i < listeners.length; ++i) {
			if(listeners[i] != null) {
				return true;
			}
		}
		return false;
	}
	
	private static void destroy() {
		destroyActiveConfigs();
		if(qNetworkConfigurationManagerHandle != 0) {
			org.eclipse.swt.internal.qt.QObjectDeleteWrapper.deleteSafely(
					qNetworkConfigurationManagerHandle);
			qNetworkConfigurationManagerHandle = 0;
		}
		if(xqCallInfoHandle != 0) {
			org.eclipse.swt.internal.qt.QObjectDeleteWrapper.deleteSafely(
					xqCallInfoHandle);
			xqCallInfoHandle = 0;
		}
		listeners = null;
		instance = null;
	}

	/**
	 * Adds the listener to the collection of listeners who will be notified of
	 * the network status changes. Can only be called by the eSWT UI thread. If
	 * there are active connections at the time of adding a listener the
	 * listener will be notified. Adding the first listener will automatically
	 * allocate the required native resources and removing the last listener
	 * will automatically free them. This class will hold a strong reference to
	 * the listener object preventing it from getting garbage collected until
	 * the listener is removed.
	 * 
	 * @param listener
	 *            the listener which should be notified when the event occurs
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
	 *                </ul>
	 * 
	 * @exception RuntimeException
	 *                <ul>
	 *                <li>If eSWT Display doesn't exist</li>
	 *                <li>If called in a non-UI thread</li>
	 *                </ul>
	 * 
	 * @see NetworkStatusListener
	 * @see #removeListener(NetworkStatusListener)
	 */
	public static void addListener(NetworkStatusListener listener) {
	    if (listener == null) throw new IllegalArgumentException();
		checkThread();
	    instance();
	    hook(listener);
    	if(notifiedStates != 0) {
    	    final NetworkStatusListener asyncNofityListener = listener; 
    	    final int asyncNotifyStates = notifiedStates;
    		Internal_PackageSupport.getDisplayInstance().asyncExec(new Runnable() {
				public void run() {
					asyncNofityListener.stateChanged(asyncNotifyStates);
				}
    		});
    	}
	}

	/**
	 * Removes the listener from the collection of listeners who will be
	 * notified of the network status changes. Can only be called by the eSWT UI
	 * thread. Removing the listener will release the reference held by this
	 * class to the listener object. When all the listeners have been removed
	 * the native resources allocated by this class are no longer needed and are
	 * automatically freed.
	 * 
	 * @param listener
	 *            the listener which should no longer be notified when the event
	 *            occurs
	 * 
	 * @exception IllegalArgumentException
	 *                <ul>
	 *                <li>ERROR_NULL_ARGUMENT - if the listener is null</li>
	 *                </ul>
	 * 
	 * @exception RuntimeException
	 *                <ul>
	 *                <li>If eSWT Display doesn't exist</li>
	 *                <li>If called in a non-UI thread</li>
	 *                </ul>
	 * 
	 * @see NetworkStatusListener
	 * @see #addListener(NetworkStatusListener)
	 */
	public static void removeListener(NetworkStatusListener listener) {
	    if (listener == null) throw new IllegalArgumentException();
		checkThread();
	    unhook(listener);
	    if(!hasListeners()) {
	    	destroy();
	    	removeDisposeListener();
	    }
	}
	
	// Connect the signals
	private void hookEvents() {
		// Packet data connections
		if(qNetworkConfigurationManagerHandle != 0) {
	        int signalProxy = org.eclipse.swt.internal.qt.OS.SignalHandler_new(
	        		qNetworkConfigurationManagerHandle, this, OS.QSIGNAL_NETWORKCONFIGURATIONCHANGED);
	        org.eclipse.swt.internal.qt.OS.QObject_connectOrThrow(
	        		qNetworkConfigurationManagerHandle, 
	        		"configurationAdded(const QNetworkConfiguration&)", 
	        		signalProxy, "widgetSignal()", 
	        		org.eclipse.swt.internal.qt.OS.QT_AUTOCONNECTION);
	        org.eclipse.swt.internal.qt.OS.QObject_connectOrThrow(
	        		qNetworkConfigurationManagerHandle, 
	        		"configurationChanged(const QNetworkConfiguration&)", 
	        		signalProxy, "widgetSignal()", 
	        		org.eclipse.swt.internal.qt.OS.QT_AUTOCONNECTION);
	        org.eclipse.swt.internal.qt.OS.QObject_connectOrThrow(
	        		qNetworkConfigurationManagerHandle, 
	        		"configurationRemoved(const QNetworkConfiguration&)", 
	        		signalProxy, "widgetSignal()", 
	        		org.eclipse.swt.internal.qt.OS.QT_AUTOCONNECTION);
		}
		
        // Voice calls
        if(xqCallInfoHandle != 0) {
	        int signalProxy = org.eclipse.swt.internal.qt.OS.SignalHandler_new(
	        		xqCallInfoHandle, this, OS.QSIGNAL_CALLINFORMATIONCHANGED);
	        org.eclipse.swt.internal.qt.OS.QObject_connectOrThrow(
	        		xqCallInfoHandle, 
	        		"callInformationChanged()", 
	        		signalProxy, "widgetSignal()", 
	        		org.eclipse.swt.internal.qt.OS.QT_AUTOCONNECTION);
        }
	}
	
	// Connected signals come here
    boolean eventProcess(int widgetHandle, int eventType, int time,
	        int arg1, int arg2, int arg3, int arg4, int arg5, String arg6) {
    	switch(eventType) {
		case OS.QSIGNAL_NETWORKCONFIGURATIONCHANGED:
			handleNetworkConfigurationChange();
			break;
		case OS.QSIGNAL_CALLINFORMATIONCHANGED:
			handleCallInformationChanged();
			break;
	    default:
	    	break;
		}
    	return false;
	}

    private static void destroyActiveConfigs() {
		// Free the QNetworkConfiguration objects
		if(activeConfigHandles != null) {
			for(int i = 0; i < activeConfigHandles.length; ++i) {
				OS.QNetworkConfiguration_delete(activeConfigHandles[i]);
				activeConfigHandles[i] = 0;
			}
			activeConfigHandles = null;
		}
    }
    
    private static void updateActiveConfigs() {
    	destroyActiveConfigs();
		// Get all the currently active configurations
		if(qNetworkConfigurationManagerHandle != 0) {
	    	activeConfigHandles = OS.QNetworkConfigurationManager_allConfigurations(
					qNetworkConfigurationManagerHandle, OS.QNETWORKCONFIGURATION_ACTIVE);
		}
    }
    
	private static void handleNetworkConfigurationChange() {
		updateActiveConfigs();
	
		// Find out the new states of all connection types
		int newStates = 0;
		newStates |= (notifiedStates & VOICE_CALL); // Voice call state didn't change
		for(int i = 0; i < activeConfigHandles.length; ++i) {
			int activeFlag = bearerNameToConnectionFlag(
					OS.QNetworkConfiguration_bearerName(activeConfigHandles[i]));
			if(activeFlag == UNKNOWN) continue;
			newStates |= activeFlag;
		}
		
		notifyChangedStates(newStates);
	}
	
	private static void handleCallInformationChanged() {
		if(xqCallInfoHandle != 0) {
			int newStates = notifiedStates;
			if(OS.XQCallInfo_swt_hasCalls(xqCallInfoHandle)) {
				newStates |= VOICE_CALL;
			} else {
				newStates &= ~VOICE_CALL;
			}
			
			notifyChangedStates(newStates);
		}
	}
	
	private static void notifyChangedStates(int newStates) {
		if(newStates != notifiedStates) {
			notifyListeners(newStates);
		}
	}
	
	private static void notifyListeners(int state) {
		if(listeners != null) {
			for(int i = 0; i < listeners.length; ++i) {
				if(listeners[i] == null) break;
				listeners[i].stateChanged(state);
			}
		}
		notifiedStates = state;
	}
	
	private static int bearerNameToConnectionFlag(String bearerName) {
		if(bearerName.equalsIgnoreCase("WCDMA")) {
			return DATA_WCDMA;
		} else if(bearerName.equalsIgnoreCase("HSPA")) {
			return DATA_HSPA;
		} else if(bearerName.equalsIgnoreCase("2G")) {
			return DATA_2G;
		} else if(bearerName.equalsIgnoreCase("WLAN")) {
			return DATA_WLAN;
		} else if(bearerName.equalsIgnoreCase("Bluetooth")) {
			return DATA_BLUETOOTH;
		} else if(bearerName.equalsIgnoreCase("CDMA2000")) {
			return DATA_CDMA2000;
		} else if(bearerName.equalsIgnoreCase("WiMAX")) {
			return DATA_WIMAX;
		} else if(bearerName.equalsIgnoreCase("Ethernet")) {
			return DATA_ETHERNET;
		}
		return UNKNOWN;
	}
	
	private static void hook(NetworkStatusListener listener) {
		if (listeners == null) listeners = new NetworkStatusListener[1];
		int length = listeners.length, index = length - 1;
		while (index >= 0) {
			if (listeners [index] != null) break;
			--index;
		}
		index++;
		if (index == length) {
			NetworkStatusListener[] newListeners = new NetworkStatusListener[length + 1];
			System.arraycopy (listeners, 0, newListeners, 0, length);
			listeners = newListeners;
		}
		listeners [index] = listener;
	}
	
	private static void unhook(NetworkStatusListener listener) {
		if (listeners == null) return;
		for (int i = 0; i < listeners.length; i++) {
			if (listeners [i] == listener) {
				remove (i);
				return;
			}
		}
	}
	
	private static void remove (int index) {
		int end = listeners.length - 1;
		System.arraycopy (listeners, index + 1, listeners, index, end - index);
		index = end;
		listeners [index] = null;
	}
}
